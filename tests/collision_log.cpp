#include <Snooker/Replay/CollisionLog.h>
#include <Snooker/Physics/BallContact.h>
#include <fstream>
#include <iostream>
#include <iterator>

#define CHECK(x) do { if(!(x)) { std::cerr<<"FAIL collision log line "<<__LINE__<<": "<<#x<<'\n'; return 1; } } while(false)
int main()
{
    using namespace Snooker;
    BallState a,b,c;a.Id=1;b.Id=2;c.Id=3;
    a.PositionMeters={-0.2,0,0};b.PositionMeters={0,0,0};c.PositionMeters={0.2,0,0};
    a.Sleeping=false;a.LinearVelocityMetersPerSecond={1,0,0};
    CollisionLog log;log.Parameters={"event-test-v1",1,0,1e-7};
    Seconds elapsed=0;StepIndex step=0;
    for(auto pair:{std::pair<BallState*,BallState*>{&a,&b},{&b,&c}}) {
        BallContactTime contact;CHECK(FindBallContactTime(*pair.first,*pair.second,1,contact));CHECK(contact.Hit);
        for(auto* ball:{&a,&b,&c})ball->PositionMeters+=ball->LinearVelocityMetersPerSecond*contact.Time;
        elapsed+=contact.Time;++step;
        CHECK(log.ResolveAndAppend(*pair.first,*pair.second,step,elapsed));
    }
    CHECK(log.Events.size()==2);CHECK(log.Events[0].FirstBefore.Id==1);CHECK(log.Events[1].SecondBefore.Id==3);
    CHECK(log.Events[0].Time<log.Events[1].Time);CHECK(log.IsValid());
    CHECK(NearlyEqual(c.LinearVelocityMetersPerSecond.X,1));
    CHECK(log.ResolveAndAppend(b,c,step,elapsed));CHECK(log.Events.size()==2);
    {std::ofstream file("collisions.txt");CHECK(bool(file));CHECK(bool(file<<log.Serialize()));}
    std::ifstream file("collisions.txt");CHECK(bool(file));const std::string text((std::istreambuf_iterator<char>(file)),{});
    CollisionLog loaded;CHECK(CollisionLog::Deserialize(text,loaded));CHECK(loaded.Serialize()==log.Serialize());
    const auto saved=loaded.Serialize();std::string error;
    for(const auto& bad:{text+"junk",text.substr(0,text.find("end")),std::string("snooker-collisions-v2")}) {
        CHECK(!CollisionLog::Deserialize(bad,loaded,&error));CHECK(!error.empty());CHECK(loaded.Serialize()==saved);
    }
    auto corrupt=log;corrupt.Events[0].FirstAfter.LinearVelocityMetersPerSecond.X=99;CHECK(!corrupt.IsValid());
    CHECK(!CollisionLog::Deserialize(corrupt.Serialize(),loaded));CHECK(loaded.Serialize()==saved);
    corrupt=log;std::swap(corrupt.Events[0],corrupt.Events[1]);CHECK(!corrupt.IsValid());
    corrupt=log;corrupt.Events[0].Response.EnergyAfter+=1;CHECK(!corrupt.IsValid());
    const auto savedA=a.Serialize(),savedB=b.Serialize();
    CHECK(!log.ResolveAndAppend(a,b,0,0));CHECK(log.Serialize()==saved);CHECK(a.Serialize()==savedA && b.Serialize()==savedB);
    CHECK(!log.ResolveAndAppend(a,a,step,elapsed));CHECK(log.Serialize()==saved);
    std::cout<<"PASS actual sequential pair collisions, rich event persistence/order and response reconstruction\n";
}
