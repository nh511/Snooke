#include <Snooker/Data/CoreConfig.h>
#include <Snooker/Physics/PhysicsSnapshot.h>
#include <Snooker/Physics/ClothFriction.h>
#include <Snooker/Physics/Sleep.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <locale>
#include <stdexcept>

namespace
{
std::string ReadFile(const char* Path)
{
    std::ifstream File(Path);
    if (!File) throw std::runtime_error(std::string("cannot read ")+Path);
    return {std::istreambuf_iterator<char>(File),{}};
}
}
int main(int argc,char** argv)
{
    using namespace Snooker;
    try {
        if(argc!=5) throw std::runtime_error("usage: snooker_single_ball <config> <snapshot> <trajectory.csv> <final.snapshot>");
        CoreConfig config; PhysicsSnapshot initial; std::string error;
        if(!CoreConfig::Deserialize(ReadFile(argv[1]),config,&error) ||
           !PhysicsSnapshot::Deserialize(ReadFile(argv[2]),initial,&error)) throw std::runtime_error(error);
        PhysicsClock clock; std::vector<BallState> balls;
        if(!initial.Restore(clock,balls,config.ParameterVersion,&error)) throw std::runtime_error(error);
        if(balls.size()!=1 || config.Balls.size()!=1 || balls[0].Id!=config.Balls[0].Id ||
           initial.FixedStepSeconds!=config.Solver.FixedStep) throw std::runtime_error("single-ball config/snapshot mismatch");
        auto& ball=balls.front(); const auto& definition=config.Balls.front();
        if(ball.RadiusMeters!=definition.RadiusMeters || ball.MassKg!=definition.MassKg ||
           ball.MaterialProfileId!=definition.MaterialProfileId) throw std::runtime_error("ball material/radius/mass mismatch");
        if(std::abs(ball.PositionMeters.Z-ball.RadiusMeters)>1e-9 || std::abs(ball.LinearVelocityMetersPerSecond.Z)>1e-9)
            throw std::runtime_error("only flat-cloth contact is implemented");
        const auto inside=[&] {
            return std::abs(ball.PositionMeters.X)+ball.RadiusMeters<=config.Table.Length/2 &&
                   std::abs(ball.PositionMeters.Y)+ball.RadiusMeters<=config.Table.Width/2;
        };
        if(!inside()) throw std::runtime_error("initial ball outside cloth");
        ClothProfile cloth{config.ParameterVersion,"loaded",config.Cloth.Sliding,config.Cloth.Rolling,config.Cloth.Spin,
            config.Solver.SleepLinearSpeed,config.Solver.SleepAngularSpeed,
            config.Solver.SleepLinearSpeed*config.Solver.FixedStep,config.Solver.SleepDuration};
        std::ofstream csv(argv[3]); csv.imbue(std::locale::classic());
        if(!csv) throw std::runtime_error("cannot write trajectory");
        csv<<std::setprecision(std::numeric_limits<double>::max_digits10)
           <<"step,time_s,id,x_m,y_m,z_m,vx_mps,vy_mps,vz_mps,wx_radps,wy_radps,wz_radps,sleeping,table_length_m,table_width_m,radius_m,parameter_version\n";
        const auto sample=[&] {
            csv<<clock.State().CompletedSteps<<','<<clock.SimulationTime()<<','<<ball.Id<<','
               <<ball.PositionMeters.ToString()<<','<<ball.LinearVelocityMetersPerSecond.ToString()<<','
               <<ball.AngularVelocityRadiansPerSecond.ToString()<<','<<(ball.Sleeping?1:0)<<','
               <<config.Table.Length<<','<<config.Table.Width<<','<<ball.RadiusMeters<<','<<config.ParameterVersion<<'\n';
            if(!csv) throw std::runtime_error("trajectory write failed");
        };
        // A snapshot can carry render-time debt; clear none of it, process one step at a time.
        bool ok=true; sample();
        const auto startStep=clock.State().CompletedSteps;
        while(!ball.Sleeping && (clock.State().CompletedSteps-startStep)*config.Solver.FixedStep<120) {
            const Seconds delta=std::max(0.0,config.Solver.FixedStep-clock.State().Accumulator);
            if(!clock.Advance(delta,[&](Seconds dt) {
                const auto previous=ball.PositionMeters;
                ok=ApplyAngularUpdate(ball,cloth,{},dt,&error) && ApplyRollingResistance(ball,cloth,{},dt,&error);
                if(!ok) return;
                ball.PositionMeters+=ball.LinearVelocityMetersPerSecond*dt;
                ok=UpdateSleep(ball,previous,cloth,dt,&error);
            },1) || !ok) throw std::runtime_error(error.empty()?"step failed":error);
            if(!inside()) throw std::runtime_error("cloth boundary reached; cushion collision not implemented");
            sample();
        }
        if(!ball.Sleeping) throw std::runtime_error("120 s simulation limit exceeded");
        csv.flush(); if(!csv) throw std::runtime_error("trajectory flush failed");
        std::ofstream final(argv[4]);
        if(!final || !(final<<PhysicsSnapshot::Capture(clock,balls,config.ParameterVersion,ESnapshotPhase::AfterShot).Serialize()))
            throw std::runtime_error("final snapshot write failed");
        final.flush(); if(!final) throw std::runtime_error("final snapshot flush failed");
        std::cout<<"PASS single-ball cloth motion settled; steps="<<clock.State().CompletedSteps<<" time="<<clock.SimulationTime()
                 <<"; no cue/collision/pocket simulation claimed\n";
    }
    catch(const std::exception& e) { std::cerr<<"FAIL "<<e.what()<<'\n'; return 1; }
}
