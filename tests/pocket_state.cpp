#include <Snooker/Physics/PocketState.h>
#include <iostream>

#define CHECK(x) do {if(!(x)){std::cerr<<"FAIL pocket state line "<<__LINE__<<": "<<#x<<'\n';return 1;}}while(false)
int main()
{
    using namespace Snooker;PocketStateData state;PocketStateUpdate out;PocketContact jaw;jaw.Hit=true;jaw.PocketId=1;jaw.Feature=PocketFeature::Jaw;
    CHECK(UpdatePocketState(state,&jaw,false,false,out));CHECK(out.State.State==PocketState::AtMouth);CHECK(out.Event==PocketEvent::None);CHECK(!out.State.PotEventEmitted);
    state=out.State;CHECK(UpdatePocketState(state,nullptr,true,false,out));CHECK(out.State.State==PocketState::InPocket);state=out.State;
    PocketContact bottom=jaw;bottom.Feature=PocketFeature::Bottom;CHECK(UpdatePocketState(state,&bottom,true,false,out));CHECK(out.State.State==PocketState::Potted);CHECK(out.Event==PocketEvent::EnteredPocket);CHECK(out.State.PotEventEmitted);state=out.State;
    CHECK(UpdatePocketState(state,nullptr,true,false,out));CHECK(out.Event==PocketEvent::None);state=out.State;
    auto saved=state;CHECK(!UpdatePocketState(state,nullptr,false,true,out));CHECK(state.State==saved.State&&state.PotEventEmitted==saved.PotEventEmitted);
    PocketStateData newState;CHECK(UpdatePocketState(newState,&jaw,false,false,out));newState=out.State;CHECK(UpdatePocketState(newState,nullptr,false,true,out));CHECK(out.State.State==PocketState::Ejected);CHECK(out.Event==PocketEvent::RecoveredToCloth);CHECK(!out.State.PotEventEmitted);
    CHECK(UpdatePocketState(out.State,nullptr,false,true,out));CHECK(out.State.State==PocketState::Ejected);CHECK(out.Event==PocketEvent::None);
    jaw.PocketId=0;CHECK(!UpdatePocketState(newState,&jaw,false,false,out));
    auto wrongPocket=bottom;wrongPocket.PocketId=2;state=PocketStateData{PocketState::InPocket,1,false};CHECK(!UpdatePocketState(state,&wrongPocket,true,false,out));
    state=PocketStateData{};CHECK(!UpdatePocketState(state,&bottom,false,true,out));
    std::cout<<"PASS desktop/mouth/in-pocket/potted/ejected state, once-only pot and explicit recovery rejection\n";
}

