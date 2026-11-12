#include <Snooker/Physics/CushionContact.h>
#include <algorithm>

namespace Snooker
{
bool FindCushionContacts(const BallState& Ball,const TableGeometry& Geometry,Meters ContactTolerance,
                         std::vector<CushionContact>& Output,std::string* Error)
{
    const auto fail=[&](const char* message){if(Error)*Error=message;return false;};
    if(!Ball.IsValid(Error))return false;
    if(!Geometry.Table.IsValid() || !Geometry.Cloth.IsValid() || !IsFinite(ContactTolerance) ||
       ContactTolerance<0 || ContactTolerance>=Ball.RadiusMeters || Geometry.Segments.size()>1000000)
        return fail("invalid cushion query bounds, tolerance or resource count");
    std::vector<CushionContact> contacts;
    for(std::size_t i=0;i<Geometry.Segments.size();++i) {
        const auto& s=Geometry.Segments[i];
        if(s.Feature==TableFeature::PocketJaw)continue;
        if(s.Feature!=TableFeature::ClothBoundary && s.Feature!=TableFeature::CushionFace &&
           s.Feature!=TableFeature::CushionTop && s.Feature!=TableFeature::PocketTransition)
            return fail("unknown table contact feature");
        if(!s.Start.IsFinite() || !s.End.IsFinite() || !s.InwardNormal.IsFinite())
            return fail("nonfinite contact contour");
        const auto delta=s.End-s.Start;
        const Scalar length=std::hypot(delta.X,delta.Y,delta.Z);
        if(!IsFinite(length) || length<=0)return fail("invalid finite contact segment");
        const auto tangent=delta*(1/length);
        auto center=Ball.PositionMeters;
        const bool diagnostic=s.Feature==TableFeature::ClothBoundary;
        // Cloth edges describe a 2D playable-area boundary, not sphere support at z=0.
        if(diagnostic)center.Z=s.Start.Z;
        const auto relative=center-s.Start;
        const Scalar projection=relative.Dot(tangent);
        if(!relative.IsFinite() || !IsFinite(projection))return fail("contact projection overflow");
        const Vec3d point=s.Start+tangent*std::clamp(projection,0.0,length);
        const Vec3d displacement=center-point;
        const Scalar distance=std::hypot(displacement.X,displacement.Y,displacement.Z);
        if(!IsFinite(distance) || !point.IsFinite())return fail("contact distance overflow");
        if(distance>Ball.RadiusMeters+ContactTolerance)continue;
        if(s.Feature==TableFeature::CushionFace && Ball.PositionMeters.Z>s.Start.Z+ContactTolerance)continue;
        if(s.Feature==TableFeature::CushionTop && Ball.PositionMeters.Z<=s.Start.Z+ContactTolerance)continue;
        if(!diagnostic) {
            if(!s.OwnerId || s.ProfileId.empty() || s.ProfileVersion.empty() ||
               !NearlyEqual(s.InwardNormal.Length(),1.0))return fail("invalid material contact metadata/normal");
            if(displacement.Dot(s.InwardNormal)<-ContactTolerance)continue;
        }
        Vec3d normal=distance>0?displacement*(1/distance):s.InwardNormal;
        if(!normal.IsFinite() || !NearlyEqual(normal.Length(),1.0))return fail("invalid contact normal");
        contacts.push_back({s.Feature,s.OwnerId,i,point,normal,distance-Ball.RadiusMeters,
                            s.ProfileId,s.ProfileVersion,diagnostic});
    }
    Output.swap(contacts);return true;
}
}
