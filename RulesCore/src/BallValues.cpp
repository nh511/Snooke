#include <Snooker/Rules/BallValues.h>
namespace Snooker
{
bool TryBallValue(EBallType Type, int& Value)
{
    switch (Type) {
    case EBallType::Red: Value=1; return true;
    case EBallType::Yellow: Value=2; return true;
    case EBallType::Green: Value=3; return true;
    case EBallType::Brown: Value=4; return true;
    case EBallType::Blue: Value=5; return true;
    case EBallType::Pink: Value=6; return true;
    case EBallType::Black: Value=7; return true;
    default: return false;
    }
}
}
