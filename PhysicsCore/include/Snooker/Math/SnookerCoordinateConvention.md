# Coordinates and Units (FND-005)

Core: double precision SI (m, kg, s, rad), right-handed XY table plane and +Z up. Origin is the table center. +X is longitudinal; +Y is the core lateral axis. Angles use the right-hand rule. Position stores ball center, not cloth contact point. Physical dimensions are configuration, not unit constants.

UE adapter contract: table-local UE +X follows core +X, UE +Y follows core -Y, and UE +Z follows core +Z. This explicit reflection M=diag(1,-1,1) maps core to the chosen left-handed UE table basis; do not apply an additional hidden axis flip. World placement is a separate actor transform, never part of the solver.

- Position and linear velocity: UE = 100*M*core (cm, cm/s); inverse = M*UE/100.
- Direction, surface normals: UE = M*core, no length scaling.
- Angular velocity (axial vector, rad/s): UE = det(M)*M*core = (-x,y,-z), no factor of 100.
- Rotation matrices: R_UE = M*R_core*M. Quaternion adapter must implement the same basis mapping; component copies are not an approved conversion.
- Time, mass and radius-independent scalar values retain SI semantics; convert only at documented adapter boundaries.
- FTransform translation uses the position rule, rotation uses basis conjugation, scale is dimensionless. Do not apply a second centimeters conversion through actor scale.

The dependency-free functions here return core vectors containing UE-local numeric values, not FVector/FTransform objects. The real UE wrappers and round-trip tests remain COR-008 and require engine validation. Standalone tests check both round trips and omega-cross-radius consistency so angular handedness cannot silently drift.
