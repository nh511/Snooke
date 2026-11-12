#include <Snooker/Table/BallPlacement.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>

#define CHECK(x) do {if(!(x)){std::cerr<<"FAIL placement line "<<__LINE__<<": "<<#x<<'\n';return 1;}}while(false)
std::string Read(const char* p){std::ifstream f(p);return {std::istreambuf_iterator<char>(f),{}};}
std::string Encode(const std::vector<Snooker::BallDefinition>& balls){std::string s;for(const auto& b:balls)s+=b.Serialize()+'\n';return s;}
int main(int argc,char** argv)
{
    using namespace Snooker;CHECK(argc==3);TableDefinition table;OpeningLayout layout;
    CHECK(TableDefinition::Deserialize(Read(argv[1]),table));CHECK(OpeningLayout::Deserialize(Read(argv[2]),layout));
    OpeningLayout loaded;CHECK(OpeningLayout::Deserialize(layout.Serialize(),loaded));CHECK(layout.Serialize()==loaded.Serialize());
    BallDefinition prototype;prototype.Id=1;
    std::vector<BallDefinition> opening,repeated;CHECK(GenerateOpeningPlacement(table,prototype,layout,opening));
    CHECK(opening.size()==22);CHECK(std::count_if(opening.begin(),opening.end(),[](const auto& b){return b.Type==EBallType::Red;})==15);
    CHECK(opening.front().Type==EBallType::Cue);CHECK(opening.back().Id==22);
    CHECK(opening[7].InitialPositionMeters.NearlyEquals(layout.RedApex));
    CHECK(NearlyEqual((opening[8].InitialPositionMeters-opening[7].InitialPositionMeters).Length(),2*prototype.RadiusMeters+layout.RedClearance));
    for(int i=0;i<100;++i){CHECK(GenerateOpeningPlacement(table,prototype,layout,repeated));CHECK(Encode(opening)==Encode(repeated));}
    std::reverse(repeated.begin(),repeated.end());CHECK(GenerateCustomPlacement(table,repeated,repeated));CHECK(Encode(repeated)==Encode(opening));
    std::vector<BallDefinition> training;CHECK(GenerateTrainingPlacement(table,training));CHECK(training.size()==1);
    BallDefinition reset;auto black=opening[6];CHECK(black.Type==EBallType::Black);
    auto occupied=opening;occupied.erase(occupied.begin()+6);
    CHECK(ResetColorPlacement(table,black,occupied,reset));CHECK(reset.Serialize()==black.Serialize());
    const auto savedReset=reset.Serialize();auto blocker=black;blocker.Id=99;occupied.push_back(blocker);std::string error;
    CHECK(!ResetColorPlacement(table,black,occupied,reset,&error));CHECK(!error.empty());CHECK(reset.Serialize()==savedReset);
    const auto saved=Encode(opening);auto bad=layout;bad.RedApex={0,0,prototype.RadiusMeters};
    CHECK(!GenerateOpeningPlacement(table,prototype,bad,opening));CHECK(Encode(opening)==saved);
    auto incomplete=table;incomplete.Spots.pop_back();CHECK(!GenerateOpeningPlacement(incomplete,prototype,layout,opening));CHECK(Encode(opening)==saved);
    auto duplicate=training;duplicate.push_back(training[0]);CHECK(!GenerateCustomPlacement(table,duplicate,opening));CHECK(Encode(opening)==saved);
    auto outside=training;outside[0].InitialPositionMeters.X=table.Cloth.Max.X;CHECK(!GenerateCustomPlacement(table,outside,opening));
    const auto oldLayout=loaded.Serialize();CHECK(!OpeningLayout::Deserialize(layout.Serialize()+" junk",loaded));CHECK(loaded.Serialize()==oldLayout);
    CHECK(!ResetColorPlacement(table,training[0],{},reset));
    std::cout<<"PASS deterministic 22-ball rack, all color spots, training/custom order, nonoverlap and occupied reset rejection\n";
}
