#include <Snooker/Table/TableGeometry.h>
#include <algorithm>
#include <set>

namespace Snooker
{
namespace
{
bool Fail(std::string* Error,const char* Message) {if(Error)*Error=Message;return false;}
template<class T> const T* Find(const std::vector<T>& Profiles,const std::string& Id)
{
    for(const auto& p:Profiles)if(p.Id==Id)return &p;
    return nullptr;
}
template<class T> bool ValidateProfiles(const std::vector<T>& Profiles,std::string* Error)
{
    if(Profiles.size()>4096)return Fail(Error,"material profile resource limit exceeded");
    std::set<std::string> ids;
    for(const auto& p:Profiles)if(!p.IsValid(Error) || !ids.insert(p.Id).second)
        return Fail(Error,"invalid or duplicate material profile id");
    return true;
}
Vec3d RotateZ(const Vec3d& v,double angle)
{
    return {v.X*std::cos(angle)-v.Y*std::sin(angle),v.X*std::sin(angle)+v.Y*std::cos(angle),v.Z};
}
bool Finite(const Vec3d& v) {return v.IsFinite();}
bool CloseBoundary(TableGeometry& g,Meters tolerance,std::string* Error)
{
    struct Edge {Vec3d A,B;};std::vector<Edge> edges;
    const auto flat=[](Vec3d p){p.Z=0;return p;};
    for(const auto& s:g.Segments)if(s.Feature!=TableFeature::ClothBoundary && s.Feature!=TableFeature::CushionTop)
        edges.push_back({flat(s.Start),flat(s.End)});
    for(const auto& p:g.Pockets)edges.push_back({p.MouthStart,p.MouthEnd});
    if(edges.empty() || edges.size()>10000)return Fail(Error,"closed boundary edge limit");
    struct End {Vec3d Point;std::uint32_t Pocket=0;std::string Id,Version;bool Used=false;};
    std::vector<End> ends;
    for(const auto& edge:edges)for(const auto& point:{edge.A,edge.B}) {
        unsigned degree=0;
        for(const auto& other:edges){degree+=point.NearlyEquals(other.A,tolerance);degree+=point.NearlyEquals(other.B,tolerance);}
        if(degree==1) {
            End end;end.Point=point;
            for(const auto& p:g.Pockets)if(point.NearlyEquals(flat(p.LeftJaw.back()),tolerance) || point.NearlyEquals(flat(p.RightJaw.back()),tolerance)) {
                end.Pocket=p.Definition.Id;end.Id=p.Parameters.Id;end.Version=p.Parameters.ParameterVersion;
            }
            ends.push_back(end);
        } else if(degree!=2)return Fail(Error,"non-manifold table contour");
    }
    // Connect each physical rail end to a distinct outer jaw end. Reject long
    // invented walls: missing rails cannot be silently closed across the table.
    for(;;) {
        double best=0.3;std::size_t a=ends.size(),b=ends.size();
        for(std::size_t i=0;i<ends.size();++i)if(!ends[i].Used && !ends[i].Pocket)
            for(std::size_t j=0;j<ends.size();++j)if(!ends[j].Used && ends[j].Pocket) {
                const double distance=(ends[i].Point-ends[j].Point).Length();
                if(distance<best){best=distance;a=i;b=j;}
            }
        if(a==ends.size())break;
        const auto& jaw=ends[b];
        g.Segments.push_back({TableFeature::PocketJaw,jaw.Pocket,ends[a].Point,jaw.Point,{},jaw.Id,jaw.Version});
        edges.push_back({ends[a].Point,jaw.Point});ends[a].Used=ends[b].Used=true;
    }
    for(const auto& end:ends)if(!end.Used)return Fail(Error,"unmatched table contour endpoint");
    std::vector<bool> used(edges.size(),false);auto point=edges[0].A;const auto origin=point;
    for(std::size_t n=0;n<edges.size();++n) {
        g.ClosedBoundary.push_back(point);bool found=false;
        for(std::size_t i=0;i<edges.size();++i)if(!used[i]) {
            if(point.NearlyEquals(edges[i].A,tolerance)){point=edges[i].B;used[i]=true;found=true;break;}
            if(point.NearlyEquals(edges[i].B,tolerance)){point=edges[i].A;used[i]=true;found=true;break;}
        }
        if(!found || (n+1<edges.size() && point.NearlyEquals(origin,tolerance)))return Fail(Error,"disconnected table boundary");
    }
    if(!point.NearlyEquals(origin,tolerance))return Fail(Error,"table boundary does not close");
    const auto cross=[](const Vec3d& a,const Vec3d& b){return a.X*b.Y-a.Y*b.X;};
    const auto& poly=g.ClosedBoundary;
    for(std::size_t i=0;i<poly.size();++i)for(std::size_t j=i+2;j<poly.size();++j) {
        if(i==0 && j+1==poly.size())continue;
        const auto a=poly[i],b=poly[(i+1)%poly.size()],c=poly[j],d=poly[(j+1)%poly.size()];
        const auto ab=b-a,cd=d-c;const double denominator=cross(ab,cd);
        if(std::abs(denominator)<=1e-14)continue;
        const double t=cross(c-a,cd)/denominator,u=cross(c-a,ab)/denominator;
        if(t>=-tolerance && t<=1+tolerance && u>=-tolerance && u<=1+tolerance)
            return Fail(Error,"self-intersecting closed table boundary");
    }
    return true;
}
}
bool GenerateTableGeometry(const TableDefinition& Table,const std::vector<CushionProfile>& Cushions,
    const std::vector<PocketProfile>& Pockets,const GeometrySettings& Settings,TableGeometry& Output,std::string* Error)
{
    if(!Table.IsValid(Error) || !ValidateProfiles(Cushions,Error) || !ValidateProfiles(Pockets,Error))return false;
    if(!IsFinite(Settings.CoordinateTolerance) || Settings.CoordinateTolerance<=0 ||
       Settings.JawSubdivisions<2 || Settings.JawSubdivisions>256)return Fail(Error,"invalid geometry settings");
    const auto near=[&](double a,double b){return NearlyEqual(a,b,Settings.CoordinateTolerance);};
    const auto onEdge=[&](const Vec3d& p,Vec3d& inward) {
        if(!Table.Cloth.Contains({p.X,p.Y}))return false;
        inward={};
        if(near(p.X,Table.Cloth.Min.X))inward.X+=1;
        if(near(p.X,Table.Cloth.Max.X))inward.X-=1;
        if(near(p.Y,Table.Cloth.Min.Y))inward.Y+=1;
        if(near(p.Y,Table.Cloth.Max.Y))inward.Y-=1;
        const double length=inward.Length();
        if(length==0)return false;
        inward=inward*(1/length);return true;
    };
    TableGeometry g;g.ParameterVersion=Table.ParameterVersion;g.Table=Table.Table;g.Cloth=Table.Cloth;
    g.CushionMaterials=Cushions;g.PocketMaterials=Pockets;
    const Vec3d corners[]={{g.Cloth.Min.X,g.Cloth.Min.Y,0},{g.Cloth.Max.X,g.Cloth.Min.Y,0},
                          {g.Cloth.Max.X,g.Cloth.Max.Y,0},{g.Cloth.Min.X,g.Cloth.Max.Y,0}};
    for(int i=0;i<4;++i) {
        const auto delta=corners[(i+1)%4]-corners[i];const double length=std::hypot(delta.X,delta.Y);
        if(!IsFinite(length) || length<=0)return Fail(Error,"cloth extent overflow");
        g.Segments.push_back({TableFeature::ClothBoundary,0,corners[i],corners[(i+1)%4],{-delta.Y/length,delta.X/length,0},{},{}});
    }
    for(const auto& rail:Table.Cushions) {
        const auto* p=Find(Cushions,rail.ProfileId);
        if(!p)return Fail(Error,"unresolved cushion profile id");
        if(!near(rail.Start.Z,p->ContactHeight) || !near(rail.End.Z,p->ContactHeight))
            return Fail(Error,"table/profile cushion height disagreement");
        const Vec3d delta=rail.End-rail.Start;const double length=std::hypot(delta.X,delta.Y);
        if(!IsFinite(length) || length<=0 || 2*p->PocketTransitionLength>=length)
            return Fail(Error,"invalid rail length or overlapping transitions");
        Vec3d n;
        if(near(rail.Start.Y,rail.End.Y) && near(rail.Start.Y,g.Cloth.Min.Y))n={0,1,0};
        else if(near(rail.Start.Y,rail.End.Y) && near(rail.Start.Y,g.Cloth.Max.Y))n={0,-1,0};
        else if(near(rail.Start.X,rail.End.X) && near(rail.Start.X,g.Cloth.Min.X))n={1,0,0};
        else if(near(rail.Start.X,rail.End.X) && near(rail.Start.X,g.Cloth.Max.X))n={-1,0,0};
        else return Fail(Error,"table-v1 rail must follow an axis-aligned cloth edge");
        if(!g.Cloth.Contains({rail.Start.X,rail.Start.Y}) || !g.Cloth.Contains({rail.End.X,rail.End.Y}))
            return Fail(Error,"rail endpoints outside cloth boundary");
        const Vec3d t=delta*(1/length);
        const Vec3d a=rail.Start+t*p->PocketTransitionLength,b=rail.End-t*p->PocketTransitionLength;
        const auto add=[&](TableFeature feature,const Vec3d& start,const Vec3d& end,const Vec3d& normal) {
            g.Segments.push_back({feature,rail.Id,start,end,normal,p->Id,p->ParameterVersion});
        };
        add(TableFeature::CushionFace,a,b,n);
        add(TableFeature::CushionTop,a,b,TableNormal);
        // End segments flare away from playable cloth; signed profile angle determines departure.
        const double angle=p->PocketTransitionAngle;
        const Vec3d left=a-t*(p->PocketTransitionLength*std::cos(angle))-n*(p->PocketTransitionLength*std::sin(angle));
        const Vec3d right=b+t*(p->PocketTransitionLength*std::cos(angle))-n*(p->PocketTransitionLength*std::sin(angle));
        if(p->PocketTransitionLength>0) {
            add(TableFeature::PocketTransition,left,a,t*(-std::sin(angle))+n*std::cos(angle));
            add(TableFeature::PocketTransition,b,right,t*std::sin(angle)+n*std::cos(angle));
        }
    }
    for(const auto& definition:Table.Pockets) {
        const auto* p=Find(Pockets,definition.ProfileId);
        if(!p)return Fail(Error,"unresolved pocket profile id");
        if(!near(definition.Opening,p->OpeningWidth) || !near(definition.Depth,p->BottomDepth))
            return Fail(Error,"table/profile pocket opening/depth disagreement");
        PocketGeometry pocket;pocket.Definition=definition;pocket.Parameters=*p;
        if(!near(definition.Center.Z,0) || !onEdge(definition.Center,pocket.Inward))
            return Fail(Error,"table-v1 pocket center must be on the cloth boundary at z=0");
        pocket.Tangent={-pocket.Inward.Y,pocket.Inward.X,0};
        const double half=p->OpeningWidth/2;
        pocket.MouthStart=definition.Center-pocket.Tangent*half;
        pocket.MouthEnd=definition.Center+pocket.Tangent*half;
        for(int side:{-1,1}) {
            auto& jaw=side<0?pocket.LeftJaw:pocket.RightJaw;
            const Vec3d endpoint=definition.Center+pocket.Tangent*(side*half);
            // Circular lip centered outward from the mouth endpoint, tessellated explicitly.
            const Vec3d center=endpoint-pocket.Inward*p->JawRadius;
            for(std::uint32_t i=0;i<=Settings.JawSubdivisions;++i) {
                const double angle=side*p->JawAngle*static_cast<double>(i)/Settings.JawSubdivisions;
                jaw.push_back(center+RotateZ(pocket.Inward,angle)*p->JawRadius);
                if(i)g.Segments.push_back({TableFeature::PocketJaw,definition.Id,jaw[i-1],jaw[i],{},p->Id,p->ParameterVersion});
            }
        }
        if(p->BottomSlope<=0)return Fail(Error,"finite pocket floor requires positive slope measured from horizontal");
        const double run=p->BottomDepth/std::tan(p->BottomSlope);
        pocket.BottomCenter=definition.Center-pocket.Inward*run+Vec3d{0,0,-p->BottomDepth};
        pocket.Floor={pocket.MouthStart,pocket.MouthEnd,pocket.BottomCenter+pocket.Tangent*half,pocket.BottomCenter-pocket.Tangent*half};
        const Vec3d bottomEnd=pocket.BottomCenter-pocket.Inward*(2*p->JawRadius);
        pocket.Bottom={pocket.BottomCenter-pocket.Tangent*half,pocket.BottomCenter+pocket.Tangent*half,
                      bottomEnd+pocket.Tangent*half,bottomEnd-pocket.Tangent*half};
        g.Pockets.push_back(pocket);
    }
    for(const auto& segment:g.Segments)if(!Finite(segment.Start) || !Finite(segment.End) || !Finite(segment.InwardNormal))
        return Fail(Error,"generated geometry overflow");
    for(const auto& p:g.Pockets) {
        if(!Finite(p.BottomCenter))return Fail(Error,"pocket floor overflow");
        for(const auto& v:p.Floor)if(!Finite(v))return Fail(Error,"pocket floor overflow");
        for(const auto& v:p.Bottom)if(!Finite(v))return Fail(Error,"pocket bottom overflow");
    }
    if(Settings.RequireClosedBoundary && !CloseBoundary(g,Settings.CoordinateTolerance,Error))return false;
    Output=std::move(g);return true;
}
}
