#include <Snooker/Service/PocketSimulation.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

namespace
{
std::string Read(const char* path)
{
    std::ifstream file(path);
    if(!file)throw std::runtime_error(std::string("cannot read ")+path);
    return {std::istreambuf_iterator<char>(file),{}};
}
}
int main(int argc,char** argv)
{
    using namespace Snooker;
    try {
        if(argc!=8)throw std::runtime_error("usage: snooker_pocket_bundle table cushion pocket cloth ball-contact cue output");
        PocketSimulationConfig config;std::string error;
        if(!TableDefinition::Deserialize(Read(argv[1]),config.Table,&error))throw std::runtime_error(error);
        CushionProfile cushion;PocketProfile pocket;ClothProfile cloth;
        if(!CushionProfile::Deserialize(Read(argv[2]),cushion,&error) ||
           !PocketProfile::Deserialize(Read(argv[3]),pocket,&error) ||
           !ClothProfile::Deserialize(Read(argv[4]),cloth,&error) ||
           !BallCollisionProfile::Deserialize(Read(argv[5]),config.BallCollision,&error) ||
           !CueProfile::Deserialize(Read(argv[6]),config.Cue,&error))throw std::runtime_error(error);
        config.Version=config.Table.ParameterVersion;config.Cushions={cushion};config.Pockets={pocket};config.Cloth=cloth;
        config.Geometry.RequireClosedBoundary=true;
        if(!config.IsValid(&error))throw std::runtime_error(error);
        std::ofstream file(argv[7]);if(!file || !(file<<config.Serialize()))throw std::runtime_error("cannot write bundle");
        file.flush();if(!file)throw std::runtime_error("bundle flush failed");
        std::cout<<"PASS physics bundle generated: "<<argv[7]<<'\n';return 0;
    } catch(const std::exception& e) {std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}
}
