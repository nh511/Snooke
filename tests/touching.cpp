#include <Snooker/Physics/Touching.h>
#include <Snooker/Replay/CollisionLog.h>
#include <iostream>

#define CHECK(x) do { if(!(x)) { std::cerr<<"FAIL touching line "<<__LINE__<<": "<<#x<<'\n'; return 1; } } while(false)
int main()
{
    using namespace Snooker;
    BallState a,b;a.Id=1;b.Id=2;b.PositionMeters={a.RadiusMeters+b.RadiusMeters,0,0};
    const auto savedA=a.Serialize(),savedB=b.Serialize();TouchingResult result;
    CHECK(QueryTouching(a,b,{},result));CHECK(result.Relation==TouchingRelation::Touching);CHECK(result.Motion==RelativeNormalMotion::Resting);
    CollisionLog log;log.Parameters={"touching-v1",0.9,0.1,1e-7};
    for(int i=0;i<1000;++i) {CHECK(QueryTouching(a,b,{},result));CHECK(log.ResolveAndAppend(a,b,0,0));}
    CHECK(log.Events.empty());CHECK(a.Serialize()==savedA && b.Serialize()==savedB);
    a.LinearVelocityMetersPerSecond={-1,0,0};
    CHECK(QueryTouching(a,b,{},result));CHECK(result.Motion==RelativeNormalMotion::Separating);
    CHECK(log.ResolveAndAppend(a,b,0,0));CHECK(log.Events.empty());
    a.LinearVelocityMetersPerSecond={1,0,0};
    CHECK(QueryTouching(a,b,{},result));CHECK(result.Motion==RelativeNormalMotion::Approaching);
    CHECK(log.ResolveAndAppend(a,b,1,0.01));CHECK(log.Events.size()==1);
    CHECK(log.ResolveAndAppend(a,b,1,0.01));CHECK(log.Events.size()==1);
    b.PositionMeters.X+=1e-3;CHECK(QueryTouching(a,b,{},result));CHECK(result.Relation==TouchingRelation::Separated);
    b.PositionMeters.X=a.RadiusMeters+b.RadiusMeters-1e-3;
    CHECK(QueryTouching(a,b,{},result));CHECK(result.Relation==TouchingRelation::Overlapping);
    const auto gap=result.SignedGap;
    auto bad=TouchingThresholds{};bad.GapTolerance=-1;CHECK(!QueryTouching(a,b,bad,result));CHECK(result.SignedGap==gap);
    CHECK(!QueryTouching(a,a,{},result));CHECK(result.SignedGap==gap);
    b.PositionMeters=a.PositionMeters;CHECK(!QueryTouching(a,b,{},result));CHECK(result.SignedGap==gap);
    std::cout<<"PASS rule-facing touching states, separation and no spurious repeated collision events\n";
}
