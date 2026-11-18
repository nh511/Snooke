#include <Snooker/Service/ISnookerPhysicsService.h>
#include <Snooker/Replay/PhysicsShotRecord.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

namespace
{
std::string Read(const char* path){std::ifstream f(path);if(!f)throw std::runtime_error("cannot open input");return {std::istreambuf_iterator<char>(f),{}};}
void Write(const std::filesystem::path& path,const std::string& text)
{
    std::ofstream f(path,std::ios::binary);if(!f || !(f<<text))throw std::runtime_error("cannot write simulation output");
    f.flush();if(!f)throw std::runtime_error("simulation output flush failed");
}
}
int main(int argc,char** argv)
{
    using namespace Snooker;
    try {
        if(argc!=5)throw std::runtime_error("usage: snooker_simulate bundle snapshot shot-input output-directory");
        PhysicsBundle bundle;PhysicsSnapshot initial;ShotInput input;std::string error;
        if(!PhysicsBundle::Deserialize(Read(argv[1]),bundle,&error) || !PhysicsSnapshot::Deserialize(Read(argv[2]),initial,&error) ||
           !ShotInput::Deserialize(Read(argv[3]),input,&error))throw std::runtime_error(error);
        auto service=CreatePhysicsService(bundle,&error);if(!service || !service->BeginShot(initial,input,&error))throw std::runtime_error(error);
        const std::filesystem::path out=argv[4];std::filesystem::create_directories(out);
        // Never leave a stale successful final snapshot when this run fails.
        if(std::filesystem::exists(out/"final.snapshot") || std::filesystem::exists(out/"events.txt"))
            throw std::runtime_error("output directory contains previous simulation results");
        PhysicsRunState state=PhysicsRunState::Running;
        for(unsigned frame=0;frame<100000 && state==PhysicsRunState::Running;++frame) {
            if(!service->Step(1.0/60,state,&error))throw std::runtime_error(error);
        }
        PhysicsShotRecord record;record.Parameters=bundle;record.Initial=initial;record.Input=input;
        record.Final=service->SaveSnapshot();record.Events=service->GetEvents();
        if(state==PhysicsRunState::Faulted){
            record.Fault=service->GetFault();record.Result=std::string("fault:")+PhysicsFaultCodeName(record.Fault.Code);
            Write(out/"fault.txt",record.Fault.Serialize());
            if(record.IsValid())Write(out/"record.shot",record.Serialize());throw std::runtime_error(error);
        }
        if(state!=PhysicsRunState::Settled)throw std::runtime_error("frame budget exhausted");
        if(!record.IsValid(&error))throw std::runtime_error(error);
        Write(out/"record.shot",record.Serialize());
        std::string events;for(const auto& event:service->GetEvents())events+=event.Serialize()+"\n";
        Write(out/"events.txt",events);Write(out/"final.snapshot",service->SaveSnapshot().Serialize());
        std::cout<<"PASS settled step="<<service->SaveSnapshot().StepIndex<<" events="<<service->GetEvents().size()<<'\n';
        return 0;
    }catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}
}
