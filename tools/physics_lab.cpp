#include <Snooker/Service/ISnookerPhysicsService.h>
#include <Snooker/Validation/DebugOptions.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <stdexcept>

namespace
{
std::string Read(const std::filesystem::path& Path)
{
    std::ifstream File(Path, std::ios::binary);
    if (!File) throw std::runtime_error("cannot read " + Path.string());
    return {std::istreambuf_iterator<char>(File), {}};
}

void Write(const std::filesystem::path& Path, const std::string& Text)
{
    std::ofstream File(Path, std::ios::binary);
    if (!File || !(File << Text)) throw std::runtime_error("cannot write " + Path.string());
}

Snooker::BallState Ball(Snooker::BallId Id, double X, double Y)
{
    Snooker::BallState Result;
    Result.Id=Id;Result.PositionMeters={X,Y,Result.RadiusMeters};Result.Sleeping=true;
    return Result;
}

struct Scenario
{
    const char* Name;
    const char* Description;
    Snooker::Vec3d Position;
    Snooker::Vec3d Direction;
    double Speed;
    std::vector<Snooker::BallState> Balls;
    const char* Expected;
};

Scenario MakeScenario(const std::string& Name)
{
    using namespace Snooker;
    if (Name=="single-ball") return {"single-ball","free cloth travel",{-0.8,0,0},{1,0,0},0.25,{Ball(1,-0.8,0)},"BallSettled"};
    if (Name=="ball-collision") return {"ball-collision","cue ball into an object ball",{-0.8,0,0},{1,0,0},1.0,{Ball(1,-0.8,0),Ball(2,-0.3,0)},"BallBall"};
    if (Name=="cushion") return {"cushion","single ball rail response",{0,0,0},{1,0,0},1.5,{Ball(1,0,0)},"Cushion"};
    if (Name=="pocket") return {"pocket","lower middle pocket approach",{0,-0.6,0},{0,-1,0},1.0,{Ball(1,0,-0.6)},"MouthEntered/PocketContact/Potted"};
    throw std::runtime_error("unknown physics lab scenario: " + Name);
}

void WriteScenario(const std::filesystem::path& Root, const Snooker::PhysicsBundle& Bundle, const Scenario& Spec)
{
    using namespace Snooker;
    const auto Directory=Root/Spec.Name;std::filesystem::create_directories(Directory);
    PhysicsClock Clock;auto Snapshot=PhysicsSnapshot::Capture(Clock,Spec.Balls,Bundle.Version,ESnapshotPhase::BeforeShot);
    ShotInput Input;Input.CueBallId=1;Input.CueDirection=Spec.Direction;Input.ForwardSpeedMetersPerSecond=Spec.Speed;
    std::string Error;
    if (!Input.IsValid(&Error)) throw std::runtime_error(std::string(Spec.Name)+": "+Error);
    Write(Directory/"initial.snapshot",Snapshot.Serialize());Write(Directory/"shot.input",Input.Serialize());
    std::ostringstream Manifest;Manifest.imbue(std::locale::classic());Manifest<<std::setprecision(17)
      <<"{\"schema\":\"snooker-physics-lab-v1\",\"name\":\""<<Spec.Name
      <<"\",\"description\":\""<<Spec.Description<<"\",\"parameterVersion\":\""<<Bundle.Version
      <<"\",\"position\":["<<Spec.Position.X<<','<<Spec.Position.Y<<','<<Spec.Position.Z<<"]"
      <<",\"direction\":["<<Spec.Direction.X<<','<<Spec.Direction.Y<<','<<Spec.Direction.Z<<"]"
      <<",\"speed_mps\":"<<Spec.Speed<<",\"ballCount\":"<<Spec.Balls.size()
      <<",\"expectedEvents\":\""<<Spec.Expected<<"\"}\n";
    Write(Directory/"scenario.json",Manifest.str());
}
}

int main(int argc, char** argv)
{
    using namespace Snooker;
    try {
        if (argc<3 || argc>4) throw std::runtime_error("usage: snooker_physics_lab bundle output-directory [scenario]");
        PhysicsBundle Bundle;std::string Error;
        if (!PhysicsBundle::Deserialize(Read(argv[1]),Bundle,&Error)) throw std::runtime_error(Error);
        const std::filesystem::path Root=argv[2];
        if (std::filesystem::exists(Root) && !std::filesystem::is_empty(Root)) throw std::runtime_error("output directory is not empty");
        std::filesystem::create_directories(Root);
        const std::vector<std::string> Names=argc==4?std::vector<std::string>{argv[3]}:
            std::vector<std::string>{"single-ball","ball-collision","cushion","pocket"};
        std::ostringstream Index;Index<<"{\"schema\":\"snooker-physics-lab-index-v1\",\"parameterVersion\":\""<<Bundle.Version<<"\",\"scenarios\":[";
        for (std::size_t i=0;i<Names.size();++i) {
            const auto Spec=MakeScenario(Names[i]);WriteScenario(Root,Bundle,Spec);
            if (i) Index<<',';Index<<'"'<<Spec.Name<<'"';
        }
        Index<<"]}\n";Write(Root/"manifest.json",Index.str());
        std::cout<<"PASS physics lab scenarios="<<Names.size()<<" output="<<Root.string()<<'\n';return 0;
    } catch (const std::exception& Exception) { std::cerr<<"FAIL "<<Exception.what()<<'\n';return 1; }
}
