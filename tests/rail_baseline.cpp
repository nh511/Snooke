#include <Snooker/Replay/ShotReplay.h>
#include <fstream>
#include <iterator>
#include <iostream>

#define CHECK(x) do {if(!(x)){std::cerr<<"FAIL rail "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
int main(int argc,char** argv)
{
    using namespace Snooker;
    std::string error;CHECK(argc==3);std::ifstream f(argv[1]);CHECK(bool(f));
    const std::string text{std::istreambuf_iterator<char>(f),{}};PhysicsShotRecord r;
    CHECK(PhysicsShotRecord::Deserialize(text,r,&error));
    const std::string mode=argv[2];unsigned rails=0;Seconds first=0;
    for(const auto& e:r.Events)if(e.Kind==PhysicsEventKind::Cushion){if(!rails)first=e.Time;++rails;}
    CHECK(rails>0);if(mode=="two-rail")CHECK(rails>=2);else CHECK(rails==1);
    if(mode=="touching-rail")CHECK(first<=1e-12);
    if(mode=="left") {
        CHECK(r.Events[0].Kind==PhysicsEventKind::CueStrike);
        CHECK(std::abs(r.Final.Balls[0].PositionMeters.Y-r.Initial.Balls[0].PositionMeters.Y)>0.001);
    }
    for(double fps:{30.,60.,120.,144.}) {
        auto s=CreatePhysicsService(r.Parameters,&error);CHECK(s);CHECK(s->BeginShot(r.Initial,r.Input,&error));
        PhysicsRunState state=PhysicsRunState::Running;
        for(unsigned n=0;n<50000 && state==PhysicsRunState::Running;++n){CHECK(s->Step(1/fps,state,&error));CHECK(state!=PhysicsRunState::Faulted);}
        CHECK(state==PhysicsRunState::Settled);CHECK(s->SaveSnapshot().Serialize()==r.Final.Serialize());
        CHECK(s->GetEvents().size()==r.Events.size());
        for(std::size_t n=0;n<r.Events.size();++n)CHECK(s->GetEvents()[n].Serialize()==r.Events[n].Serialize());
    }
    PhysicsShotRecord replay;CHECK(ReplayShot(r,replay,&error));CHECK(replay.Serialize()==r.Serialize());
    std::cout<<"PASS "<<mode<<" rails="<<rails<<" FPS/event/snapshot/replay parity\n";
}
