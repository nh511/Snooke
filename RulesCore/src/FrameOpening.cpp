#include <Snooker/Rules/FrameOpening.h>
#include <utility>

namespace Snooker
{
bool CreateFrameOpening(const TableDefinition& Table,const BallDefinition& Prototype,const OpeningLayout& Layout,
    const std::string& PhysicsParameterVersion,Seconds FixedStep,std::uint32_t FrameNumber,std::uint32_t Breaker,
    FrameOpening& Output,std::string* Error)
{
    FrameOpening opening;
    opening.Match.FrameNumber=FrameNumber;opening.Match.CurrentPlayer=Breaker;
    if(!opening.Match.IsValid(Error))return false;
    if(!GenerateOpeningPlacement(Table,Prototype,Layout,opening.Definitions,Error))return false;
    PhysicsClock clock;
    if(!clock.Restore({FixedStep,0,0})){if(Error)*Error="invalid opening fixed step";return false;}
    std::vector<BallState> balls;
    for(const auto& definition:opening.Definitions) {
        BallState ball;ball.Id=definition.Id;ball.PositionMeters=definition.InitialPositionMeters;
        ball.RadiusMeters=definition.RadiusMeters;ball.MassKg=definition.MassKg;ball.MaterialProfileId=definition.MaterialProfileId;
        ball.Contact={EContactKind::Cloth,0,TableNormal};balls.push_back(ball);
    }
    opening.Physics=PhysicsSnapshot::Capture(clock,balls,PhysicsParameterVersion,ESnapshotPhase::BeforeShot);
    if(!opening.Physics.IsValid(Error))return false;
    opening.TableVersion=Table.ParameterVersion;opening.LayoutVersion=Layout.ParameterVersion;
    Output=std::move(opening);return true;
}
}
