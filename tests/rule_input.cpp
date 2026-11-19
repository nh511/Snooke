#include <Snooker/Rules/ShotPhysicsResult.h>
#include <Snooker/Rules/ShotScoring.h>
#include <fstream>
#include <iterator>
#include <iostream>
#include <algorithm>

#define CHECK(x) do{if(!(x)){std::cerr<<"FAIL rule input "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
int main(int argc,char** argv)
{
    using namespace Snooker;std::string error;CHECK(argc==2);std::ifstream f(argv[1]);CHECK(bool(f));
    const std::string text{std::istreambuf_iterator<char>(f),{}};PhysicsShotRecord record;
    CHECK(PhysicsShotRecord::Deserialize(text,record,&error));
    std::vector<BallDefinition> definitions;
    for(const auto& b:record.Initial.Balls){BallDefinition d;d.Id=b.Id;d.InitialPositionMeters=b.PositionMeters;d.RadiusMeters=b.RadiusMeters;d.MassKg=b.MassKg;d.MaterialProfileId=b.MaterialProfileId;d.Type=b.Id==record.Input.CueBallId?EBallType::Cue:EBallType::Red;definitions.push_back(d);}
    ShotPhysicsResult result;CHECK(BuildShotRuleInput(record,definitions,result,&error));CHECK(result.IsValid(&error));
    CHECK(result.FirstContact==2);CHECK(result.Potted.empty());CHECK(!result.CueBallPotted);
    const auto saved=result.Final.Serialize();auto bad=definitions;bad[1].Id=bad[0].Id;
    CHECK(!BuildShotRuleInput(record,bad,result,&error));CHECK(result.Final.Serialize()==saved);
    bad=definitions;bad[0].Type=EBallType::Red;CHECK(!BuildShotRuleInput(record,bad,result,&error));
    auto forged=result;forged.FirstContact=0;CHECK(!forged.IsValid(&error));
    forged=result;forged.Potted.push_back(2);CHECK(!forged.IsValid(&error));
    forged=result;forged.Events[0].Sequence++;CHECK(!forged.IsValid(&error));
    // Read-only event projection and ball metadata do not mutate physical record.
    CHECK(record.Serialize()==text);
    // Pot input is projected from a real service result.
    auto service=CreatePhysicsService(record.Parameters,&error);CHECK(service);
    auto initial=record.Initial;initial.Balls[0].PositionMeters={0,-0.6,initial.Balls[0].RadiusMeters};
    initial.Balls[1].PositionMeters={0,-0.78,initial.Balls[1].RadiusMeters};
    auto input=record.Input;input.CueDirection={0,-1,0};input.CueTipOffsetMeters={};input.StrikePointOnBall={};input.ForwardSpeedMetersPerSecond=1;
    CHECK(service->BeginShot(initial,input,&error));PhysicsRunState run=PhysicsRunState::Running;
    for(unsigned n=0;n<10000 && run==PhysicsRunState::Running;++n){CHECK(service->Step(1.0/60,run,&error));CHECK(run!=PhysicsRunState::Faulted);}
    CHECK(run==PhysicsRunState::Settled);auto pot=record;pot.Initial=initial;pot.Input=input;pot.Final=service->SaveSnapshot();pot.Events=service->GetEvents();
    CHECK(pot.IsValid(&error));CHECK(BuildShotRuleInput(pot,definitions,result,&error));CHECK(result.FirstContact==2);
    CHECK(std::find(result.Potted.begin(),result.Potted.end(),2)!=result.Potted.end());
    CHECK(!result.CueBallPotted);CHECK(result.IsValid(&error));
    MatchState match;match.RemainingReds=1;ShotScoreResult score;
    CHECK(EvaluateShot(match,result,score,&error));CHECK(score.Points==1 && score.State.Scores[0]==1 && score.ContinueBreak);
    std::cout<<"PASS read-only rules event input, first contact, real pot and invalid metadata/cache rejection\n";
}
