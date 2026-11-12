#include <Snooker/Rules/BallOn.h>
#include <algorithm>
#include <iostream>

#define CHECK(x) do {if(!(x)){std::cerr<<"FAIL BallOn line "<<__LINE__<<": "<<#x<<'\n';return 1;}}while(false)
int main()
{
    using namespace Snooker;MatchState state;BallOnQuery query;
    CHECK(GetBallOn(state,query));CHECK(query.LegalChoices==std::vector<EBallType>{EBallType::Red});CHECK(query.Selected==EBallType::Red);
    state.Phase=FramePhase::ColorAfterRed;
    for(auto color:{EBallType::Yellow,EBallType::Green,EBallType::Brown,EBallType::Blue,EBallType::Pink,EBallType::Black}) {
        state.BallOn=color;const auto original=state.Serialize();
        CHECK(GetBallOn(state,query));CHECK(query.LegalChoices.size()==6);CHECK(query.Selected==color);
        CHECK(std::find(query.LegalChoices.begin(),query.LegalChoices.end(),EBallType::Red)==query.LegalChoices.end());
        CHECK(std::find(query.LegalChoices.begin(),query.LegalChoices.end(),EBallType::Cue)==query.LegalChoices.end());
        CHECK(state.Serialize()==original);
    }
    state.RemainingReds=0;CHECK(GetBallOn(state,query));CHECK(query.LegalChoices.size()==6);
    state.Phase=FramePhase::FinalColors;
    for(auto color:{EBallType::Yellow,EBallType::Green,EBallType::Brown,EBallType::Blue,EBallType::Pink,EBallType::Black}) {
        state.BallOn=color;CHECK(GetBallOn(state,query));CHECK(query.LegalChoices.size()==1);CHECK(query.LegalChoices[0]==color);
    }
    state.Phase=FramePhase::Finished;CHECK(GetBallOn(state,query));CHECK(query.FrameFinished);CHECK(query.LegalChoices.empty());
    const auto saved=query;state=MatchState{};state.FreeBallAvailable=true;std::string error;
    CHECK(!GetBallOn(state,query,&error));CHECK(!error.empty());CHECK(query.FrameFinished==saved.FrameFinished && query.LegalChoices==saved.LegalChoices);
    state=MatchState{};state.RemainingReds=0;CHECK(!GetBallOn(state,query));CHECK(query.LegalChoices==saved.LegalChoices);
    state=MatchState{};state.ShotInProgress=true;CHECK(GetBallOn(state,query));CHECK(query.Selected==EBallType::Red);
    std::cout<<"PASS read-only Ball On red, nominated color, last-red color, final cursor, finished and unsupported FreeBall rejection\n";
}
