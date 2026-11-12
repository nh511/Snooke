#include <Snooker/Rules/FrameOpening.h>
#include <fstream>
#include <iostream>
#include <iterator>

#define CHECK(x) do {if(!(x)){std::cerr<<"FAIL frame opening line "<<__LINE__<<": "<<#x<<'\n';return 1;}}while(false)
std::string Read(const char* p){std::ifstream f(p);return {std::istreambuf_iterator<char>(f),{}};}
int main(int argc,char** argv)
{
    using namespace Snooker;CHECK(argc==3);TableDefinition table;OpeningLayout layout;BallDefinition prototype;prototype.Id=1;
    CHECK(TableDefinition::Deserialize(Read(argv[1]),table));CHECK(OpeningLayout::Deserialize(Read(argv[2]),layout));
    FrameOpening opening;CHECK(CreateFrameOpening(table,prototype,layout,"frame-physics-v1",1.0/240,1,0,opening));
    CHECK(opening.Definitions.size()==22 && opening.Physics.Balls.size()==22);
    CHECK(opening.Match.Scores[0]==0 && opening.Match.Scores[1]==0 && opening.Match.RemainingReds==15);
    CHECK(opening.Match.BallOn==EBallType::Red && opening.Match.CurrentPlayer==0 && opening.Match.Phase==FramePhase::Reds);
    CHECK(opening.Physics.StepIndex==0 && opening.Physics.AccumulatorSeconds==0);
    for(const auto& b:opening.Physics.Balls){CHECK(b.Sleeping);CHECK(b.LinearVelocityMetersPerSecond.Length()==0);CHECK(b.AngularVelocityRadiansPerSecond.Length()==0);CHECK(b.Contact.Kind==EContactKind::Cloth);}
    const auto state=opening.Match.Serialize(),physics=opening.Physics.Serialize();
    for(int i=0;i<50;++i){
        opening.Match.Scores={91,54};opening.Match.MissPending=true;opening.Physics.Balls[0].PositionMeters.X+=1;
        CHECK(CreateFrameOpening(table,prototype,layout,"frame-physics-v1",1.0/240,1,0,opening));
        CHECK(opening.Match.Serialize()==state && opening.Physics.Serialize()==physics);
    }
    std::string error;
    CHECK(!CreateFrameOpening(table,prototype,layout,"frame-physics-v1",1.0/240,1,2,opening,&error));CHECK(!error.empty());
    CHECK(opening.Match.Serialize()==state && opening.Physics.Serialize()==physics);
    CHECK(!CreateFrameOpening(table,prototype,layout,"",1.0/240,1,0,opening));CHECK(opening.Physics.Serialize()==physics);
    CHECK(!CreateFrameOpening(table,prototype,layout,"v1",0,1,0,opening));CHECK(opening.Physics.Serialize()==physics);
    auto missing=table;missing.Spots.clear();CHECK(!CreateFrameOpening(missing,prototype,layout,"v1",1.0/240,1,0,opening));CHECK(opening.Physics.Serialize()==physics);
    CHECK(CreateFrameOpening(table,prototype,layout,"frame-physics-v1",1.0/240,2,1,opening));
    CHECK(opening.Match.FrameNumber==2 && opening.Match.CurrentPlayer==1);
    std::cout<<"PASS actual opening rack/score/state restart, stationary snapshot and transactional invalid options\n";
}
