#include <Snooker/Service/ISnookerPhysicsService.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#endif

std::string Read(const char* path)
{
    std::ifstream file(path,std::ios::binary);
    if(!file)throw std::runtime_error("cannot read bundle");
    return {std::istreambuf_iterator<char>(file),{}};
}
std::uint64_t Memory()
{
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX info{};info.cb=sizeof(info);
    if(GetProcessMemoryInfo(GetCurrentProcess(),reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&info),sizeof(info)))return info.WorkingSetSize;
#endif
    return 0;
}
int main(int argc,char** argv)
{
    using namespace Snooker;
    try {
        if(argc!=5)throw std::runtime_error("usage: snooker_stability bundle shots seed report");
        PhysicsBundle bundle;std::string error;
        if(!PhysicsBundle::Deserialize(Read(argv[1]),bundle,&error))throw std::runtime_error(error);
        const unsigned shots=std::stoul(argv[2]),seed=std::stoul(argv[3]);
        if(!shots||shots>100000)throw std::runtime_error("invalid shot count");
        std::mt19937 rng(seed);std::uniform_real_distribution<double> speed(.05,.4),angle(-Pi,Pi);
        std::uint64_t hash=1469598103934665603ULL,memoryStart=0,maxSteps=0,maxEvents=0;
        const auto started=std::chrono::steady_clock::now();
        for(unsigned n=0;n<shots;++n) {
            BallState ball;ball.Id=1;ball.PositionMeters={(n%10==9)?1.65:0,0,ball.RadiusMeters};
            PhysicsClock clock;auto snapshot=PhysicsSnapshot::Capture(clock,{ball},bundle.Version,ESnapshotPhase::BeforeShot);
            ShotInput input;input.CueBallId=1;const double a=n%2?angle(rng):0;
            input.CueDirection={std::cos(a),std::sin(a),0};input.ForwardSpeedMetersPerSecond=n%2?speed(rng):.2;
            auto service=CreatePhysicsService(bundle,&error);
            if(!service||!service->BeginShot(snapshot,input,&error))throw std::runtime_error(error);
            PhysicsRunState state=PhysicsRunState::Running;
            for(unsigned frame=0;frame<100000&&state==PhysicsRunState::Running;++frame)
                if(!service->Step(1.0/60,state,&error))throw std::runtime_error(error);
            if(state!=PhysicsRunState::Settled||service->GetFault().Code!=PhysicsFaultCode::None)
                throw std::runtime_error("stability shot faulted or did not settle");
            const auto final=service->SaveSnapshot();maxSteps=std::max(maxSteps,final.StepIndex);
            maxEvents=std::max<std::uint64_t>(maxEvents,service->GetEvents().size());
            for(unsigned char c:final.Serialize()){hash^=c;hash*=1099511628211ULL;}
            if(n==9||(shots<10&&n==0))memoryStart=Memory();
        }
        const std::uint64_t memoryEnd=Memory();
        const std::uint64_t growth=memoryEnd>=memoryStart?memoryEnd-memoryStart:0;
        const auto elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-started).count();
        if(memoryStart&&growth>10*1024*1024)throw std::runtime_error("working set growth exceeds 10 MiB");
        std::ofstream report(argv[4],std::ios::binary);
        report<<"{\"schema\":\"snooker-stability-v1\",\"shots\":"<<shots<<",\"seed\":"<<seed
              <<",\"hash\":"<<hash<<",\"elapsedMs\":"<<elapsed<<",\"memoryStart\":"<<memoryStart
              <<",\"memoryEnd\":"<<memoryEnd<<",\"memoryGrowth\":"<<growth<<",\"maxSteps\":"<<maxSteps
              <<",\"maxEvents\":"<<maxEvents<<",\"faults\":0}\n";
        report.flush();if(!report)throw std::runtime_error("report write failed");
        std::cout<<"PASS stability shots="<<shots<<" growth="<<growth<<" hash="<<hash<<'\n';return 0;
    } catch(const std::exception& error) {std::cerr<<"FAIL "<<error.what()<<'\n';return 1;}
}
