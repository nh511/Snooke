#include <Snooker/Physics/PocketContact.h>
#include <Snooker/Physics/BallResponse.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>

#define CHECK(x) do {if(!(x)){std::cerr<<"FAIL pocket contact "<<__LINE__<<": "<<#x<<'\n';return 1;}}while(false)
std::string Read(const char* path){std::ifstream f(path);return {std::istreambuf_iterator<char>(f),{}};}
Snooker::PocketGeometry Fixture()
{
    using namespace Snooker;
    PocketGeometry p;p.Parameters={"test","pocket-contact-test-v1",0.1,0.02,0.5,0.1,0.4,0.08,0.6,0.01};
    p.Definition={1,{0,0,0},0.1,0.1,"test"};p.Inward={1,0,0};p.Tangent={0,1,0};
    p.MouthStart={0,-0.05,0};p.MouthEnd={0,0.05,0};p.BottomCenter={-0.3,0,-0.1};
    p.LeftJaw={{0,-0.05,0},{-0.01,-0.04,0}};p.RightJaw={{0,0.05,0},{-0.01,0.04,0}};
    p.Floor={{0,-0.05,0},{0,0.05,0},{-0.3,0.05,-0.1},{-0.3,-0.05,-0.1}};
    p.Bottom={{-0.3,-0.05,-0.1},{-0.3,0.05,-0.1},{-0.34,0.05,-0.1},{-0.34,-0.05,-0.1}};
    return p;
}
int main(int argc,char** argv)
{
    using namespace Snooker;CHECK(argc==4);TableDefinition table;CushionProfile cushion;PocketProfile material;TableGeometry g;
    CHECK(TableDefinition::Deserialize(Read(argv[1]),table));CHECK(CushionProfile::Deserialize(Read(argv[2]),cushion));
    CHECK(PocketProfile::Deserialize(Read(argv[3]),material));CHECK(GenerateTableGeometry(table,{cushion},{material},{},g));
    CHECK(g.Pockets[0].Bottom.size()==4);
    auto p=Fixture();BallState ball;ball.Id=1;ball.Sleeping=false;ball.RadiusMeters=0.01;
    ball.PositionMeters={0,0.15,0.005};ball.LinearVelocityMetersPerSecond={0,-100,0};
    PocketContact contact;std::string error;
    CHECK(FindPocketContact(ball,p,0.002,contact,&error));CHECK(contact.Hit);CHECK(contact.Feature==PocketFeature::Jaw);
    const double expected=(0.1-std::sqrt(0.01*0.01-0.005*0.005))/100;
    CHECK(NearlyEqual(contact.Time,expected,1e-12));CHECK(NearlyEqual(contact.Normal.Length(),1));
    auto interior=p;interior.RightJaw={{0,0.05,-0.02},{0,0.05,0.02}};
    PocketContact segmentHit;CHECK(FindPocketContact(ball,interior,0.002,segmentHit));
    CHECK(segmentHit.Hit && segmentHit.Feature==PocketFeature::Jaw);CHECK(NearlyEqual(segmentHit.Time,0.0009,1e-12));
    ball.PositionMeters+=ball.LinearVelocityMetersPerSecond*contact.Time;contact.Time=0;
    const auto energy=BallKineticEnergy(ball);
    CHECK(ReflectPocketContact(ball,p,contact,&error));CHECK(ball.LinearVelocityMetersPerSecond.Y>0);
    CHECK(BallKineticEnergy(ball)<=energy+1e-9);
    auto after=ball.Serialize();CHECK(ReflectPocketContact(ball,p,contact));CHECK(ball.Serialize()==after);
    ball.PositionMeters={-0.32,0,0.1};ball.LinearVelocityMetersPerSecond={0,0,-100};
    CHECK(FindPocketContact(ball,p,0.003,contact));CHECK(contact.Hit);CHECK(contact.Feature==PocketFeature::Bottom);
    CHECK(NearlyEqual(contact.Time,0.0019,1e-12));CHECK(contact.Normal.NearlyEquals({0,0,1}));
    ball.PositionMeters+=ball.LinearVelocityMetersPerSecond*contact.Time;contact.Time=0;
    CHECK(ReflectPocketContact(ball,p,contact));CHECK(NearlyEqual(ball.LinearVelocityMetersPerSecond.Z,0));
    ball.PositionMeters={-0.15,0,0.1};ball.LinearVelocityMetersPerSecond={0,0,-10};
    CHECK(FindPocketContact(ball,p,0.03,contact));CHECK(contact.Hit);CHECK(contact.Feature==PocketFeature::Slope);
    const auto center=ball.PositionMeters+ball.LinearVelocityMetersPerSecond*contact.Time;
    CHECK(NearlyEqual((center-contact.Point).Length(),ball.RadiusMeters,1e-10));
    ball.PositionMeters={-0.32,0.2,0.1};CHECK(FindPocketContact(ball,p,0.1,contact));CHECK(!contact.Hit);
    ball.PositionMeters={0.2,0,0.1};ball.LinearVelocityMetersPerSecond={1,0,0};
    CHECK(FindPocketContact(ball,p,1,contact));CHECK(!contact.Hit);
    const auto saved=contact;CHECK(!FindPocketContact(ball,p,-1,contact));CHECK(contact.Hit==saved.Hit && contact.Time==saved.Time);
    auto broken=p;broken.Floor[0].X=std::numeric_limits<double>::infinity();CHECK(!FindPocketContact(ball,broken,1,contact));
    broken=p;broken.Bottom.clear();CHECK(!FindPocketContact(ball,broken,1,contact));
    broken=p;broken.Parameters.Id="wrong";CHECK(!FindPocketContact(ball,broken,1,contact));
    ball.PositionMeters={0,0.05,0};CHECK(FindPocketContact(ball,p,0,contact));CHECK(contact.InitiallyOverlapping);
    after=ball.Serialize();CHECK(!ReflectPocketContact(ball,p,contact));CHECK(ball.Serialize()==after);
    // Actual generated pocket bottom supports a finite sphere, not an infinite plane.
    const auto& actual=g.Pockets[0];
    ball.PositionMeters=actual.BottomCenter-actual.Inward*material.JawRadius+Vec3d{0,0,0.1};
    ball.LinearVelocityMetersPerSecond={0,0,-1};
    CHECK(FindPocketContact(ball,actual,0.2,contact));CHECK(contact.Hit);CHECK(contact.Feature==PocketFeature::Bottom);
    for(int speed:{1,10,100,1000}) {
        ball.PositionMeters={-0.32,0,0.1};ball.LinearVelocityMetersPerSecond={0,0,-double(speed)};
        CHECK(FindPocketContact(ball,p,0.3/speed,contact));CHECK(contact.Hit);
        CHECK(NearlyEqual(contact.Time,0.19/speed,1e-12));
    }
    std::cout<<"PASS continuous finite pocket jaw/edge/slope/bottom contacts, rebound, high speed and invalid inputs\n";
}
