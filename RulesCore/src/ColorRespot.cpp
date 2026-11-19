#include <Snooker/Rules/ColorRespot.h>
#include <Snooker/Rules/BallValues.h>
#include <algorithm>

namespace Snooker
{
namespace {bool Fail(std::string* e,const char* m){if(e)*e=m;return false;}}
bool ColorRespotResult::IsValid(std::string* Error) const
{
    if(Status==RespotStatus::NoSpotAvailable)return true;
    if(!Ball.IsValid(Error)||Ball.Type==EBallType::Cue||Ball.Type==EBallType::Red||SpotType==EBallType::Cue||SpotType==EBallType::Red)
        return Fail(Error,"invalid color respot result");
    if((Status==RespotStatus::OwnSpot)!=(SpotType==Ball.Type))return Fail(Error,"respot status/spot mismatch");
    return true;
}
bool FindColorRespot(const TableDefinition& table,const BallDefinition& color,const std::vector<BallDefinition>& occupied,ColorRespotResult& Output,std::string* Error)
{
    if(!table.IsValid(Error)||!color.IsValid(Error)||color.Type==EBallType::Cue||color.Type==EBallType::Red)return Fail(Error,"invalid color respot input");
    for(const auto& b:occupied)if(!b.IsValid(Error)||b.Id==color.Id)return Fail(Error,"invalid/duplicate occupied respot ball");
    std::vector<EBallType> order{color.Type,EBallType::Black,EBallType::Pink,EBallType::Blue,EBallType::Brown,EBallType::Green,EBallType::Yellow};
    order.erase(std::unique(order.begin(),order.end()),order.end());
    for(const auto type:order) {
        const auto spot=std::find_if(table.Spots.begin(),table.Spots.end(),[&](const auto& s){return s.Type==type;});if(spot==table.Spots.end())continue;
        auto candidate=color;candidate.InitialPositionMeters=spot->Position;bool clear=true;
        for(const auto& b:occupied)if((candidate.InitialPositionMeters-b.InitialPositionMeters).Length()+1e-10<candidate.RadiusMeters+b.RadiusMeters){clear=false;break;}
        if(clear){ColorRespotResult result;result.Ball=candidate;result.SpotType=type;result.Status=type==color.Type?RespotStatus::OwnSpot:RespotStatus::AlternateSpot;if(!result.IsValid(Error))return false;Output=result;return true;}
    }
    ColorRespotResult result;result.Status=RespotStatus::NoSpotAvailable;result.Ball=color;result.SpotType=color.Type;Output=result;return true;
}
bool FindColorRespots(const TableDefinition& table,const std::vector<BallDefinition>& colors,const std::vector<BallDefinition>& occupied,std::vector<ColorRespotResult>& Output,std::string* Error)
{
    auto sorted=colors;std::sort(sorted.begin(),sorted.end(),[](const auto& a,const auto& b){int va=0,vb=0;TryBallValue(a.Type,va);TryBallValue(b.Type,vb);return va>vb||(va==vb&&a.Id<b.Id);});
    auto used=occupied;std::vector<ColorRespotResult> results;
    for(const auto& color:sorted){ColorRespotResult result;if(!FindColorRespot(table,color,used,result,Error))return false;results.push_back(result);if(result.Status!=RespotStatus::NoSpotAvailable)used.push_back(result.Ball);}
    Output=std::move(results);return true;
}
}
