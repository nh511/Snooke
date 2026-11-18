#pragma once
#include <Snooker/Replay/PhysicsShotRecord.h>

namespace Snooker
{
class ReplayController
{
public:
    // Validates by recomputing the shot; checkpoints are solver output only.
    bool Load(const PhysicsShotRecord&,std::string* Error=nullptr);
    bool SeekStep(StepIndex,std::string* Error=nullptr);
    // Fractional times select the first completed fixed step at/after Time.
    bool SeekTime(Seconds,std::string* Error=nullptr);
    bool SeekEvent(std::size_t Index,std::string* Error=nullptr);
    bool SeekFault(std::string* Error=nullptr);
    const PhysicsSnapshot& Snapshot() const{return Current;}
    const std::vector<PhysicsEvent>& Events() const{return Record.Events;}
    const PhysicsFault& Fault() const{return Record.Fault;}
    std::size_t CheckpointCount() const{return Checkpoints.size();}
private:
    PhysicsShotRecord Record;
    std::vector<PhysicsSnapshot> Checkpoints;
    PhysicsSnapshot Current;
    bool Ready=false;
};
}
