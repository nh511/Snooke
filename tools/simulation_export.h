#pragma once
#include <Snooker/Replay/PhysicsShotRecord.h>
#include <Snooker/Validation/DebugOptions.h>
#include <iomanip>
#include <ostream>

namespace SnookerExport
{
inline std::string Quote(const std::string& text)
{
    std::ostringstream s;s<<char(34);
    for(unsigned char c:text) {
        if(c==34 || c==92)s<<char(92)<<char(c);
        else if(c<32)s<<char(92)<<"u00"<<std::hex<<std::setw(2)<<std::setfill('0')<<unsigned(c)<<std::dec;
        else s<<char(c);
    }
    s<<char(34);return s.str();
}
inline void Vector(std::ostream& s,const Snooker::Vec3d& v){s<<'['<<v.X<<','<<v.Y<<','<<v.Z<<']';}
inline void Snapshot(std::ostream& s,const Snooker::PhysicsSnapshot& snapshot)
{
    s<<"{\"step\":"<<snapshot.StepIndex<<",\"time\":"<<snapshot.SimulationTimeSeconds<<",\"eventCount\":"<<snapshot.EventCount
     <<",\"serialized\":"<<Quote(snapshot.Serialize())<<",\"balls\":[";
    for(std::size_t i=0;i<snapshot.Balls.size();++i) {
        if(i)s<<',';const auto& b=snapshot.Balls[i];
        const auto p=snapshot.PocketStates.empty()?Snooker::PocketStateData{}:snapshot.PocketStates[i];
        s<<"{\"id\":"<<b.Id<<",\"position\":";Vector(s,b.PositionMeters);
        s<<",\"velocity\":";Vector(s,b.LinearVelocityMetersPerSecond);
        s<<",\"angular\":";Vector(s,b.AngularVelocityRadiansPerSecond);
        s<<",\"radius\":"<<b.RadiusMeters<<",\"sleeping\":"<<(b.Sleeping?"true":"false")
         <<",\"pocketState\":"<<unsigned(p.State)<<",\"pocketId\":"<<p.PocketId<<",\"potEmitted\":"<<(p.PotEventEmitted?"true":"false")<<'}';
    }
    s<<"]}";
}
inline std::string Result(const Snooker::PhysicsShotRecord& record,const Snooker::TableGeometry& g)
{
    std::ostringstream s;s.imbue(std::locale::classic());s<<std::setprecision(17);
    s<<"{\"schema\":\"snooker-result-v1\",\"build\":"<<Quote(record.BuildVersion)
     <<",\"parameterVersion\":"<<Quote(record.Parameters.Version)<<",\"bundle\":"<<Quote(record.Parameters.Serialize())
     <<",\"input\":"<<Quote(record.Input.Serialize())<<",\"result\":"<<Quote(record.Result)<<",\"initial\":";
    Snapshot(s,record.Initial);s<<",\"final\":";Snapshot(s,record.Final);
    s<<",\"events\":[";
    for(std::size_t i=0;i<record.Events.size();++i) {
        if(i)s<<',';const auto& e=record.Events[i];
        s<<"{\"sequence\":"<<e.Sequence<<",\"step\":"<<e.Step<<",\"time\":"<<e.Time<<",\"kind\":"<<unsigned(e.Kind)
         <<",\"ball\":"<<e.Ball<<",\"other\":"<<e.Other<<",\"point\":";Vector(s,e.Point);
        s<<",\"normal\":";Vector(s,e.Normal);s<<",\"impulse\":"<<e.Impulse<<'}';
    }
    s<<"],\"geometry\":{\"segments\":[";
    for(std::size_t i=0;i<g.Segments.size();++i) {
        if(i)s<<',';const auto& e=g.Segments[i];s<<"{\"kind\":"<<unsigned(e.Feature)<<",\"owner\":"<<e.OwnerId<<",\"start\":";
        Vector(s,e.Start);s<<",\"end\":";Vector(s,e.End);s<<'}';
    }
    s<<"],\"pockets\":[";
    for(std::size_t i=0;i<g.Pockets.size();++i) {
        if(i)s<<',';const auto& p=g.Pockets[i];s<<"{\"id\":"<<p.Definition.Id<<",\"center\":";
        Vector(s,p.Definition.Center);s<<",\"opening\":"<<p.Definition.Opening<<'}';
    }
    s<<"]}}\n";return s.str();
}
inline std::string Debug(const Snooker::DebugOptions& options)
{
    std::ostringstream s;s<<"{\"schema\":\"snooker-debug-v1\",\"mask\":"<<options.Mask<<",\"enabled\":[";
    bool first=true;
    for(const auto flag:{Snooker::DebugFlag::Trajectory,Snooker::DebugFlag::CollisionPoints,
                         Snooker::DebugFlag::ContactNormals,Snooker::DebugFlag::SleepingState,
                         Snooker::DebugFlag::PhysicsStepCount}) if(options.Enabled(flag)) {
        if(!first)s<<',';s<<Quote(options.Name(flag));first=false;
    }
    s<<"],\"serialized\":"<<Quote(options.Serialize())<<"}\n";return s.str();
}
inline void CsvSample(std::ostream& s,const Snooker::PhysicsSnapshot& snapshot)
{
    for(std::size_t i=0;i<snapshot.Balls.size();++i) {
        const auto& b=snapshot.Balls[i];const auto p=snapshot.PocketStates.empty()?Snooker::PocketStateData{}:snapshot.PocketStates[i];
        s<<snapshot.StepIndex<<','<<snapshot.SimulationTimeSeconds<<','<<b.Id<<','<<b.PositionMeters.ToString()<<','
         <<b.LinearVelocityMetersPerSecond.ToString()<<','<<b.AngularVelocityRadiansPerSecond.ToString()<<','<<unsigned(b.Sleeping)
         <<','<<unsigned(p.State)<<','<<p.PocketId<<','<<unsigned(p.PotEventEmitted)<<','<<snapshot.EventCount<<'\n';
    }
    if(!s)throw std::runtime_error("trajectory write failed");
}
}
