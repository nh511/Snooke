#include <Snooker/Service/ISnookerPhysicsService.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <stdexcept>

namespace
{
std::string Read(const char* Path)
{
    std::ifstream File(Path,std::ios::binary);if(!File)throw std::runtime_error("cannot read input");
    return {std::istreambuf_iterator<char>(File),{}};
}
}

int main(int argc,char** argv)
{
    using namespace Snooker;
    try {
        if (argc!=5) throw std::runtime_error("usage: snooker_performance bundle snapshot shot-input output-directory");
        PhysicsBundle Bundle;PhysicsSnapshot Initial;ShotInput Input;std::string Error;
        if (!PhysicsBundle::Deserialize(Read(argv[1]),Bundle,&Error) ||
            !PhysicsSnapshot::Deserialize(Read(argv[2]),Initial,&Error) ||
            !ShotInput::Deserialize(Read(argv[3]),Input,&Error)) throw std::runtime_error(Error);
        auto Service=CreatePhysicsService(Bundle,&Error);if(!Service || !Service->BeginShot(Initial,Input,&Error))throw std::runtime_error(Error);
        const std::filesystem::path Output=argv[4];
        if(std::filesystem::exists(Output) && !std::filesystem::is_empty(Output))throw std::runtime_error("output directory is not empty");
        std::filesystem::create_directories(Output);
        const auto WallStart=std::chrono::steady_clock::now();PhysicsRunState State=PhysicsRunState::Running;
        for(unsigned Frame=0;Frame<100000 && State==PhysicsRunState::Running;++Frame) {
            if(!Service->Step(Bundle.FixedStep,State,&Error))throw std::runtime_error(Error);
            if(State==PhysicsRunState::Faulted)throw std::runtime_error(Error.empty()?"physics fault":Error);
        }
        if(State!=PhysicsRunState::Settled)throw std::runtime_error("performance run did not settle");
        const auto WallNanoseconds=static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now()-WallStart).count());
        std::ofstream Csv(Output/"performance.csv",std::ios::binary);Csv.imbue(std::locale::classic());Csv<<std::setprecision(17)
          <<"sample_kind,step,time_s,elapsed_ns,search_ns,ball_ball_ns,cushion_ns,jaw_ns,pocket_contact_ns,mouth_entered_ns,recovered_to_cloth_ns,total_events,ball_ball,cushion,jaw,pocket_contact,mouth_entered,potted,recovered_to_cloth,ball_settled,shot_settled\n";
        PhysicsPerformanceSample Total;Total.Step=Service->SaveSnapshot().StepIndex;Total.Time=Service->SaveSnapshot().SimulationTimeSeconds;
        for(const auto& Sample:Service->GetPerformanceSamples()) {
            Csv<<"step,"<<Sample.Step<<','<<Sample.Time<<','<<Sample.ElapsedNanoseconds<<','<<Sample.SearchNanoseconds<<','
                <<Sample.BallBallNanoseconds<<','<<Sample.CushionNanoseconds<<','<<Sample.JawNanoseconds<<','
                <<Sample.PocketContactNanoseconds<<','<<Sample.MouthEnteredNanoseconds<<','<<Sample.RecoveredToClothNanoseconds<<','<<Sample.TotalEvents<<','
                <<Sample.BallBall<<','<<Sample.Cushion<<','<<Sample.Jaw<<','<<Sample.PocketContact<<','<<Sample.MouthEntered<<','
                <<Sample.Potted<<','<<Sample.RecoveredToCloth<<','<<Sample.BallSettled<<','<<Sample.ShotSettled<<'\n';
            Total.ElapsedNanoseconds+=Sample.ElapsedNanoseconds;Total.SearchNanoseconds+=Sample.SearchNanoseconds;
            Total.BallBallNanoseconds+=Sample.BallBallNanoseconds;Total.CushionNanoseconds+=Sample.CushionNanoseconds;
            Total.JawNanoseconds+=Sample.JawNanoseconds;Total.PocketContactNanoseconds+=Sample.PocketContactNanoseconds;
            Total.MouthEnteredNanoseconds+=Sample.MouthEnteredNanoseconds;Total.RecoveredToClothNanoseconds+=Sample.RecoveredToClothNanoseconds;
            Total.TotalEvents+=Sample.TotalEvents;Total.BallBall+=Sample.BallBall;
            Total.Cushion+=Sample.Cushion;Total.Jaw+=Sample.Jaw;Total.PocketContact+=Sample.PocketContact;Total.MouthEntered+=Sample.MouthEntered;
            Total.Potted+=Sample.Potted;Total.RecoveredToCloth+=Sample.RecoveredToCloth;Total.BallSettled+=Sample.BallSettled;Total.ShotSettled+=Sample.ShotSettled;
        }
        Csv<<"shot,"<<Total.Step<<','<<Total.Time<<','<<WallNanoseconds<<','<<Total.SearchNanoseconds<<','<<Total.BallBallNanoseconds<<','
           <<Total.CushionNanoseconds<<','<<Total.JawNanoseconds<<','<<Total.PocketContactNanoseconds<<','<<Total.MouthEnteredNanoseconds<<','
           <<Total.RecoveredToClothNanoseconds<<','<<Total.TotalEvents<<','<<Total.BallBall<<','<<Total.Cushion<<','<<Total.Jaw<<','
           <<Total.PocketContact<<','<<Total.MouthEntered<<','<<Total.Potted<<','<<Total.RecoveredToCloth<<','<<Total.BallSettled<<','<<Total.ShotSettled<<'\n';
        Csv.flush();if(!Csv)throw std::runtime_error("performance CSV flush failed");
        std::ofstream Summary(Output/"performance-summary.txt",std::ios::binary);
        Summary<<"schema=snooker-performance-v1\nparameter_version="<<Bundle.Version<<"\nfixed_steps="<<Service->GetPerformanceSamples().size()
                <<"\nwall_time_ns="<<WallNanoseconds<<"\nsolver_time_ns="<<Total.ElapsedNanoseconds
                <<"\nsearch_time_ns="<<Total.SearchNanoseconds<<"\nball_ball_time_ns="<<Total.BallBallNanoseconds
                <<"\ncushion_time_ns="<<Total.CushionNanoseconds<<"\njaw_time_ns="<<Total.JawNanoseconds
                <<"\npocket_contact_time_ns="<<Total.PocketContactNanoseconds<<"\nmouth_entered_time_ns="<<Total.MouthEnteredNanoseconds
                <<"\nrecovered_to_cloth_time_ns="<<Total.RecoveredToClothNanoseconds<<"\nevents="<<Total.TotalEvents<<"\n";
        std::cout<<"PASS performance samples="<<Service->GetPerformanceSamples().size()<<" events="<<Total.TotalEvents<<'\n';return 0;
    } catch(const std::exception& Exception) {std::cerr<<"FAIL "<<Exception.what()<<'\n';return 1;}
}
