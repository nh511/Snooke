#include <Snooker/Physics/BallState.h>
#include <iostream>

#define CHECK(x) do { if (!(x)) { std::cerr << "FAIL state line " << __LINE__ << '\n'; return 1; } } while(false)
int main()
{
    using namespace Snooker;
    BallState state; state.Id=1; state.PositionMeters={1,2,0.02625};
    state.Sleeping=false; state.LinearVelocityMetersPerSecond={0.5,-0.25,0};
    state.AngularVelocityRadiansPerSecond={2,-3,4};
    state.Contact={EContactKind::Ball,2,{1,0,0}};
    CHECK(state.IsValid());
    BallState copy=state; CHECK(copy==state);
    copy.PositionMeters.X+=1; CHECK(!(copy==state)); CHECK(NearlyEqual(state.PositionMeters.X,1));
    copy=state; copy.Sleeping=true; CHECK(!(copy==state));
    copy=state; copy.MaterialProfileId="other-v1"; CHECK(!(copy==state));
    BallState restored;
    CHECK(BallState::Deserialize(state.Serialize(),restored)); CHECK(state==restored);
    state.Contact.OtherBallId=state.Id; CHECK(!state.IsValid());
    state.Contact.OtherBallId=0; CHECK(!state.IsValid());
    state.Contact={EContactKind::Cloth,0,TableNormal}; CHECK(state.IsValid());
    CHECK(BallState::Deserialize(state.Serialize(),restored)); CHECK(state==restored);
    std::cout << "PASS BallState copy independence, equality, contact validation and round trip\n";
}
