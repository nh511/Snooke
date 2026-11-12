# UE Skeleton and Integration Boundary

FND-002 creates module descriptors and startup/target rules only. No UE build, editor startup, map, packaging or physics integration has been performed. Exact UE5 version must be selected and its supported MSVC/SDK verified before generating project files. EngineAssociation is intentionally not fabricated.

Dependencies: SnookerGame -> SnookerPhysicsUE -> SnookerPhysicsCore -> shared PhysicsCore/RulesCore source. SnookerEditor is Editor-only and may depend on UnrealEd; no runtime or standalone core target depends on it. The empty plugin core module is only a UE module startup hook; it is NOT the actual standalone core compiled into UE yet.

COR-007: design USnookerPhysicsSubsystem as owner of simulation calls and event forwarding; USnookerBallComponent and USnookerTableComponent present/query state. No actor writes solver state directly. No class skeletons or functioning service are claimed here.

COR-008: implement FVector/FQuat/FTransform wrappers following PhysicsCore/include/Snooker/Math/SnookerCoordinateConvention.md; test handedness, units, polar/axial vectors and round trips in engine.

COR-009: compile the SAME PhysicsCore/RulesCore sources into the plugin without source copies or precompiled static libraries. Determine UE exception/RTTI and STL/runtime requirements and disable unity compilation if conflicting anonymous namespaces are combined. This integration is intentionally not predeclared as working by this module skeleton.

COR-010: read the same versioned config and shot input, compare CLI/UE events and final state. No UE import or result parity has yet been validated.

Required FND-002 acceptance once UE exists: generate project files, build Editor Development and Editor DebugGame using the selected UE build scripts, and open the actual editor. Capture build commands, exit codes and engine version. Missing engine is a blocker, not a passed test.
