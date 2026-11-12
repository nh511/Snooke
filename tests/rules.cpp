#include <Snooker/Rules/BallValues.h>
#include <iostream>
int main()
{
    using namespace Snooker;
    int Value=99;
    for (int i=1;i<=7;++i)
        if (!TryBallValue(static_cast<EBallType>(i),Value) || Value!=i) return 1;
    if (TryBallValue(EBallType::Cue,Value) || Value!=7) return 1;
    if (TryBallValue(static_cast<EBallType>(255),Value) || Value!=7) return 1;
    std::cout << "PASS rules score lookup and invalid ball rejection\n";
}
