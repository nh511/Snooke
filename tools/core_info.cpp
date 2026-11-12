#include <Snooker/Math/CoreTypes.h>
#include <Snooker/Rules/BallValues.h>
#include <iostream>
int main()
{
    int Value=0;
    if (!Snooker::TryBallValue(Snooker::EBallType::Black,Value) || Value!=7) return 1;
    std::cout << "PhysicsCore/RulesCore C++17; SI m kg s rad; black value=" << Value << '\n';
}
