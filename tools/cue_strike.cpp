#include <Snooker/Shot/CueImpulse.h>
#include <Snooker/Replay/ShotRecord.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

std::string Read(const char* Path)
{
    std::ifstream f(Path); if(!f) throw std::runtime_error(std::string("cannot read ")+Path);
    return {std::istreambuf_iterator<char>(f),{}};
}
int main(int argc,char** argv)
{
    using namespace Snooker;
    try {
        CoreConfig config; PhysicsSnapshot initial; ShotInput input; std::string error; const char* output=nullptr;
        if(argc==7 && std::string(argv[1])=="--files") {
            if(!CoreConfig::Deserialize(Read(argv[2]),config,&error) ||
               !PhysicsSnapshot::Deserialize(Read(argv[3]),initial,&error) ||
               !ShotInput::Deserialize(Read(argv[4]),input,&error) || std::string(argv[5])!="--output")
                throw std::runtime_error(error.empty()?"invalid output flag":error);
            output=argv[6];
        }
        else if(argc==5 && std::string(argv[1])=="--record" && std::string(argv[3])=="--output") {
            ShotRecord record;
            if(!ShotRecord::Deserialize(Read(argv[2]),record,&error)) throw std::runtime_error(error);
            config=record.Parameters; initial=record.Initial; input=record.Input; output=argv[4];
        }
        else throw std::runtime_error("usage: --files config snapshot input --output path OR --record record --output path");
        PhysicsClock clock; std::vector<BallState> balls;
        if(!initial.Restore(clock,balls,config.ParameterVersion,&error)) throw std::runtime_error(error);
        if(initial.Phase!=ESnapshotPhase::BeforeShot || initial.FixedStepSeconds!=config.Solver.FixedStep ||
           balls.size()!=1 || config.Balls.size()!=1 || balls[0].Id!=config.Balls[0].Id ||
           balls[0].RadiusMeters!=config.Balls[0].RadiusMeters || balls[0].MassKg!=config.Balls[0].MassKg ||
           balls[0].MaterialProfileId!=config.Balls[0].MaterialProfileId)
            throw std::runtime_error("single-ball snapshot/config mismatch");
        CueImpulseResult hit;
        if(!ApplyCueImpulse(balls[0],input,config.Cue,hit,&error)) throw std::runtime_error(error);
        const auto snapshot=PhysicsSnapshot::Capture(clock,balls,config.ParameterVersion,ESnapshotPhase::DuringStep);
        std::ofstream file(output);
        if(!file || !(file<<snapshot.Serialize())) throw std::runtime_error("cannot write struck snapshot");
        file.flush(); if(!file) throw std::runtime_error("struck snapshot flush failed");
        std::cout<<"PASS cue impulse="<<hit.ImpulseNewtonSeconds<<" N s; no simulation time advanced\n";
    }
    catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}
}
