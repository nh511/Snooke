#include <Snooker/Service/ISnookerPhysicsService.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <Snooker/Table/BallPlacement.h>

#define CHECK(x) do {if(!(x)){std::cerr<<"FAIL service "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
using namespace Snooker;
std::string Read(const char* path){std::ifstream f(path);return {std::istreambuf_iterator<char>(f),{}};}
BallState Ball(unsigned id,double x,double y=0)
{
    BallState b;b.Id=id;b.PositionMeters={x,y,b.RadiusMeters};return b;
}
int main(int argc,char** argv)
{
    std::string error;CHECK(argc==2);PhysicsBundle bundle;
    CHECK(PhysicsBundle::Deserialize(Read(argv[1]),bundle,&error));
    auto service=CreatePhysicsService(bundle,&error);CHECK(service!=nullptr);
    TableGeometry closed;CHECK(GenerateTableGeometry(bundle.Table,bundle.Cushions,bundle.Pockets,bundle.Geometry,closed,&error));
    CHECK(closed.ClosedBoundary.size()>100);CHECK(bundle.Table.Cushions.size()==6 && closed.Pockets.size()==6);
    auto missing=bundle;missing.Table.Cushions.pop_back();CHECK(!CreatePhysicsService(missing,&error));
    missing=bundle;missing.Geometry.RequireClosedBoundary=false;CHECK(!CreatePhysicsService(missing,&error));
    PhysicsClock clock;auto s=PhysicsSnapshot::Capture(clock,{Ball(1,-0.3),Ball(2,0),Ball(3,0.3)},bundle.Version,ESnapshotPhase::BeforeShot);
    ShotInput input;input.CueBallId=1;input.ForwardSpeedMetersPerSecond=0.5;
    CHECK(service->BeginShot(s,input,&error));
    PhysicsRunState run;for(int n=0;n<1200;++n) {
        CHECK(service->Step(1.0/60,run,&error));CHECK(run!=PhysicsRunState::Faulted);if(run==PhysicsRunState::Settled)break;
    }
    CHECK(service->IsSettled());unsigned hits=0;
    for(const auto& e:service->GetEvents())if(e.Kind==PhysicsEventKind::BallBall)++hits;
    CHECK(hits>=1);CHECK(service->GetEvents().back().Kind==PhysicsEventKind::ShotSettled);
    const auto final=service->SaveSnapshot().Serialize();const auto eventCount=service->GetEvents().size();
    std::string referenceEvents;for(const auto& e:service->GetEvents())referenceEvents+=e.Serialize()+"\n";
    CHECK(!service->Step(-1,run,&error));CHECK(service->SaveSnapshot().Serialize()==final);
    CHECK(service->Step(1,run,&error));CHECK(service->GetEvents().size()==eventCount);
    // Render scheduling changes neither the physics events nor final bytes.
    for(double fps:{30.,60.,120.,144.}) {
        CHECK(service->BeginShot(s,input,&error));
        for(int n=0;n<10000;++n){CHECK(service->Step(1/fps,run,&error));CHECK(run!=PhysicsRunState::Faulted);if(run==PhysicsRunState::Settled)break;}
        CHECK(service->SaveSnapshot().Serialize()==final);
        std::string events;for(const auto& e:service->GetEvents())events+=e.Serialize()+"\n";CHECK(events==referenceEvents);
    }
    CHECK(service->BeginShot(s,input,&error));
    for(unsigned n=0;n<10000;++n){CHECK(service->Step(n%3==0?0.01:n%3==1?0.005:0.019,run,&error));CHECK(run!=PhysicsRunState::Faulted);if(run==PhysicsRunState::Settled)break;}
    CHECK(service->SaveSnapshot().Serialize()==final);
    // Constant-velocity high-speed chain: more than one impact in one step.
    auto free=bundle;free.Cloth.Sliding=free.Cloth.Rolling=free.Cloth.Spin=0;free.BallCollision.Restitution=1;free.BallCollision.TangentialFriction=0;
    auto chain=CreatePhysicsService(free,&error);CHECK(chain);
    auto c=PhysicsSnapshot::Capture(clock,{Ball(1,-0.13),Ball(2,0),Ball(3,0.13)},free.Version,ESnapshotPhase::DuringStep);
    c.Balls[0].Sleeping=false;c.Balls[0].LinearVelocityMetersPerSecond={40,0,0};
    CHECK(chain->RestoreSnapshot(c,&error));CHECK(chain->Step(free.FixedStep,run,&error));CHECK(run!=PhysicsRunState::Faulted);
    CHECK(chain->GetEvents().size()==2);
    CHECK(chain->GetEvents()[0].Other==2 && chain->GetEvents()[1].Other==3);
    CHECK(chain->GetEvents()[0].Time<chain->GetEvents()[1].Time);
    auto checkpoint=chain->SaveSnapshot();PhysicsSnapshot loaded;CHECK(PhysicsSnapshot::Deserialize(checkpoint.Serialize(),loaded,&error));
    auto resumed=CreatePhysicsService(free,&error);CHECK(resumed->RestoreSnapshot(loaded,&error));
    CHECK(chain->Step(free.FixedStep,run,&error));CHECK(resumed->Step(free.FixedStep,run,&error));
    CHECK(chain->SaveSnapshot().Serialize()==resumed->SaveSnapshot().Serialize());
    CHECK(chain->GetEvents().size()==2+resumed->GetEvents().size());
    for(std::size_t n=0;n<resumed->GetEvents().size();++n)CHECK(chain->GetEvents()[n+2].Serialize()==resumed->GetEvents()[n].Serialize());
    // Finite two-rail flight and spin response use the service, not hand-timed calls.
    c=PhysicsSnapshot::Capture(clock,{Ball(1,0.5)},free.Version,ESnapshotPhase::DuringStep);
    c.Balls[0].Sleeping=false;c.Balls[0].LinearVelocityMetersPerSecond={0,-10,0};c.Balls[0].AngularVelocityRadiansPerSecond={0,0,10};
    CHECK(chain->RestoreSnapshot(c,&error));
    for(int n=0;n<100;++n){CHECK(chain->Step(free.FixedStep,run,&error));CHECK(run!=PhysicsRunState::Faulted);}
    unsigned rails=0;for(const auto& e:chain->GetEvents())if(e.Kind==PhysicsEventKind::Cushion)++rails;
    CHECK(rails>=2);
    // Pocket descent freezes the ball, and once-only pot survives restore.
    TableGeometry g;CHECK(GenerateTableGeometry(bundle.Table,bundle.Cushions,bundle.Pockets,bundle.Geometry,g,&error));
    const auto& p=g.Pockets[0];auto b=Ball(1,0);b.Sleeping=false;
    b.PositionMeters=p.BottomCenter-p.Inward*p.Parameters.JawRadius+Vec3d{0,0,0.08};
    b.LinearVelocityMetersPerSecond={0,0,-0.1};
    c=PhysicsSnapshot::Capture(clock,{b},bundle.Version,ESnapshotPhase::DuringStep);c.PocketStates={{PocketState::InPocket,p.Definition.Id,false}};
    CHECK(service->RestoreSnapshot(c,&error));
    for(int n=0;n<120;++n){CHECK(service->Step(bundle.FixedStep,run,&error));CHECK(run!=PhysicsRunState::Faulted);if(run==PhysicsRunState::Settled)break;}
    CHECK(service->SaveSnapshot().PocketStates[0].State==PocketState::Potted);
    unsigned pots=0;for(const auto& e:service->GetEvents())if(e.Kind==PhysicsEventKind::Potted)++pots;CHECK(pots==1);
    CHECK(service->RestoreSnapshot(service->SaveSnapshot(),&error));CHECK(service->Step(1,run,&error));CHECK(service->GetEvents().empty());
    // Complete 22-ball opening with the same six-pocket geometry.
    OpeningLayout layout{"service-opening-v1",{-1.2,0.1,0.02625},{0.943,0,0.02625},{1,0},0.0001};
    BallDefinition prototype;prototype.Id=1;std::vector<BallDefinition> definitions;
    CHECK(GenerateOpeningPlacement(bundle.Table,prototype,layout,definitions,&error));CHECK(definitions.size()==22);
    std::vector<BallState> opening;
    for(const auto& d:definitions){auto ball=Ball(d.Id,d.InitialPositionMeters.X,d.InitialPositionMeters.Y);opening.push_back(ball);}
    auto rack=PhysicsSnapshot::Capture(clock,opening,bundle.Version,ESnapshotPhase::BeforeShot);
    input.CueDirection={2.143,-0.1,0};input.ForwardSpeedMetersPerSecond=5;
    {
        std::ofstream f("service-opening.snapshot",std::ios::binary);f<<rack.Serialize();CHECK(bool(f));
        std::ofstream shot("service-opening.input",std::ios::binary);shot<<input.Serialize();CHECK(bool(shot));
    }
    CHECK(service->BeginShot(rack,input,&error));
    for(int n=0;n<120;++n){CHECK(service->Step(1.0/60,run,&error));CHECK(run!=PhysicsRunState::Faulted);}
    const auto midRack=service->SaveSnapshot();auto rackResume=CreatePhysicsService(bundle,&error);
    CHECK(rackResume->RestoreSnapshot(midRack,&error));const auto prior=service->GetEvents().size();
    for(int n=0;n<8000;++n){
        CHECK(service->Step(1.0/60,run,&error));CHECK(run!=PhysicsRunState::Faulted);
        PhysicsRunState resumedState;CHECK(rackResume->Step(1.0/60,resumedState,&error));CHECK(run==resumedState);
        if(run==PhysicsRunState::Settled)break;
    }
    CHECK(service->SaveSnapshot().Serialize()==rackResume->SaveSnapshot().Serialize());
    CHECK(service->GetEvents().size()==prior+rackResume->GetEvents().size());
    for(std::size_t n=0;n<rackResume->GetEvents().size();++n)CHECK(service->GetEvents()[prior+n].Serialize()==rackResume->GetEvents()[n].Serialize());
    CHECK(service->IsSettled());CHECK(service->SaveSnapshot().Balls.size()==22);
    hits=0;rails=0;for(const auto& e:service->GetEvents()) {
        if(e.Kind==PhysicsEventKind::BallBall)++hits;if(e.Kind==PhysicsEventKind::Cushion)++rails;
    }
    CHECK(hits>0);CHECK(rails>0);
    {
        std::ofstream f("service-opening-final.snapshot",std::ios::binary);f<<service->SaveSnapshot().Serialize();CHECK(bool(f));
    }
    // A 22-ball training layout exercises a real struck object pot in one shot.
    opening.clear();opening.push_back(Ball(1,0,-0.6));opening.push_back(Ball(2,0,-0.78));
    for(unsigned id=3;id<=22;++id)opening.push_back(Ball(id,-1.4+0.12*(id-3),0.4));
    rack=PhysicsSnapshot::Capture(clock,opening,bundle.Version,ESnapshotPhase::BeforeShot);
    input.CueDirection={0,-1,0};input.ForwardSpeedMetersPerSecond=1;
    CHECK(service->BeginShot(rack,input,&error));
    for(int n=0;n<8000;++n){CHECK(service->Step(1.0/60,run,&error));CHECK(run!=PhysicsRunState::Faulted);if(run==PhysicsRunState::Settled)break;}
    CHECK(service->IsSettled());pots=0;hits=0;
    for(const auto& e:service->GetEvents()){if(e.Kind==PhysicsEventKind::Potted)++pots;if(e.Kind==PhysicsEventKind::BallBall)++hits;}
    CHECK(hits>0);CHECK(pots>0);
    std::cout<<"PASS unified service shot, FPS, high-speed multi-contact, rails, pocket and snapshot continuation\n";
}
