#include <Snooker/Shot/CueImpulse.h>
#include <Snooker/Physics/ClothFriction.h>
#include <Snooker/Physics/Sleep.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>

#define CHECK(x) do { if(!(x)) { std::cerr<<"FAIL impulse line "<<__LINE__<<": "<<#x<<'\n'; return 1; } } while(false)
double Energy(const Snooker::BallState& b)
{
    return 0.5*b.MassKg*b.LinearVelocityMetersPerSecond.LengthSquared()+
           0.2*b.MassKg*b.RadiusMeters*b.RadiusMeters*b.AngularVelocityRadiansPerSecond.LengthSquared();
}
int main()
{
    using namespace Snooker;
    BallState initial; initial.Id=1; initial.PositionMeters={0,0,initial.RadiusMeters};
    CueProfile cue;
    ShotInput input; input.CueBallId=1; input.CueDirection={3,0,0}; input.PullbackDistanceMeters=0.1; input.ForwardSpeedMetersPerSecond=2;
    auto center=initial; CueImpulseResult result;
    CHECK(ApplyCueImpulse(center,input,cue,result));
    const double expected=(1+cue.TipRestitution)*2/(1/initial.MassKg+1/cue.MassKg);
    CHECK(NearlyEqual(result.ImpulseNewtonSeconds,expected));
    CHECK(NearlyEqual(center.LinearVelocityMetersPerSecond.X,expected/initial.MassKg));
    CHECK(center.AngularVelocityRadiansPerSecond.Length()==0); CHECK(!center.Sleeping); CHECK(center.SleepTimeSeconds==0);
    const double cueAfter=2-expected/cue.MassKg;
    CHECK(Energy(center)+0.5*cue.MassKg*cueAfter*cueAfter<=0.5*cue.MassKg*4+1e-12);
    CHECK(NearlyEqual(initial.MassKg*center.LinearVelocityMetersPerSecond.X+cue.MassKg*cueAfter,2*cue.MassKg));
    for(const Vec3d strike: {Vec3d{0,0,0.3},Vec3d{0,0,-0.3},Vec3d{0,0.3,0},Vec3d{0,-0.3,0}}) {
        auto shot=input; shot.StrikePointOnBall=strike; shot.CueTipOffsetMeters=strike*initial.RadiusMeters;
        auto ball=initial; CueImpulseResult hit; CHECK(ApplyCueImpulse(ball,shot,cue,hit));
        CHECK((hit.ContactPointMeters-ball.PositionMeters).Length()>initial.RadiusMeters-1e-12);
        if(strike.Z!=0) CHECK(ball.AngularVelocityRadiansPerSecond.Y*strike.Z>0);
        if(strike.Y!=0) CHECK(ball.AngularVelocityRadiansPerSecond.Z*strike.Y<0);
        CHECK(ball.LinearVelocityMetersPerSecond.X<center.LinearVelocityMetersPerSecond.X);
        const double after=2-hit.ImpulseNewtonSeconds/cue.MassKg;
        CHECK(Energy(ball)+0.5*cue.MassKg*after*after<=0.5*cue.MassKg*4+1e-12);
    }
    auto limited=cue; limited.MaxImpulseNewtonSeconds=0.01; auto capped=initial;
    CHECK(ApplyCueImpulse(capped,input,limited,result)); CHECK(result.ImpulseNewtonSeconds==0.01);
    auto stronger=input; stronger.ForwardSpeedMetersPerSecond=3; auto strongBall=initial;
    CHECK(ApplyCueImpulse(strongBall,stronger,cue,result)); CHECK(strongBall.LinearVelocityMetersPerSecond.X>center.LinearVelocityMetersPerSecond.X);
    auto timed=input; timed.InputTimeSeconds=123; timed.PullbackDistanceMeters=0.05; auto timedBall=initial;
    CHECK(ApplyCueImpulse(timedBall,timed,cue,result)); CHECK(timedBall.Serialize()==center.Serialize());
    auto zero=input; zero.ForwardSpeedMetersPerSecond=0; auto zeroBall=initial;
    CHECK(ApplyCueImpulse(zeroBall,zero,cue,result)); CHECK(zeroBall.Serialize()==initial.Serialize());
    auto rotated=input; rotated.CueDirection={0,1,0}; auto rotatedBall=initial;
    CHECK(ApplyCueImpulse(rotatedBall,rotated,cue,result)); CHECK(NearlyEqual(rotatedBall.LinearVelocityMetersPerSecond.Y,center.LinearVelocityMetersPerSecond.X));
    CHECK(rotatedBall.AngularVelocityRadiansPerSecond.Length()==0);
    const auto before=center.Serialize(); result.ImpulseNewtonSeconds=77;
    CHECK(!ApplyCueImpulse(center,input,cue,result)); CHECK(center.Serialize()==before); CHECK(result.ImpulseNewtonSeconds==77);
    auto invalid=input; invalid.CueTipOffsetMeters={1,0,0}; auto rejected=initial;
    CHECK(!ApplyCueImpulse(rejected,invalid,cue,result)); CHECK(rejected.Serialize()==initial.Serialize()); CHECK(result.ImpulseNewtonSeconds==77);
    {std::ofstream f("repeat.input"); CHECK(bool(f)); CHECK(bool(f<<input.Serialize()));}
    std::ifstream f("repeat.input"); CHECK(bool(f));
    const std::string text((std::istreambuf_iterator<char>(f)),{}); ShotInput restored;
    CHECK(ShotInput::Deserialize(text,restored));
    auto fileBall=initial; CHECK(ApplyCueImpulse(fileBall,restored,cue,result)); CHECK(fileBall.Serialize()==center.Serialize());
    ClothProfile cloth{"impulse-cloth-v1","dry",0.2,0.01,2,0.001,0.01,0.0001,0.25};
    for(int i=0;i<12000;++i) {
        for(auto* b:{&fileBall,&center}) {
            const auto p=b->PositionMeters;
            CHECK(ApplyAngularUpdate(*b,cloth,{},1.0/240)); CHECK(ApplyRollingResistance(*b,cloth,{},1.0/240));
            b->PositionMeters+=b->LinearVelocityMetersPerSecond*(1.0/240);
            CHECK(UpdateSleep(*b,p,cloth,1.0/240));
        }
    }
    CHECK(fileBall.Sleeping); CHECK(fileBall.Serialize()==center.Serialize());
    std::cout<<"PASS analytic impulse/momentum/energy, center/follow/draw/side signs, cap and repeated file simulation\n";
}
