#include <Snooker/Physics/BallPairs.h>
#include <algorithm>
#include <array>

namespace Snooker
{
bool FindBallPairCandidates(const std::vector<BallState>& Balls,Seconds Horizon,std::vector<BallPair>& Output,std::string* Error)
{
    const auto fail=[&](const char* message){if(Error)*Error=message;return false;};
    if(!IsFinite(Horizon) || Horizon<0 || Balls.size()>4096) return fail("invalid broad-phase horizon or resource count");
    struct Box { BallId Id; std::array<Scalar,3> Min,Max; };
    std::vector<Box> boxes; boxes.reserve(Balls.size());
    for(const auto& ball:Balls) {
        if(!ball.IsValid(Error)) return false;
        const auto end=ball.PositionMeters+ball.LinearVelocityMetersPerSecond*Horizon;
        if(!end.IsFinite()) return fail("nonfinite swept ball endpoint");
        const std::array<Scalar,3> p{ball.PositionMeters.X,ball.PositionMeters.Y,ball.PositionMeters.Z},q{end.X,end.Y,end.Z};
        Box b; b.Id=ball.Id;
        for(int axis=0;axis<3;++axis) {
            b.Min[axis]=std::min(p[axis],q[axis])-ball.RadiusMeters;
            b.Max[axis]=std::max(p[axis],q[axis])+ball.RadiusMeters;
            if(!IsFinite(b.Min[axis]) || !IsFinite(b.Max[axis])) return fail("swept bounds overflow");
        }
        boxes.push_back(b);
    }
    std::sort(boxes.begin(),boxes.end(),[](const Box& a,const Box& b){return a.Id<b.Id;});
    for(std::size_t i=1;i<boxes.size();++i) if(boxes[i].Id==boxes[i-1].Id) return fail("duplicate ball id in broad phase");
    std::vector<BallPair> candidates;
    for(std::size_t i=0;i<boxes.size();++i) for(std::size_t j=i+1;j<boxes.size();++j) {
        bool overlap=true;
        for(int axis=0;axis<3;++axis)
            if(boxes[i].Max[axis]<boxes[j].Min[axis] || boxes[j].Max[axis]<boxes[i].Min[axis]) {overlap=false;break;}
        if(overlap) candidates.push_back({boxes[i].Id,boxes[j].Id});
    }
    Output.swap(candidates);return true;
}
}
