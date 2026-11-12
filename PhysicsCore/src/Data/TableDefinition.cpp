#include <Snooker/Data/TableDefinition.h>
#include <Snooker/Serialization/Text.h>
#include <sstream>
#include <iomanip>
#include <locale>
#include <set>

namespace Snooker
{
namespace
{
bool Fail(std::string* Error,const char* Message) { if(Error) *Error=Message; return false; }
bool Token(const std::string& s) { return !s.empty() && s.find_first_of(" \t\r\n")==std::string::npos; }
bool Tag(std::istream& s,const char* Expected) { std::string t; return bool(s>>t) && t==Expected; }
bool Read(std::istream& s,Vec3d& v) { return bool(s>>v.X>>v.Y>>v.Z); }
bool Read(std::istream& s,TableBounds& b) { return bool(s>>b.Min.X>>b.Min.Y>>b.Max.X>>b.Max.Y); }
bool Count(std::istream& s,const char* name,std::size_t& n) { return Tag(s,name) && ReadUnsigned(s,n) && n<=4096; }
void Write(std::ostream& s,const Vec3d& v) { s<<v.X<<' '<<v.Y<<' '<<v.Z; }
void Write(std::ostream& s,const TableBounds& b) { s<<b.Min.X<<' '<<b.Min.Y<<' '<<b.Max.X<<' '<<b.Max.Y; }
}
bool TableBounds::IsValid() const { return Min.IsFinite() && Max.IsFinite() && Min.X<Max.X && Min.Y<Max.Y; }
bool TableBounds::Contains(const Vec2d& p) const
{
    return p.IsFinite() && p.X>=Min.X && p.X<=Max.X && p.Y>=Min.Y && p.Y<=Max.Y;
}
bool TableDefinition::IsValid(std::string* Error) const
{
    if(!Token(ParameterVersion) || !Table.IsValid() || !Cloth.IsValid() || !Table.Contains(Cloth.Min) || !Table.Contains(Cloth.Max))
        return Fail(Error,"invalid table version or bounds");
    if(Cushions.size()>4096 || Pockets.size()>4096 || Spots.size()>4096 || InitialBalls.size()>4096)
        return Fail(Error,"table record limit exceeded");
    std::set<std::uint32_t> ids;
    for(const auto& c:Cushions) {
        if(!c.Id || !ids.insert(c.Id).second || !c.Start.IsFinite() || !c.End.IsFinite() ||
           !(c.End-c.Start).IsFinite() || (c.End-c.Start).Length()<=1e-12 || !Token(c.ProfileId))
            return Fail(Error,"invalid cushion record");
    }
    ids.clear();
    for(const auto& p:Pockets) {
        if(!p.Id || !ids.insert(p.Id).second || !p.Center.IsFinite() || !IsFinite(p.Opening) ||
           !IsFinite(p.Depth) || p.Opening<=0 || p.Depth<=0 || !Token(p.ProfileId))
            return Fail(Error,"invalid pocket record");
    }
    std::set<EBallType> types;
    for(const auto& spot:Spots) {
        if(std::string(BallTypeToString(spot.Type))=="Unknown" || !types.insert(spot.Type).second ||
           !spot.Position.IsFinite() || !Cloth.Contains({spot.Position.X,spot.Position.Y}))
            return Fail(Error,"invalid or duplicate ball spot");
    }
    ids.clear();
    for(const auto& ball:InitialBalls) {
        if(!ball.IsValid(Error)) return false;
        if(!ids.insert(ball.Id).second || !Cloth.Contains({ball.InitialPositionMeters.X,ball.InitialPositionMeters.Y}))
            return Fail(Error,"invalid initial placement");
    }
    return true;
}
std::string TableDefinition::Serialize() const
{
    std::ostringstream s; s.imbue(std::locale::classic()); s<<std::setprecision(std::numeric_limits<double>::max_digits10);
    s<<"snooker-table-v1 "<<ParameterVersion<<"\ntable "; Write(s,Table);
    s<<"\ncloth "; Write(s,Cloth);
    s<<"\ncushions "<<Cushions.size();
    for(const auto& c:Cushions) { s<<"\ncushion "<<c.Id<<' '; Write(s,c.Start); s<<' '; Write(s,c.End); s<<' '<<c.ProfileId; }
    s<<"\npockets "<<Pockets.size();
    for(const auto& p:Pockets) { s<<"\npocket "<<p.Id<<' '; Write(s,p.Center); s<<' '<<p.Opening<<' '<<p.Depth<<' '<<p.ProfileId; }
    s<<"\nspots "<<Spots.size();
    for(const auto& spot:Spots) { s<<"\nspot "<<BallTypeToString(spot.Type)<<' '; Write(s,spot.Position); }
    s<<"\nplacements "<<InitialBalls.size();
    for(const auto& ball:InitialBalls) s<<"\nball "<<ball.Serialize();
    s<<"\nend\n";
    return s.str();
}
bool TableDefinition::Deserialize(const std::string& Text,TableDefinition& Output,std::string* Error)
{
    std::istringstream s(Text); s.imbue(std::locale::classic());
    TableDefinition t; std::size_t count=0;
    if(!Tag(s,"snooker-table-v1") || !(s>>t.ParameterVersion) || !Tag(s,"table") || !Read(s,t.Table) ||
       !Tag(s,"cloth") || !Read(s,t.Cloth) || !Count(s,"cushions",count)) return Fail(Error,"invalid table header");
    for(std::size_t i=0;i<count;++i) {
        CushionDefinition c;
        if(!Tag(s,"cushion") || !ReadUnsigned(s,c.Id) || !Read(s,c.Start) || !Read(s,c.End) || !(s>>c.ProfileId))
            return Fail(Error,"invalid cushion fields");
        t.Cushions.push_back(c);
    }
    if(!Count(s,"pockets",count)) return Fail(Error,"invalid pockets count");
    for(std::size_t i=0;i<count;++i) {
        PocketDefinition p;
        if(!Tag(s,"pocket") || !ReadUnsigned(s,p.Id) || !Read(s,p.Center) || !(s>>p.Opening>>p.Depth>>p.ProfileId))
            return Fail(Error,"invalid pocket fields");
        t.Pockets.push_back(p);
    }
    if(!Count(s,"spots",count)) return Fail(Error,"invalid spots count");
    for(std::size_t i=0;i<count;++i) {
        BallSpot spot; std::string type;
        if(!Tag(s,"spot") || !(s>>type) || !TryParseBallType(type,spot.Type) || !Read(s,spot.Position))
            return Fail(Error,"invalid spot fields");
        t.Spots.push_back(spot);
    }
    if(!Count(s,"placements",count)) return Fail(Error,"invalid placements count");
    for(std::size_t i=0;i<count;++i) {
        std::string line; BallDefinition ball;
        if(!Tag(s,"ball") || !std::getline(s,line)) return Fail(Error,"missing initial ball");
        if(!BallDefinition::Deserialize(line,ball,Error)) return false;
        t.InitialBalls.push_back(ball);
    }
    if(!Tag(s,"end")) return Fail(Error,"missing table terminator");
    s>>std::ws;
    if(!s.eof()) return Fail(Error,"unexpected data after table");
    if(!t.IsValid(Error)) return false;
    Output=std::move(t); return true;
}
}
