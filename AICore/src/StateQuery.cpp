#include <Snooker/AI/StateQuery.h>
#include <algorithm>
#include <set>

namespace Snooker
{
namespace
{
bool Fail(std::string* e,const char* m){if(e)*e=m;return false;}
std::uint64_t Hash(const std::string& text){std::uint64_t h=1469598103934665603ULL;for(unsigned char c:text){h^=c;h*=1099511628211ULL;}return h;}
class Query final:public IAIStateQuery
{
public:
    MatchState M;PhysicsSnapshot P;std::vector<BallDefinition> D;CueProfile C;BallOnQuery O;std::uint64_t H=0;
    const MatchState& Match()const override{return M;}const PhysicsSnapshot& Physics()const override{return P;}
    const std::vector<BallDefinition>& Definitions()const override{return D;}const CueProfile& Cue()const override{return C;}
    const BallOnQuery& BallOn()const override{return O;}std::uint64_t StateHash()const override{return H;}
    const BallState* FindBall(BallId id)const override{auto i=std::find_if(P.Balls.begin(),P.Balls.end(),[&](const auto& b){return b.Id==id;});return i==P.Balls.end()?nullptr:&*i;}
};
}
std::unique_ptr<IAIStateQuery> CreateAIStateQuery(const MatchState& match,const PhysicsSnapshot& physics,const std::vector<BallDefinition>& defs,const CueProfile& cue,std::string* error)
{
    if(!match.IsValid(error)||!physics.IsValid(error)||!cue.IsValid(error)||defs.size()!=physics.Balls.size())return nullptr;std::set<BallId> ids;unsigned cues=0;
    for(const auto& d:defs){if(!d.IsValid(error)||!ids.insert(d.Id).second)return nullptr;if(d.Type==EBallType::Cue)++cues;auto b=std::find_if(physics.Balls.begin(),physics.Balls.end(),[&](const auto& x){return x.Id==d.Id;});if(b==physics.Balls.end()||b->RadiusMeters!=d.RadiusMeters||b->MassKg!=d.MassKg||b->MaterialProfileId!=d.MaterialProfileId){Fail(error,"AI definition/physics mismatch");return nullptr;}}
    if(cues!=1){Fail(error,"AI query requires one cue ball");return nullptr;}auto q=std::make_unique<Query>();q->M=match;q->P=physics;q->D=defs;q->C=cue;if(!GetBallOn(match,q->O,error))return nullptr;std::string bytes=match.Serialize()+"\n"+physics.Serialize()+"\n"+cue.Serialize();for(const auto& d:defs)bytes+="\n"+d.Serialize();q->H=Hash(bytes);return q;
}
}
