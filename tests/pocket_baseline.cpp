#include <Snooker/Service/PocketSimulation.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

using namespace Snooker;
namespace
{
void Require(bool value,const std::string& message){if(!value)throw std::runtime_error(message);}
std::string Read(const std::filesystem::path& path)
{
    std::ifstream f(path,std::ios::binary);Require(bool(f),"cannot read "+path.string());
    return {std::istreambuf_iterator<char>(f),{}};
}
void Write(const std::filesystem::path& path,const std::string& text)
{
    std::ofstream f(path,std::ios::binary);Require(bool(f),"cannot write baseline");f<<text;f.flush();Require(bool(f),"baseline flush failed");
}
}
int main(int argc,char** argv)
{
    try {
        Require(argc==3 || (argc==4 && std::string(argv[3])=="--generate"),"usage: pocket_baseline bundle baseline-directory [--generate]");
        const bool generate=argc==4;std::string error;PocketSimulationConfig config;
        Require(PocketSimulationConfig::Deserialize(Read(argv[1]),config,&error),error);
        TableGeometry geometry;Require(GenerateTableGeometry(config.Table,config.Cushions,config.Pockets,config.Geometry,geometry,&error),error);
        const auto& p=geometry.Pockets[1];PhysicsClock clock;
        const std::filesystem::path root=argv[2];
        for(const auto& mode:{"entry","jaw","mouth-stop","recovery","capture"}) {
            BallState ball;ball.Id=1;ball.RadiusMeters=0.01;ball.Sleeping=false;
            ball.PositionMeters=p.Definition.Center+p.Inward*0.001+Vec3d{0,0,ball.RadiusMeters};
            ball.LinearVelocityMetersPerSecond=p.Inward*(-0.5);
            PocketStateData state;unsigned steps=600;
            if(std::string(mode)=="jaw") {
                ball.PositionMeters=p.MouthStart-p.Tangent*0.08+Vec3d{0,0,0.005};
                ball.LinearVelocityMetersPerSecond=p.Tangent*2;state={PocketState::AtMouth,p.Definition.Id,false};steps=25;
            } else if(std::string(mode)=="mouth-stop") {
                ball.Sleeping=true;ball.LinearVelocityMetersPerSecond={};steps=100;
            } else if(std::string(mode)=="recovery") {
                ball.PositionMeters=p.Definition.Center-p.Inward*0.001+Vec3d{0,0,ball.RadiusMeters};
                ball.LinearVelocityMetersPerSecond=p.Inward*2;state={PocketState::InPocket,p.Definition.Id,false};steps=5;
            } else if(std::string(mode)=="capture") {
                ball.PositionMeters=p.BottomCenter-p.Inward*p.Parameters.JawRadius+Vec3d{0,0,0.08};
                ball.LinearVelocityMetersPerSecond={0,0,-0.1};state={PocketState::InPocket,p.Definition.Id,false};steps=150;
            }
            auto initial=PhysicsSnapshot::Capture(clock,{ball},config.Version,ESnapshotPhase::DuringStep);initial.PocketStates={state};
            const auto dir=root/mode;
            if(generate)std::filesystem::create_directories(dir);
            else Require(Read(dir/"initial.snapshot")==initial.Serialize(),std::string(mode)+" initial fixture changed");
            PocketSimulation continuous,resumed;Require(continuous.Restore(config,initial,&error),error);
            const unsigned split=steps/2;
            for(unsigned n=0;n<split;++n)Require(continuous.Step(&error),std::string(mode)+" "+error);
            PhysicsSnapshot loaded;Require(PhysicsSnapshot::Deserialize(continuous.SaveSnapshot().Serialize(),loaded,&error),error);
            Require(resumed.Restore(config,loaded,&error),error);const auto offset=continuous.GetEvents().size();
            for(unsigned n=split;n<steps;++n) {
                Require(continuous.Step(&error),std::string(mode)+" "+error);Require(resumed.Step(&error),error);
                Require(continuous.SaveSnapshot().Serialize()==resumed.SaveSnapshot().Serialize(),"mid-pocket state divergence");
            }
            Require(continuous.GetEvents().size()==offset+resumed.GetEvents().size(),"mid-pocket event count divergence");
            for(std::size_t n=0;n<resumed.GetEvents().size();++n)
                Require(continuous.GetEvents()[offset+n].Serialize()==resumed.GetEvents()[n].Serialize(),"mid-pocket event divergence");
            unsigned pots=0,entries=0,recoveries=0,jaws=0;std::string events;
            for(const auto& e:continuous.GetEvents()) {
                events+=e.Serialize()+"\n";
                if(e.Kind==PocketSimulationEventKind::Potted)++pots;
                if(e.Kind==PocketSimulationEventKind::MouthEntered)++entries;
                if(e.Kind==PocketSimulationEventKind::RecoveredToCloth)++recoveries;
                if(e.Kind==PocketSimulationEventKind::Contact && e.Feature==PocketFeature::Jaw) {
                    ++jaws;Require(e.NormalSpeedBefore<0 && e.NormalSpeedAfter>0,"jaw must physically rebound");
                    Require(NearlyEqual(e.NormalSpeedAfter,-p.Parameters.JawRestitution*e.NormalSpeedBefore,1e-10),"jaw restitution mismatch");
                }
            }
            const auto final=continuous.SaveSnapshot();
            if(std::string(mode)=="entry" || std::string(mode)=="capture") {
                Require(pots==1 && final.PocketStates[0].State==PocketState::Potted,"capture must emit exactly one pot");
                if(std::string(mode)=="entry")Require(entries==1,"entry must cross the actual mouth");
            } else Require(pots==0,"jaw/stop/recovery must not pot");
            if(std::string(mode)=="jaw")Require(jaws>0,"jaw contact missing");
            if(std::string(mode)=="mouth-stop")Require(final.Balls[0].Sleeping && events.empty(),"mouth stop must remain unpotted and quiet");
            if(std::string(mode)=="recovery")Require(recoveries==1 && final.PocketStates[0].State==PocketState::Ejected,"once-only cloth recovery missing");
            if(generate){Write(dir/"initial.snapshot",initial.Serialize());Write(dir/"final.snapshot",final.Serialize());Write(dir/"events.txt",events);}
            else {Require(Read(dir/"final.snapshot")==final.Serialize(),std::string(mode)+" final baseline mismatch");Require(Read(dir/"events.txt")==events,std::string(mode)+" event baseline mismatch");}
            std::cout<<"PASS pocket "<<mode<<" pots="<<pots<<" events="<<continuous.GetEvents().size()<<" steps="<<steps<<'\n';
        }
        return 0;
    }catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}
}
