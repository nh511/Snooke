#include <Snooker/Data/BallDefinition.h>
#include <Snooker/Physics/PhysicsSnapshot.h>
#include <Snooker/Shot/ShotInput.h>
#include <iostream>
#include <locale>
#include <sstream>

#define CHECK(x) do { if (!(x)) { std::cerr << "FAIL parser line " << __LINE__ << ": " << #x << '\n'; return 1; } } while(false)
struct Comma : std::numpunct<char> { char do_decimal_point() const override { return ','; } };
template<class T> bool Reject(const std::string& text, const T& original)
{
    T output=original;
    std::string error;
    return !T::Deserialize(text,output,&error) && !error.empty() && output.Serialize()==original.Serialize();
}
template<class T> bool Truncations(const T& value)
{
    std::istringstream tokens(value.Serialize());
    std::string word, prefix;
    while(tokens >> word) {
        if(!Reject<T>(prefix,value)) return false;
        prefix += word + ' ';
    }
    return true;
}
int main()
{
    using namespace Snooker;
    FBallDefinition definition; definition.Id=1;
    FBallState ball; ball.Id=1;
    FShotInput shot; shot.CueBallId=1;
    FPhysicsSnapshot snapshot; snapshot.PhysicsParameterVersion="test-v1"; snapshot.Balls={ball};
    CHECK(Truncations(definition)); CHECK(Truncations(ball)); CHECK(Truncations(shot));
    CHECK(Reject<FBallDefinition>("-1 Cue 0 0 0 0.02625 0.142 standard",definition));
    CHECK(Reject<FBallDefinition>("4294967296 Cue 0 0 0 0.02625 0.142 standard",definition));
    CHECK(Reject<FBallDefinition>("1 Orange 0 0 0 0.02625 0.142 standard",definition));
    CHECK(Reject<FBallDefinition>(definition.Serialize()+" extra",definition));
    CHECK(Reject<FBallState>(ball.Serialize()+" extra",ball));
    CHECK(Reject<FShotInput>(shot.Serialize()+" extra",shot));
    CHECK(Reject<FPhysicsSnapshot>("2 BeforeShot test-v1 0 0 0.004166666666666667 0 18446744073709551615",snapshot));
    CHECK(Reject<FPhysicsSnapshot>("2 BeforeShot test-v1 0 0 0.004166666666666667 0 -1",snapshot));
    CHECK(Reject<FPhysicsSnapshot>("2 BeforeShot test-v1 -1 0 0.004166666666666667 0 0",snapshot));
    CHECK(Reject<FPhysicsSnapshot>("3 BeforeShot test-v1 0 0 0.004166666666666667 0 0",snapshot));
    CHECK(Reject<FPhysicsSnapshot>("2 WrongPhase test-v1 0 0 0.004166666666666667 0 0",snapshot));
    CHECK(Reject<FPhysicsSnapshot>("2 BeforeShot test-v1 0 0 0.004166666666666667 0 0 header-junk",snapshot));
    CHECK(Reject<FPhysicsSnapshot>("2 BeforeShot test-v1 0 0 0.004166666666666667 0 1",snapshot));
    CHECK(Reject<FPhysicsSnapshot>(snapshot.Serialize()+"\n\ntrailing-junk",snapshot));
    auto invalidDefinition=definition; invalidDefinition.Type=static_cast<EBallType>(255); CHECK(!invalidDefinition.IsValid());
    auto invalidBall=ball; invalidBall.Contact.Kind=static_cast<EContactKind>(255); CHECK(!invalidBall.IsValid());
    CHECK(Reject<FBallState>(invalidBall.Serialize(),ball));
    auto invalidSnapshot=snapshot; invalidSnapshot.Phase=static_cast<ESnapshotPhase>(255); CHECK(!invalidSnapshot.IsValid());
    invalidSnapshot.Balls.push_back(ball); CHECK(Reject<FPhysicsSnapshot>(invalidSnapshot.Serialize(),snapshot));
    snapshot.Balls[0].PositionMeters.X=1.5;
    const std::locale previous=std::locale();
    std::locale::global(std::locale(previous,new Comma));
    const auto text=snapshot.Serialize();
    FPhysicsSnapshot restored;
    const bool loaded=FPhysicsSnapshot::Deserialize(text,restored);
    std::locale::global(previous);
    CHECK(loaded); CHECK(text.find("1.5")!=std::string::npos); CHECK(text.find("1,5")==std::string::npos); CHECK(restored==snapshot);
    std::cout << "PASS parser bounds, truncation, enum, tail, locale and unchanged-output tests\n";
}
