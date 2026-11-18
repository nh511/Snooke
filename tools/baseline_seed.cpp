#include <Snooker/Service/ISnookerPhysicsService.h>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <iostream>

int main(int argc,char** argv)
{
    using namespace Snooker;
    if(argc!=3 && argc!=4)return 2;
    std::ifstream input(argv[1]);if(!input)return 2;
    const std::string text{std::istreambuf_iterator<char>(input),{}};PhysicsBundle bundle;std::string error;
    if(!PhysicsBundle::Deserialize(text,bundle,&error)){std::cerr<<error;return 1;}
    const std::filesystem::path out=argv[2];std::filesystem::create_directories(out);
    BallState ball;ball.Id=1;ball.PositionMeters={0,0,ball.RadiusMeters};
    std::vector<BallState> balls{ball};
    const std::string mode=argc==4?argv[3]:"center";
    if(mode=="follow" || mode=="draw") {auto target=ball;target.Id=2;target.PositionMeters.X=0.075;balls.push_back(target);}
    PhysicsClock clock;auto initial=PhysicsSnapshot::Capture(clock,balls,bundle.Version,ESnapshotPhase::BeforeShot);
    ShotInput shot;shot.CueBallId=1;shot.ForwardSpeedMetersPerSecond=0.2;
    if(mode=="follow" || mode=="draw") {
        shot.ForwardSpeedMetersPerSecond=0.6;shot.StrikePointOnBall={0,0,mode=="follow"?0.5:-0.5};
        shot.CueTipOffsetMeters=shot.StrikePointOnBall*ball.RadiusMeters;
    }
    auto changed=shot;changed.ForwardSpeedMetersPerSecond+=1e-6;
    std::ofstream a(out/"initial.snapshot",std::ios::binary),b(out/"center.input",std::ios::binary),c(out/"changed.input",std::ios::binary);
    a<<initial.Serialize();b<<shot.Serialize();c<<changed.Serialize();
    a.flush();b.flush();c.flush();return a && b && c?0:1;
}
