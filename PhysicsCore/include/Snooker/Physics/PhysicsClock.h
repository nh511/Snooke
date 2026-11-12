#pragma once
#include <Snooker/Math/SnookerUnits.h>
#include <functional>
#include <limits>

namespace Snooker
{
struct ClockState
{
    Seconds FixedStep=1.0/240.0;
    Seconds Accumulator=0;
    std::uint64_t CompletedSteps=0;
};
class PhysicsClock
{
public:
    const ClockState& State() const { return Current; }
    Seconds SimulationTime() const { return Current.CompletedSteps*Current.FixedStep; }
    bool Restore(const ClockState& State)
    {
        if (!IsFinite(State.FixedStep) || State.FixedStep<1e-6 || State.FixedStep>1 ||
            !IsFinite(State.Accumulator) || State.Accumulator<0 || State.Accumulator>3600 ||
            !IsFinite(State.CompletedSteps*State.FixedStep)) return false;
        Current=State; return true;
    }
    // Budget bounds work, not time: unprocessed debt remains for Advance(0,...).
    // Callback must not throw or reenter this clock; engine rendering never writes solver time.
    bool Advance(Seconds FrameTime,const std::function<void(Seconds)>& Step,std::uint32_t Budget=1024)
    {
        if (!IsFinite(FrameTime) || FrameTime<0 || !Step || Budget==0 ||
            FrameTime>3600-Current.Accumulator ||
            Current.CompletedSteps>std::numeric_limits<std::uint64_t>::max()-Budget) return false;
        Current.Accumulator+=FrameTime;
        const Scalar Tolerance=Current.FixedStep*1e-10;
        for (std::uint32_t i=0;i<Budget && Current.Accumulator+Tolerance>=Current.FixedStep;++i) {
            Step(Current.FixedStep);
            Current.Accumulator-=Current.FixedStep;
            if (Current.Accumulator<0) Current.Accumulator=0;
            ++Current.CompletedSteps;
        }
        return true;
    }
private:
    ClockState Current;
};
}
