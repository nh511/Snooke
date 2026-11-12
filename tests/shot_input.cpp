#include <Snooker/Shot/ShotInput.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>

#define CHECK(x) do { if(!(x)) { std::cerr<<"FAIL input line "<<__LINE__<<": "<<#x<<'\n'; return 1; } } while(false)
int main()
{
    using namespace Snooker;
    BallState ball; ball.Id=1;
    CueProfile cue;
    ShotInput input; input.CueBallId=1; input.CueDirection={2,0,0};
    input.StrikePointOnBall={0,0,0.25}; input.CueTipOffsetMeters=input.StrikePointOnBall*ball.RadiusMeters;
    input.PullbackDistanceMeters=0.1; input.ForwardSpeedMetersPerSecond=2; input.InputTimeSeconds=5;
    CHECK(input.ValidateFor(cue,ball));
    {std::ofstream file("shot.input"); CHECK(bool(file)); CHECK(bool(file<<input.Serialize()));}
    std::ifstream file("shot.input"); CHECK(bool(file));
    const std::string text((std::istreambuf_iterator<char>(file)),{});
    ShotInput loaded; CHECK(ShotInput::Deserialize(text,loaded)); CHECK(loaded.Serialize()==input.Serialize());
    CHECK(loaded.ValidateFor(cue,ball));
    const auto original=loaded.Serialize(); CHECK(!ShotInput::Deserialize(text+" junk",loaded)); CHECK(loaded.Serialize()==original);
    auto bad=input; bad.CueProfileId="other"; CHECK(!bad.ValidateFor(cue,ball));
    bad=input; bad.CueBallId=2; CHECK(!bad.ValidateFor(cue,ball));
    bad=input; bad.CueTipOffsetMeters={}; CHECK(!bad.ValidateFor(cue,ball));
    bad=input; bad.StrikePointOnBall={0.1,0,0}; CHECK(!bad.IsValid());
    bad=input; bad.CueDirection={std::numeric_limits<double>::max(),std::numeric_limits<double>::max(),0}; CHECK(!bad.IsValid());
    bad=input; bad.PullbackDistanceMeters=cue.MaxPullbackMeters+1; CHECK(!bad.ValidateFor(cue,ball));
    bad=input; bad.ForwardSpeedMetersPerSecond=cue.MaxForwardSpeedMetersPerSecond+1; CHECK(!bad.ValidateFor(cue,ball));
    std::cout<<"PASS ShotInput persistence, direction/transverse geometry, cue limits and unchanged parse failure\n";
}
