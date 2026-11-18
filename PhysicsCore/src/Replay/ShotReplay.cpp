#include <Snooker/Replay/ShotReplay.h>

namespace Snooker
{
bool ReplayShot(const PhysicsShotRecord& record,PhysicsShotRecord& output,std::string* error)
{
    auto fail=[&](const std::string& message){if(error)*error=message;return false;};
    if(!record.IsValid(error))return false;
    if(record.BuildVersion!="standalone-solver-v1")return fail("unsupported replay solver build");
    auto service=CreatePhysicsService(record.Parameters,error);
    if(!service || !service->BeginShot(record.Initial,record.Input,error))return false;
    PhysicsRunState run=PhysicsRunState::Running;std::size_t checked=0;
    for(unsigned n=0;n<1000000 && run==PhysicsRunState::Running;++n) {
        const auto before=service->SaveSnapshot();
        if(!service->Step(std::max(0.0,record.Parameters.FixedStep-before.AccumulatorSeconds),run,error))return false;
        const auto& events=service->GetEvents();
        for(;checked<events.size();++checked) {
            if(checked>=record.Events.size())return fail("extra event at sequence "+std::to_string(events[checked].Sequence));
            if(events[checked].Serialize()!=record.Events[checked].Serialize())
                return fail("event mismatch at step "+std::to_string(events[checked].Step)+" sequence "+std::to_string(events[checked].Sequence));
        }
        if(service->SaveSnapshot().StepIndex>record.Final.StepIndex && run!=PhysicsRunState::Faulted)
            return fail("replay exceeded recorded final step");
    }
    if(run==PhysicsRunState::Running)return fail("replay step budget exhausted");
    if(checked!=record.Events.size())return fail("missing recorded events");
    if(service->SaveSnapshot().Serialize()!=record.Final.Serialize())return fail("final snapshot mismatch");
    const auto result=run==PhysicsRunState::Settled?std::string("Settled"):
        std::string("fault:")+PhysicsFaultCodeName(service->GetFault().Code);
    if(result!=record.Result)return fail("replay result mismatch");
    if(run==PhysicsRunState::Faulted && service->GetFault().Code!=record.Fault.Code)return fail("replay fault mismatch");
    auto candidate=record;candidate.Final=service->SaveSnapshot();candidate.Events=service->GetEvents();
    if(run==PhysicsRunState::Faulted)candidate.Fault=service->GetFault();
    output=std::move(candidate);return true;
}
}
