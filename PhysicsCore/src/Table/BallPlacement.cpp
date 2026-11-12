#include <Snooker/Table/BallPlacement.h>
#include <algorithm>
#include <iomanip>
#include <locale>
#include <sstream>
#include <utility>

namespace Snooker
{
namespace
{
bool Fail(std::string* Error,const char* Message) {if(Error)*Error=Message;return false;}
}
bool OpeningLayout::IsValid(std::string* Error) const
{
    const double length=std::hypot(RackDirection.X,RackDirection.Y);
    if(ParameterVersion.empty() || ParameterVersion.find_first_of(" \t\r\n")!=std::string::npos ||
       !CuePosition.IsFinite() || !RedApex.IsFinite() || !RackDirection.IsFinite() || !IsFinite(length) || length<=1e-9 ||
       !IsFinite(RedClearance) || RedClearance<0)return Fail(Error,"invalid opening layout");
    return true;
}
std::string OpeningLayout::Serialize() const
{
    std::ostringstream s;s.imbue(std::locale::classic());s<<std::setprecision(std::numeric_limits<double>::max_digits10)
        <<"snooker-opening-v1 "<<ParameterVersion<<' '<<CuePosition.X<<' '<<CuePosition.Y<<' '<<CuePosition.Z<<' '
        <<RedApex.X<<' '<<RedApex.Y<<' '<<RedApex.Z<<' '<<RackDirection.X<<' '<<RackDirection.Y<<' '<<RedClearance;
    return s.str();
}
bool OpeningLayout::Deserialize(const std::string& Text,OpeningLayout& Output,std::string* Error)
{
    std::istringstream s(Text);s.imbue(std::locale::classic());OpeningLayout layout;std::string magic;
    if(!(s>>magic>>layout.ParameterVersion>>layout.CuePosition.X>>layout.CuePosition.Y>>layout.CuePosition.Z
        >>layout.RedApex.X>>layout.RedApex.Y>>layout.RedApex.Z>>layout.RackDirection.X>>layout.RackDirection.Y>>layout.RedClearance) ||
        magic!="snooker-opening-v1")return Fail(Error,"invalid opening layout fields/version");
    s>>std::ws;if(!s.eof())return Fail(Error,"unexpected layout tail");
    if(!layout.IsValid(Error))return false;
    Output=std::move(layout);return true;
}
bool GenerateCustomPlacement(const TableDefinition& Table,const std::vector<BallDefinition>& Input,
    std::vector<BallDefinition>& Output,std::string* Error)
{
    if(!Table.IsValid(Error))return false;
    if(Input.size()>4096)return Fail(Error,"placement resource limit exceeded");
    auto balls=Input;
    std::sort(balls.begin(),balls.end(),[](const auto& a,const auto& b){return a.Id<b.Id;});
    for(std::size_t i=0;i<balls.size();++i) {
        const auto& b=balls[i];const auto& p=b.InitialPositionMeters;const auto r=b.RadiusMeters;
        if(!b.IsValid(Error))return false;
        if(!NearlyEqual(p.Z,r,1e-10) || p.X-r<Table.Cloth.Min.X || p.X+r>Table.Cloth.Max.X ||
           p.Y-r<Table.Cloth.Min.Y || p.Y+r>Table.Cloth.Max.Y)return Fail(Error,"ball placement outside supported cloth surface");
        for(std::size_t j=0;j<i;++j) {
            if(b.Id==balls[j].Id)return Fail(Error,"duplicate placement id");
            const auto d=p-balls[j].InitialPositionMeters;const auto separation=std::hypot(d.X,d.Y,d.Z);
            if(!IsFinite(separation) || separation+1e-10<r+balls[j].RadiusMeters)return Fail(Error,"overlapping placement");
        }
    }
    Output.swap(balls);return true;
}
bool GenerateTrainingPlacement(const TableDefinition& Table,std::vector<BallDefinition>& Output,std::string* Error)
{
    return GenerateCustomPlacement(Table,Table.InitialBalls,Output,Error);
}
bool GenerateOpeningPlacement(const TableDefinition& Table,const BallDefinition& Prototype,const OpeningLayout& Layout,
    std::vector<BallDefinition>& Output,std::string* Error)
{
    if(!Table.IsValid(Error) || !Prototype.IsValid(Error) || !Layout.IsValid(Error))return false;
    std::vector<BallDefinition> balls;
    auto cue=Prototype;cue.Id=1;cue.Type=EBallType::Cue;cue.InitialPositionMeters=Layout.CuePosition;balls.push_back(cue);
    BallId id=2;
    for(auto type:{EBallType::Yellow,EBallType::Green,EBallType::Brown,EBallType::Blue,EBallType::Pink,EBallType::Black}) {
        const BallSpot* spot=nullptr;
        for(const auto& candidate:Table.Spots)if(candidate.Type==type)spot=&candidate;
        if(!spot)return Fail(Error,"opening requires all six explicit color spots");
        auto color=Prototype;color.Id=id++;color.Type=type;color.InitialPositionMeters=spot->Position;balls.push_back(color);
    }
    const double length=std::hypot(Layout.RackDirection.X,Layout.RackDirection.Y);
    const Vec3d direction{Layout.RackDirection.X/length,Layout.RackDirection.Y/length,0}, lateral{-direction.Y,direction.X,0};
    const double spacing=2*Prototype.RadiusMeters+Layout.RedClearance;
    if(!IsFinite(spacing))return Fail(Error,"rack spacing overflow");
    for(int row=0;row<5;++row)for(int column=0;column<=row;++column) {
        auto red=Prototype;red.Id=id++;red.Type=EBallType::Red;
        red.InitialPositionMeters=Layout.RedApex+direction*(row*spacing*std::sqrt(3.0)/2)+lateral*((column-row*0.5)*spacing);
        balls.push_back(red);
    }
    return GenerateCustomPlacement(Table,balls,Output,Error);
}
bool ResetColorPlacement(const TableDefinition& Table,const BallDefinition& Color,
    const std::vector<BallDefinition>& Occupied,BallDefinition& Output,std::string* Error)
{
    if(!Table.IsValid(Error) || !Color.IsValid(Error))return false;
    if(Color.Type==EBallType::Cue || Color.Type==EBallType::Red)return Fail(Error,"only colors have reset spots");
    const BallSpot* spot=nullptr;
    for(const auto& candidate:Table.Spots)if(candidate.Type==Color.Type)spot=&candidate;
    if(!spot)return Fail(Error,"missing reset spot");
    auto reset=Color;reset.InitialPositionMeters=spot->Position;
    auto combined=Occupied;combined.push_back(reset);std::vector<BallDefinition> validated;
    if(!GenerateCustomPlacement(Table,combined,validated,Error))return false;
    Output=reset;return true;
}
}
