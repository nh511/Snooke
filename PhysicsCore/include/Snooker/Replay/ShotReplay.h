#pragma once
#include <Snooker/Replay/PhysicsShotRecord.h>

namespace Snooker
{
// Recomputes dynamics from recorded parameters/input; stored final/events are
// comparison targets only. Output is unchanged on invalid input or divergence.
bool ReplayShot(const PhysicsShotRecord&,PhysicsShotRecord& Output,std::string* Error=nullptr);
}
