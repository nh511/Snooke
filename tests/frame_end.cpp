#include <Snooker/Rules/FrameEnd.h>
#include <fstream>
#include <iterator>
#include <iostream>
#define CHECK(x) do{if(!(x)){std::cerr<<"FAIL frame end "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
std::string Read(const char* p){std::ifstream f(p);return {std::istreambuf_iterator<char>(f),{}};}
int main(int argc,char** argv){using namespace Snooker;std::string error;CHECK(argc==3);MatchState state;state.Phase=FramePhase::Finished;state.RemainingReds=0;state.BallOn=EBallType::Black;state.Scores={70,63};FrameEndResult result;CHECK(ResolveFrameEnd(state,0,result,&error));CHECK(result.FrameEnded&&result.Winner==0&&!result.NeedsRespottedBlack);
 state.Scores={70,70};CHECK(ResolveFrameEnd(state,1,result,&error));CHECK(result.NeedsRespottedBlack&&!result.FrameEnded&&result.State.RespottedBlack&&result.State.CurrentPlayer==1);
 auto black=result.State;black.Scores[1]+=7;CHECK(ResolveFrameEnd(black,0,result,&error));CHECK(result.FrameEnded&&result.Winner==1&&result.State.Phase==FramePhase::Finished);
 const auto saved=result.State.Serialize();CHECK(!ResolveFrameEnd(MatchState{},0,result,&error));CHECK(result.State.Serialize()==saved);CHECK(!ResolveFrameEnd(state,2,result,&error));
 TableDefinition table;OpeningLayout layout;CHECK(TableDefinition::Deserialize(Read(argv[1]),table,&error));CHECK(OpeningLayout::Deserialize(Read(argv[2]),layout,&error));BallDefinition prototype;prototype.Id=1;FrameOpening opening;CHECK(StartNextFrame(result.State,table,prototype,layout,"next-v1",1.0/240,0,opening,&error));CHECK(opening.Match.FrameNumber==2&&opening.Match.CurrentPlayer==0&&opening.Match.Scores[0]==0&&opening.Definitions.size()==22);
 const auto old=result.State.Serialize();CHECK(!StartNextFrame(state,table,prototype,layout,"next-v1",1.0/240,0,opening,&error));CHECK(result.State.Serialize()==old);
 std::cout<<"PASS unequal winner, tied respotted black, black decision and deterministic next frame\n";}
