#include <Snooker/Replay/ShotReplay.h>
#include <fstream>
#include <iterator>
#include <iostream>

int main(int argc,char** argv)
{
    using namespace Snooker;
    if(argc!=2){std::cerr<<"usage: snooker_replay record.shot\n";return 2;}
    std::ifstream f(argv[1]);if(!f){std::cerr<<"cannot read replay record\n";return 2;}
    const std::string text{std::istreambuf_iterator<char>(f),{}};PhysicsShotRecord record,actual;std::string error;
    if(!PhysicsShotRecord::Deserialize(text,record,&error) || !ReplayShot(record,actual,&error)) {
        std::cerr<<"FAIL "<<error<<'\n';return 1;
    }
    std::cout<<"PASS replay "<<actual.Events.size()<<" events, final step "<<actual.Final.StepIndex<<'\n';return 0;
}
