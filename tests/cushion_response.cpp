#include <Snooker/Physics/CushionResponse.h>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <iterator>

#define CHECK(x) do { if(!(x)) { std::cerr<<"FAIL cushion response line "<<__LINE__<<": "<<#x<<'\n'; return 1; } } while(false)
std::string Read(const char* path) {std::ifstream f(path);return {std::istreambuf_iterator<char>(f),{}};}
bool Face(const Snooker::BallState& b,const Snooker::TableGeometry& g,Snooker::CushionContact& output)
{
    std::vector<Snooker::CushionContact> cs;
    if(!Snooker::FindCushionContacts(b,g,1e-7,cs))return false;
    for(const auto& c:cs)if(c.Feature==Snooker::TableFeature::CushionFace){output=c;return true;}
    return false;
}
int main(int argc,char** argv)
{
    using namespace Snooker;CHECK(argc==4);
    TableDefinition table;CushionProfile profile;PocketProfile pocket;TableGeometry g;
    CHECK(TableDefinition::Deserialize(Read(argv[1]),table));CHECK(CushionProfile::Deserialize(Read(argv[2]),profile));
    CHECK(PocketProfile::Deserialize(Read(argv[3]),pocket));CHECK(GenerateTableGeometry(table,{profile},{pocket},{},g));
    BallState ball;ball.Id=1;ball.Sleeping=false;
    ball.PositionMeters={0,table.Cloth.Min.Y+ball.RadiusMeters,profile.ContactHeight};ball.LinearVelocityMetersPerSecond={0,-1,0};
    CushionContact contact;CHECK(Face(ball,g,contact));CushionResponse result;
    CHECK(ResolveCushionContact(ball,g,contact,1e-7,result));CHECK(result.Applied);
    CHECK(NearlyEqual(ball.LinearVelocityMetersPerSecond.Y,profile.Restitution));
    CHECK(ball.AngularVelocityRadiansPerSecond.Length()==0);CHECK(result.EnergyAfter<=result.EnergyBefore);
    // Two analytically timed flights between finite parallel rails; save real pre/post states.
    std::ofstream log("cushion-two-rail.txt");CHECK(bool(log));
    log<<std::quoted(profile.Serialize())<<'\n';
    ball.LinearVelocityMetersPerSecond={0,-1,0};
    for(int event=0;event<2;++event) {
        if(event) {
            const double target=table.Cloth.Max.Y-ball.RadiusMeters;
            const double time=(target-ball.PositionMeters.Y)/ball.LinearVelocityMetersPerSecond.Y;
            CHECK(time>0);ball.PositionMeters+=ball.LinearVelocityMetersPerSecond*time;
        }
        CHECK(Face(ball,g,contact));const auto before=ball.Serialize();
        CHECK(ResolveCushionContact(ball,g,contact,1e-7,result));CHECK(result.Applied);
        log<<std::quoted(before)<<' '<<std::quoted(ball.Serialize())<<'\n';
    }
    log.close();CHECK(!log.fail());
    std::ifstream saved("cushion-two-rail.txt");std::string text,before,after;CHECK(bool(saved>>std::quoted(text)));
    CushionProfile loaded;CHECK(CushionProfile::Deserialize(text,loaded));
    TableGeometry restored;CHECK(GenerateTableGeometry(table,{loaded},{pocket},{},restored));
    for(int event=0;event<2;++event) {
        CHECK(bool(saved>>std::quoted(before)>>std::quoted(after)));BallState replay;
        CHECK(BallState::Deserialize(before,replay));CHECK(Face(replay,restored,contact));
        CHECK(ResolveCushionContact(replay,restored,contact,1e-7,result));CHECK(replay.Serialize()==after);
    }
    for(double spin:{-20.0,20.0}) {
        ball=BallState{};ball.Id=1;ball.Sleeping=false;
        ball.PositionMeters={0,table.Cloth.Min.Y+ball.RadiusMeters,profile.ContactHeight};
        ball.LinearVelocityMetersPerSecond={0,-1,0};ball.AngularVelocityRadiansPerSecond={0,0,spin};
        CHECK(Face(ball,g,contact));CHECK(ResolveCushionContact(ball,g,contact,1e-7,result));
        CHECK(result.Applied);CHECK(ball.LinearVelocityMetersPerSecond.X*spin<0);
        const double contactSlip=spin*ball.RadiusMeters;
        const double expectedTangential=-std::copysign(std::min(std::abs(contactSlip)*ball.MassKg/3.5,
            profile.TangentialFriction*(1+profile.Restitution)*ball.MassKg),spin);
        CHECK(NearlyEqual(result.TangentialImpulse.X,expectedTangential,1e-12));
        CHECK(std::abs(ball.AngularVelocityRadiansPerSecond.Z)<std::abs(spin));CHECK(result.EnergyAfter<=result.EnergyBefore+1e-12);
        const auto record=ball.Serialize();CHECK(ResolveCushionContact(ball,g,contact,1e-7,result));CHECK(!result.Applied);CHECK(ball.Serialize()==record);
    }
    const auto original=ball.Serialize();result.NormalImpulse=77;
    auto invalid=contact;invalid.Normal={0,0,1};CHECK(!ResolveCushionContact(ball,g,invalid,1e-7,result));
    CHECK(ball.Serialize()==original);CHECK(result.NormalImpulse==77);
    auto stale=g;stale.CushionMaterials[0].ParameterVersion="changed";
    CHECK(!ResolveCushionContact(ball,stale,contact,1e-7,result));CHECK(ball.Serialize()==original);
    std::cout<<"PASS cushion normal/spin/friction/energy, real one/two-rail saved response reconstruction and stale rejection\n";
}
