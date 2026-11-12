#include <Snooker/Rules/MatchState.h>
#include <fstream>
#include <iostream>
#include <iterator>

#define CHECK(x) do {if(!(x)){std::cerr<<"FAIL match state line "<<__LINE__<<": "<<#x<<'\n';return 1;}}while(false)
int main()
{
    using namespace Snooker;
    MatchState state;CHECK(state.IsValid());
    for(auto phase:{FramePhase::Reds,FramePhase::ColorAfterRed,FramePhase::FinalColors,FramePhase::Finished}) {
        state=MatchState{};state.Phase=phase;state.FrameNumber=3;state.CurrentPlayer=1;state.Scores={42,17};
        if(phase!=FramePhase::Reds)state.BallOn=EBallType::Black;
        if(phase==FramePhase::FinalColors || phase==FramePhase::Finished)state.RemainingReds=0;
        if(phase!=FramePhase::Finished){state.FreeBallAvailable=true;state.MissPending=true;state.CueBallInHand=true;}
        CHECK(state.IsValid());
        {std::ofstream f("match-state.txt");CHECK(bool(f));CHECK(bool(f<<state.Serialize()));}
        std::ifstream f("match-state.txt");CHECK(bool(f));const std::string text((std::istreambuf_iterator<char>(f)),{});
        MatchState restored;CHECK(MatchState::Deserialize(text,restored));CHECK(restored.Serialize()==state.Serialize());
        const auto original=restored.Serialize();std::string error;
        for(const auto& bad:{text+" junk",text.substr(0,text.find_last_of(' ')),std::string("snooker-match-v2"),
            std::string("snooker-match-v1 1 Reds 2 0 0 Red 15 0 0 0 0"),
            std::string("snooker-match-v1 1 Reds 0 -1 0 Red 15 0 0 0 0"),
            std::string("snooker-match-v1 1 Reds 0 4294967296 0 Red 15 0 0 0 0"),
            std::string("snooker-match-v1 1 Reds 0 0 0 Red 15 2 0 0 0")}) {
            CHECK(!MatchState::Deserialize(bad,restored,&error));CHECK(!error.empty());CHECK(restored.Serialize()==original);
        }
        std::cout<<text<<'\n';
    }
    state=MatchState{};state.RemainingReds=0;CHECK(!state.IsValid());
    state=MatchState{};state.BallOn=EBallType::Cue;CHECK(!state.IsValid());
    state=MatchState{};state.Phase=static_cast<FramePhase>(99);CHECK(!state.IsValid());
    state=MatchState{};state.RemainingReds=16;CHECK(!state.IsValid());
    state=MatchState{};state.Phase=FramePhase::ColorAfterRed;state.BallOn=EBallType::Blue;state.RemainingReds=0;CHECK(state.IsValid());
    state.Phase=FramePhase::Finished;state.ShotInProgress=true;CHECK(!state.IsValid());
    std::cout<<"PASS independent MatchState phases, file save/restore/print, flags, score bounds and transactional rejection\n";
}
