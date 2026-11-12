#pragma once
#include <Snooker/Data/BallDefinition.h>
namespace Snooker
{
// Cue ball is never a scoring object. Unknown values are rejected, not silently scored.
bool TryBallValue(EBallType Type, int& Value);
}
