#include <Snooker/Rules/ShotPhysicsResult.h>
#include <map>
#include <set>
#include <algorithm>

namespace Snooker
{
namespace
{
bool Fail(std::string* e,const char* m){if(e)*e=m;return false;}
bool Extract(const ShotPhysicsResult& r,BallId& first,std::vector<BallId>& pots,std::string* error)
{
    first=0;pots.clear();std::set<BallId> known,seen;unsigned cueCount=0;
    if(!r.Initial.IsValid(error) || !r.Final.IsValid(error) || r.Definitions.size()!=r.Initial.Balls.size() ||
       r.Initial.Balls.size()!=r.Final.Balls.size() || r.Final.Phase!=ESnapshotPhase::AfterShot)
        return Fail(error,"rules require a complete settled shot and matching definitions");
    for(const auto& d:r.Definitions) {
        if(!d.IsValid(error) || !known.insert(d.Id).second)return Fail(error,"invalid/duplicate rules ball definition");
        if(d.Type==EBallType::Cue){++cueCount;if(d.Id!=r.CueBall)return Fail(error,"rules cue identity mismatch");}
    }
    if(cueCount!=1)return Fail(error,"rules need exactly one explicit cue ball");
    for(const auto* snapshot:{&r.Initial,&r.Final})for(std::size_t i=0;i<snapshot->Balls.size();++i) {
        const auto& b=snapshot->Balls[i];
        auto it=std::find_if(r.Definitions.begin(),r.Definitions.end(),[&](const auto& d){return d.Id==b.Id;});
        if(it==r.Definitions.end() || b.RadiusMeters!=it->RadiusMeters || b.MassKg!=it->MassKg || b.MaterialProfileId!=it->MaterialProfileId)
            return Fail(error,"rules snapshot/definition mismatch");
        const auto p=snapshot->PocketStates.empty()?PocketStateData{}:snapshot->PocketStates[i];
        if(snapshot==&r.Final && p.State!=PocketState::Potted && (!b.Sleeping || p.State==PocketState::InPocket))
            return Fail(error,"rules cannot consume moving final balls");
    }
    if(r.Initial.PhysicsParameterVersion!=r.Final.PhysicsParameterVersion || r.Initial.FixedStepSeconds!=r.Final.FixedStepSeconds ||
       r.Initial.StepIndex>r.Final.StepIndex)return Fail(error,"rules snapshot clock/version mismatch");
    auto cursor=r.Initial.EventCount;auto time=r.Initial.SimulationTimeSeconds;auto step=r.Initial.StepIndex;
    for(const auto& e:r.Events) {
        if(unsigned(e.Kind)>unsigned(PhysicsEventKind::ShotSettled) || cursor==std::numeric_limits<std::uint64_t>::max() ||
           e.Sequence!=cursor++ || !IsFinite(e.Time) || e.Time<time || e.Time>r.Final.SimulationTimeSeconds+1e-12 ||
           !e.Point.IsFinite() || !e.Normal.IsFinite() || !IsFinite(e.Impulse) || e.Impulse<0 ||
           e.Step<step || e.Step>r.Final.StepIndex || (e.Ball && !known.count(e.Ball)))return Fail(error,"invalid rules event ordering/reference");
        if(e.Kind==PhysicsEventKind::BallBall) {
            if(!known.count(e.Other) || e.Other==e.Ball)return Fail(error,"invalid rules contact IDs");
            if(!first && (e.Ball==r.CueBall || e.Other==r.CueBall))first=e.Ball==r.CueBall?e.Other:e.Ball;
        }
        if(e.Kind==PhysicsEventKind::Potted) {
            if(!known.count(e.Ball) || !seen.insert(e.Ball).second)return Fail(error,"duplicate/unknown pot event");
            const auto before=std::find_if(r.Initial.Balls.begin(),r.Initial.Balls.end(),[&](const auto& b){return b.Id==e.Ball;});
            const auto after=std::find_if(r.Final.Balls.begin(),r.Final.Balls.end(),[&](const auto& b){return b.Id==e.Ball;});
            if(before==r.Initial.Balls.end() || after==r.Final.Balls.end())return Fail(error,"pot event ball absent from snapshots");
            const auto a=static_cast<std::size_t>(before-r.Initial.Balls.begin()),b=static_cast<std::size_t>(after-r.Final.Balls.begin());
            if((!r.Initial.PocketStates.empty() && r.Initial.PocketStates[a].State==PocketState::Potted) ||
               r.Final.PocketStates.empty() || r.Final.PocketStates[b].State!=PocketState::Potted)
                return Fail(error,"pot event does not match state transition");
            pots.push_back(e.Ball);
        }
        time=e.Time;step=e.Step;
    }
    if(cursor!=r.Final.EventCount || r.Events.empty() || r.Events.front().Kind!=PhysicsEventKind::CueStrike ||
       r.Events.front().Ball!=r.CueBall || r.Events.back().Kind!=PhysicsEventKind::ShotSettled)
        return Fail(error,"rules event sequence is incomplete");
    for(std::size_t i=0;i<r.Final.Balls.size();++i) {
        const auto& b=r.Final.Balls[i];const auto before=std::find_if(r.Initial.Balls.begin(),r.Initial.Balls.end(),[&](const auto& x){return x.Id==b.Id;});
        if(before==r.Initial.Balls.end())return Fail(error,"changed rules ball set");
        const auto j=static_cast<std::size_t>(before-r.Initial.Balls.begin());
        const bool was=!r.Initial.PocketStates.empty() && r.Initial.PocketStates[j].State==PocketState::Potted;
        const bool now=!r.Final.PocketStates.empty() && r.Final.PocketStates[i].State==PocketState::Potted;
        if(was && !now)return Fail(error,"potted ball silently respotted during shot");
        if(now && !was && !seen.count(b.Id))return Fail(error,"missing pot event");
    }
    return true;
}
}
bool ShotPhysicsResult::IsValid(std::string* error) const
{
    BallId first=0;std::vector<BallId> pots;
    if(!Extract(*this,first,pots,error))return false;
    const bool cue=std::find(pots.begin(),pots.end(),CueBall)!=pots.end();
    if(first!=FirstContact || pots!=Potted || cue!=CueBallPotted)return Fail(error,"rules cached result differs from events");
    return true;
}
bool BuildShotRuleInput(const PhysicsShotRecord& record,const std::vector<BallDefinition>& definitions,ShotPhysicsResult& output,std::string* error)
{
    if(!record.IsValid(error))return false;
    if(record.Result!="Settled")return Fail(error,"faulted simulation cannot be scored");
    ShotPhysicsResult r;r.CueBall=record.Input.CueBallId;r.Definitions=definitions;r.Events=record.Events;r.Initial=record.Initial;r.Final=record.Final;
    if(!Extract(r,r.FirstContact,r.Potted,error))return false;
    r.CueBallPotted=std::find(r.Potted.begin(),r.Potted.end(),r.CueBall)!=r.Potted.end();
    if(!r.IsValid(error))return false;output=std::move(r);return true;
}
}
