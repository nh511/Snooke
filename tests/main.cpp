#include "Snooker/Data/BallDefinition.h"
#include "Snooker/Physics/BallState.h"
#include "Snooker/Physics/PhysicsSnapshot.h"
#include "Snooker/Shot/ShotInput.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace
{
int Fail(const char* Test, const std::string& Detail)
{
    std::cerr << "FAIL " << Test << ": " << Detail << '\n';
    return EXIT_FAILURE;
}

#define REQUIRE(TestName, Condition) \
    do \
    { \
        if (!(Condition)) return Fail(TestName, #Condition); \
    } while (false)
}

int main()
{
    using namespace Snooker;

    REQUIRE("units", NearlyEqual(CentimetersToMeters(2.625), 0.02625));
    const FSnookerVector3 ExpectedTableNormal = {0.0, 0.0, 1.0};
    REQUIRE("units", TableNormal.NearlyEquals(ExpectedTableNormal));

    FBallDefinition Definition;
    Definition.Id = 1;
    Definition.Type = EBallType::Cue;
    Definition.InitialPositionMeters = {0.5, 0.25, 0.02625};
    const std::string DefinitionText = Definition.Serialize();
    FBallDefinition ParsedDefinition;
    REQUIRE("ball_definition", FBallDefinition::Deserialize(DefinitionText, ParsedDefinition));
    REQUIRE("ball_definition", Definition == ParsedDefinition);
    Definition.MaterialProfileId = "";
    REQUIRE("ball_definition", !Definition.IsValid());

    FBallState State;
    State.Id = 1;
    State.PositionMeters = {0.5, 0.25, 0.02625};
    State.LinearVelocityMetersPerSecond = {1.0, 0.0, 0.0};
    State.AngularVelocityRadiansPerSecond = {0.0, 2.0, 0.0};
    State.Sleeping = false;
    State.SleepTimeSeconds = 0.01;
    State.Contact = {EContactKind::Cloth, 0, TableNormal};
    FBallState ParsedState;
    REQUIRE("ball_state", FBallState::Deserialize(State.Serialize(), ParsedState));
    REQUIRE("ball_state", State == ParsedState);
    State.Contact = {EContactKind::Cloth, 7, TableNormal};
    REQUIRE("ball_state", !State.IsValid());

    FPhysicsSnapshot Snapshot;
    Snapshot.PhysicsParameterVersion = "physics-v1";
    Snapshot.Phase = ESnapshotPhase::DuringStep;
    Snapshot.StepIndex = 42;
    Snapshot.SimulationTimeSeconds = 0.175;
    Snapshot.Balls.push_back(ParsedState);
    FPhysicsSnapshot ParsedSnapshot;
    REQUIRE("snapshot", FPhysicsSnapshot::Deserialize(Snapshot.Serialize(), ParsedSnapshot));
    REQUIRE("snapshot", Snapshot == ParsedSnapshot);
    Snapshot.Balls.push_back(ParsedState);
    REQUIRE("snapshot", !Snapshot.IsValid());

    FShotInput Input;
    Input.CueBallId = 1;
    Input.CueDirection = {0.0, 1.0, 0.0};
    Input.StrikePointOnBall = {0.0, 0.0, -0.25};
    Input.PullbackDistanceMeters = 0.05;
    Input.ForwardSpeedMetersPerSecond = 2.0;
    Input.InputTimeSeconds = 1.25;
    FShotInput ParsedInput;
    REQUIRE("shot_input", FShotInput::Deserialize(Input.Serialize(), ParsedInput));
    REQUIRE("shot_input", Input == ParsedInput);
    Input.StrikePointOnBall = {0.0, 0.0, 1.1};
    REQUIRE("shot_input", !Input.IsValid());

    std::cout << "PASS SnookerCoreTests\n";
    return EXIT_SUCCESS;
}
