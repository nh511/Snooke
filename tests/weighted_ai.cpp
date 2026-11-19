#include <Snooker/AI/WeightedAI.h>
#include <fstream>
#include <iterator>
#include <iostream>

#define CHECK(x) do{if(!(x)){std::cerr<<"FAIL weighted AI "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
std::string Read(const char* path){std::ifstream file(path);return {std::istreambuf_iterator<char>(file),{}};}
int main(int argc,char** argv)
{
    using namespace Snooker;std::string error;CHECK(argc==2);PhysicsBundle bundle;
    CHECK(PhysicsBundle::Deserialize(Read(argv[1]),bundle,&error));TableGeometry geometry;
    CHECK(GenerateTableGeometry(bundle.Table,bundle.Cushions,bundle.Pockets,bundle.Geometry,geometry,&error));
    BallState cue;cue.Id=1;cue.PositionMeters={0,-.6,cue.RadiusMeters};auto red=cue;red.Id=2;red.PositionMeters={0,-.78,red.RadiusMeters};
    PhysicsClock clock;auto physics=PhysicsSnapshot::Capture(clock,{cue,red},bundle.Version,ESnapshotPhase::BeforeShot);
    BallDefinition cueDef;cueDef.Id=1;cueDef.Type=EBallType::Cue;cueDef.InitialPositionMeters=cue.PositionMeters;
    auto redDef=cueDef;redDef.Id=2;redDef.Type=EBallType::Red;redDef.InitialPositionMeters=red.PositionMeters;
    MatchState match;auto query=CreateAIStateQuery(match,physics,{cueDef,redDef},bundle.Cue,&error);CHECK(query);
    std::vector<ShotEvaluation> evaluations;
    for(std::uint32_t pocket:{2U,3U,5U})for(double speed:{.6,1.0}) {
        ShotCandidate candidate;if(!CreatePotCandidate(*query,geometry,2,pocket,speed,candidate,&error))continue;
        ShotEvaluation evaluation;if(EvaluateCandidate(*query,bundle,candidate,evaluation,&error))evaluations.push_back(evaluation);
    }
    CHECK(evaluations.size()>=4);WeightedAIResult easy,medium,hard,repeat;std::uint32_t selectedSeed=0;
    for(std::uint32_t seed=1;seed<=1000&&!selectedSeed;++seed){WeightedAIResult probeEasy,probeHard;CHECK(SelectWeightedShot(evaluations,DifficultyPreset(AIDifficulty::Easy,seed),probeEasy,&error));CHECK(SelectWeightedShot(evaluations,DifficultyPreset(AIDifficulty::Hard,seed),probeHard,&error));if(probeEasy.Index!=probeHard.Index){selectedSeed=seed;easy=probeEasy;hard=probeHard;}}
    CHECK(selectedSeed!=0);CHECK(SelectWeightedShot(evaluations,DifficultyPreset(AIDifficulty::Medium,selectedSeed),medium,&error));CHECK(SelectWeightedShot(evaluations,DifficultyPreset(AIDifficulty::Easy,selectedSeed),repeat,&error));
    CHECK(repeat.Index==easy.Index&&repeat.Detail==easy.Detail);
    CHECK(hard.Evaluation.LegalPotted>=easy.Evaluation.LegalPotted);CHECK(easy.Index!=hard.Index);
    CHECK(easy.Detail.find("safety=")!=std::string::npos&&hard.Detail.find("offense=")!=std::string::npos);
    PhysicsShotRecord replay;CHECK(ReplayShot(hard.Evaluation.Record,replay,&error));CHECK(replay.Serialize()==hard.Evaluation.Record.Serialize());
    std::cout<<"PASS deterministic difficulty weights choose explainably different easy/hard shots: easy="<<easy.Index<<" hard="<<hard.Index<<'\n';
}
