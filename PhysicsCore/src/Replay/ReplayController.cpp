#include <Snooker/Replay/ReplayController.h>
#include <algorithm>

namespace Snooker
{
namespace {bool Fail(std::string* e,const char* m){if(e)*e=m;return false;}}
bool ReplayController::Load(const PhysicsShotRecord& record,std::string* error)
{
    if(!record.IsValid(error))return false;
    if(record.BuildVersion!="standalone-solver-v1")return Fail(error,"unsupported controller solver build");
    if(record.Final.StepIndex-record.Initial.StepIndex>1000000)return Fail(error,"controller step resource limit");
    auto service=CreatePhysicsService(record.Parameters,error);
    if(!service || !service->BeginShot(record.Initial,record.Input,error))return false;
    ReplayController next;next.Record=record;next.Checkpoints.push_back(service->SaveSnapshot());
    PhysicsRunState run=PhysicsRunState::Running;std::size_t cursor=0;
    for(unsigned n=0;n<1000000 && run==PhysicsRunState::Running;++n) {
        const auto before=service->SaveSnapshot();
        if(before.AccumulatorSeconds>=2*record.Parameters.FixedStep)return Fail(error,"controller requires one-step replay debt");
        if(!service->Step(std::max(0.0,record.Parameters.FixedStep-before.AccumulatorSeconds),run,error))return false;
        const auto& events=service->GetEvents();
        for(;cursor<events.size();++cursor)if(cursor>=record.Events.size() || events[cursor].Serialize()!=record.Events[cursor].Serialize())
            return Fail(error,"controller event replay mismatch");
        const auto snapshot=service->SaveSnapshot();
        if(snapshot.StepIndex%120==0 || run!=PhysicsRunState::Running)next.Checkpoints.push_back(snapshot);
        if(snapshot.StepIndex>record.Final.StepIndex)return Fail(error,"controller exceeded recorded final step");
    }
    if(run==PhysicsRunState::Running || cursor!=record.Events.size() || service->SaveSnapshot().Serialize()!=record.Final.Serialize())
        return Fail(error,"controller final replay mismatch");
    if((run==PhysicsRunState::Settled)!=(record.Result=="Settled"))return Fail(error,"controller result mismatch");
    next.Ready=true;next.Current=next.Checkpoints.front();*this=std::move(next);return true;
}
bool ReplayController::SeekStep(StepIndex step,std::string* error)
{
    if(!Ready || step<Record.Initial.StepIndex || step>Record.Final.StepIndex)return Fail(error,"replay step outside recording");
    const auto it=std::upper_bound(Checkpoints.begin(),Checkpoints.end(),step,[](StepIndex value,const auto& p){return value<p.StepIndex;});
    auto snapshot=*(it-1);
    if(snapshot.StepIndex<step) {
        auto service=CreatePhysicsService(Record.Parameters,error);
        if(!service || !service->RestoreSnapshot(snapshot,error))return false;
        PhysicsRunState state;
        while(snapshot.StepIndex<step) {
            if(!service->Step(std::max(0.0,Record.Parameters.FixedStep-snapshot.AccumulatorSeconds),state,error) || state==PhysicsRunState::Faulted)
                return Fail(error,"fault while seeking replay checkpoint");
            snapshot=service->SaveSnapshot();
        }
    }
    Current=std::move(snapshot);return true;
}
bool ReplayController::SeekTime(Seconds time,std::string* error)
{
    if(!Ready || !IsFinite(time) || time<Record.Initial.SimulationTimeSeconds || time>Record.Final.SimulationTimeSeconds)
        return Fail(error,"replay time outside recording");
    const auto raw=std::ceil(time/Record.Parameters.FixedStep-1e-10);
    return SeekStep(std::clamp(static_cast<StepIndex>(raw),Record.Initial.StepIndex,Record.Final.StepIndex),error);
}
bool ReplayController::SeekEvent(std::size_t index,std::string* error)
{
    if(!Ready || index>=Record.Events.size())return Fail(error,"replay event outside recording");
    return SeekStep(Record.Events[index].Step,error);
}
bool ReplayController::SeekFault(std::string* error)
{
    if(!Ready || Record.Fault.Code==PhysicsFaultCode::None)return Fail(error,"record has no physics fault");
    Current=Record.Fault.Snapshot;return true;
}
}
