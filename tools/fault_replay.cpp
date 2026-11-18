#include <Snooker/Service/PocketSimulation.h>
#include <fstream>
#include <iostream>
#include <iterator>

int main(int argc,char** argv)
{
    using namespace Snooker;
    if(argc!=2){std::cerr<<"usage: snooker_fault_replay fault-file\n";return 2;}
    std::ifstream file(argv[1]);
    if(!file){std::cerr<<"cannot open fault file\n";return 2;}
    const std::string text{std::istreambuf_iterator<char>(file),{}};
    PhysicsFault recorded;PocketSimulationConfig config;std::string error;
    if(!PhysicsFault::Deserialize(text,recorded,&error) || recorded.ReplayConfig.empty() ||
       !PocketSimulationConfig::Deserialize(recorded.ReplayConfig,config,&error)) {
        std::cerr<<"fault has no executable replay context: "<<error<<'\n';return 2;
    }
    PocketSimulation service;
    if(!service.Restore(config,recorded.Snapshot,&error)){std::cerr<<error<<'\n';return 2;}
    if(service.Step(&error) || !service.IsFaulted()) {
        std::cerr<<"FAIL recorded fault did not recur\n";return 1;
    }
    const auto& actual=service.GetFault();
    if(actual.Code!=recorded.Code || actual.Step!=recorded.Step || actual.Serialize()!=recorded.Serialize()) {
        std::cerr<<"FAIL replay fault differs from recording\n";return 1;
    }
    std::cout<<"PASS reproduced "<<PhysicsFaultCodeName(actual.Code)<<" at step "<<actual.Step<<'\n';return 0;
}
