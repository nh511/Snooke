#include <Snooker/Replay/ShotReplay.h>
#include <fstream>
#include <iterator>
#include <iostream>

#define CHECK(x) do {if(!(x)){std::cerr<<"FAIL replay "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
int main(int argc,char** argv)
{
    using namespace Snooker;
    std::string error;CHECK(argc==2);std::ifstream f(argv[1]);CHECK(bool(f));
    const std::string text{std::istreambuf_iterator<char>(f),{}};PhysicsShotRecord record,actual;
    CHECK(PhysicsShotRecord::Deserialize(text,record,&error));CHECK(ReplayShot(record,actual,&error));
    CHECK(actual.Serialize()==record.Serialize());const auto saved=actual.Serialize();
    auto bad=record;bad.Events[0].Impulse+=1e-6;CHECK(bad.IsValid(&error));
    CHECK(!ReplayShot(bad,actual,&error));CHECK(error.find("event mismatch")!=std::string::npos);CHECK(actual.Serialize()==saved);
    bad=record;bad.Final.Balls[0].PositionMeters.X+=1e-6;CHECK(bad.IsValid(&error));
    CHECK(!ReplayShot(bad,actual,&error));CHECK(error.find("final snapshot")!=std::string::npos);CHECK(actual.Serialize()==saved);
    bad=record;bad.BuildVersion="other-solver";CHECK(!ReplayShot(bad,actual,&error));
    std::cout<<"PASS physical replay and tampered event/final/build rejection\n";
}
