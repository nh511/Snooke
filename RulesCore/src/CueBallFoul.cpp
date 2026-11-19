#include <Snooker/Rules/CueBallFoul.h>
#include <algorithm>

namespace Snooker
{
namespace {bool Fail(std::string* e,const char* m){if(e)*e=m;return false;}}
bool CueBallPlacementRules::IsValid(std::string* Error) const
{
    if(!IsFinite(DLineX)||!IsFinite(DCenterY)||!IsFinite(DRadius)||!IsFinite(Tolerance)||DRadius<=0||Tolerance<0)
        return Fail(Error,"invalid cue-ball placement rules");
    return true;
}
bool CueBallFoulResult::IsValid(std::string* Error) const
{
    if(CueBall==0 || (!Foul && (CueBallInHand || SuppressObjectScoring || !Reason.empty())) ||
       (Foul && (!CueBallInHand || !SuppressObjectScoring || Reason.empty())))
        return Fail(Error,"invalid cue-ball foul result");
    for(std::size_t i=0;i<ObjectBallsPotted.size();++i) {
        if(ObjectBallsPotted[i]==0 || ObjectBallsPotted[i]==CueBall ||
           std::find(ObjectBallsPotted.begin(),ObjectBallsPotted.begin()+i,ObjectBallsPotted[i])!=ObjectBallsPotted.begin()+i)
            return Fail(Error,"invalid object ball foul list");
    }
    return true;
}
bool EvaluateCueBallPotted(const ShotPhysicsResult& Input,CueBallFoulResult& Output,std::string* Error)
{
    if(!Input.IsValid(Error))return false;
    CueBallFoulResult result;result.CueBall=Input.CueBall;
    for(const auto id:Input.Potted)if(id!=Input.CueBall)result.ObjectBallsPotted.push_back(id);
    if(Input.CueBallPotted) {
        result.Foul=true;result.CueBallInHand=true;result.SuppressObjectScoring=true;
        result.Reason="cue ball potted";
    }
    if(!result.IsValid(Error))return false;Output=std::move(result);return true;
}
bool PlaceCueBallInD(const TableDefinition& Table,const BallDefinition& Cue,const Vec3d& Position,
                     const std::vector<BallDefinition>& Occupied,const CueBallPlacementRules& Rules,
                     BallDefinition& Output,std::string* Error)
{
    if(!Table.IsValid(Error)||!Cue.IsValid(Error)||!Rules.IsValid(Error))return false;
    if(Cue.Type!=EBallType::Cue)return Fail(Error,"placement definition is not cue ball");
    if(!Position.IsFinite() || !NearlyEqual(Position.Z,Cue.RadiusMeters,Rules.Tolerance) ||
       Position.X>Rules.DLineX+Rules.Tolerance ||
       std::hypot(Position.X-Rules.DLineX,Position.Y-Rules.DCenterY)>Rules.DRadius+Rules.Tolerance ||
       Position.X-Cue.RadiusMeters<Table.Cloth.Min.X || Position.X+Cue.RadiusMeters>Table.Cloth.Max.X ||
       Position.Y-Cue.RadiusMeters<Table.Cloth.Min.Y || Position.Y+Cue.RadiusMeters>Table.Cloth.Max.Y)
        return Fail(Error,"cue-ball position is outside the D or cloth");
    for(const auto& other:Occupied) {
        if(!other.IsValid(Error)||other.Id==Cue.Id)return Fail(Error,"invalid or duplicate occupied ball");
        if((Position-other.InitialPositionMeters).Length()+Rules.Tolerance<Cue.RadiusMeters+other.RadiusMeters)
            return Fail(Error,"cue-ball placement overlaps occupied ball");
    }
    auto result=Cue;result.InitialPositionMeters=Position;Output=std::move(result);return true;
}
}
