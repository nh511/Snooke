#include <Snooker/Replay/PhysicsShotRecord.h>
#include <fstream>
#include <iterator>
#include <iostream>

#define CHECK(x) do {if(!(x)){std::cerr<<"FAIL physics record "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
using namespace Snooker;
int main(int argc,char** argv)
{
    std::string error;CHECK(argc==2);std::ifstream f(argv[1]);CHECK(bool(f));
    const std::string text{std::istreambuf_iterator<char>(f),{}};PhysicsShotRecord r;
    CHECK(PhysicsShotRecord::Deserialize(text,r,&error));CHECK(r.Serialize()==text);
    CHECK(r.Events.size()>3 && r.Final.Balls.size()==22 && r.Result=="Settled");
    const auto saved=r.Serialize();
    for(const auto& bad:{saved+"tail",saved.substr(0,saved.find("\nfinal ")),std::string("snooker-shot-v99")}) {
        CHECK(!PhysicsShotRecord::Deserialize(bad,r,&error));CHECK(r.Serialize()==saved);
    }
    auto invalid=r;std::swap(invalid.Events[0],invalid.Events[1]);CHECK(!invalid.IsValid(&error));
    invalid=r;invalid.Events[1].Ball=999;CHECK(!invalid.IsValid(&error));
    invalid=r;invalid.Parameters.Version="wrong-version";CHECK(!invalid.IsValid(&error));
    invalid=r;invalid.Final.EventCount++;CHECK(!invalid.IsValid(&error));
    invalid=r;invalid.Final.Balls.pop_back();invalid.Final.PocketStates.pop_back();CHECK(!invalid.IsValid(&error));
    invalid=r;invalid.Input.CueProfileId="wrong-cue";CHECK(!invalid.IsValid(&error));
    invalid=r;invalid.Events[1].Impulse=-1;CHECK(!invalid.IsValid(&error));
    // A record is transport, not proof of recomputed dynamics (REP-002).
    auto service=CreatePhysicsService(r.Parameters,&error);CHECK(service);
    CHECK(service->BeginShot(r.Initial,r.Input,&error));
    std::cout<<"PASS real multi-ball record round trip and malformed/order/reference/version rejection\n";
}
