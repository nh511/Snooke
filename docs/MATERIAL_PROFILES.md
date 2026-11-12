# Independent Material Profiles

All files are uncalibrated engineering fixtures. IDs resolve table references; each profile carries its own immutable parameter version. Parsing is transactional, uses the classic locale and rejects unsupported schemas, nonfinite/out-of-range fields, truncation and extra tokens. These files are not UE assets.

## Cushion v1 (DAT-004)

Token order: snooker-cushion-v1, profile ID, parameter version, normal restitution [0,1], dimensionless nonnegative tangential friction, contact height in meters (>0), pocket-transition length in meters (>=0), signed pocket-transition angle in radians ([-pi/2,pi/2]). The angle describes local contour departure; left/right mirroring is a geometry-generator responsibility, not an implicit sign change in the loader.

data/profiles/cushion-standard.txt and cushion-training.txt resolve the IDs referenced by the two current table fixtures. Changing material/transition parameters requires no geometry-source edit. Neither an actual cushion collision response nor generated pocket contours are implemented by this data task. DAT-007/PHY-016/017 must consume these files before their own acceptance.

CoreConfig v1 retains a coarse material summary for the existing single-ball cloth experiment; it is NOT the complete material registry. The separate profile files and their versions must be included/resolved in a future config/ShotRecord schema before cushion/pocket parameters influence simulation. Do not silently reconstruct them from local defaults on replay.

UCushionProfileAsset and engine import/compilation remain unverified until UE5 is available; DAT-004 whole-task status remains REVIEW even after standalone tests pass.

## Cue v1 (DAT-006)

Token order: snooker-cue-v1, ID, parameter version, length (m), mass (kg), tip radius (m), maximum pullback (m), maximum impulse (N s), maximum forward speed (m/s), maximum lateral offset/radius ratio [0,1), tip restitution [0,1]. AllowsInput bounds pullback, sampled speed and transverse tip offset; effective offset is also bounded by ball radius minus tip radius. MaxImpulse is enforced by the later impulse solver, not by changing rules. Two presets exercise distinct reach/speed constraints. CueProfile replaces the original FCueProfile name with a compatibility alias to one implementation.

CoreConfig v2 persists every cue field; v1 is explicitly rejected rather than silently filling missing limits. Embedded ShotRecord config uses the same v2 loader; old records containing v1 are rejected with a config schema error. No automatic old-record migration or UCueProfileAsset import is claimed. Geometry/material profiles for cushions/pockets still require future registry integration before they influence simulation.

## Pocket v1 (DAT-005)

Token order: snooker-pocket-v1, ID, parameter version, opening width (m), jaw radius (m), signed jaw angle (rad, [-pi/2,pi/2]), bottom depth (m, positive downward from cloth), inward/downward bottom slope (rad, [0,pi/2)), capture depth (m, >0 and <= bottom depth), jaw restitution [0,1], minimum outward ejection speed (m/s, >=0).

Capture depth is a geometry parameter for a future state machine, not an enter-trigger score. Ejection threshold does not move a ball by itself. Separate standard/training files match the IDs, opening and depth in the current table fixtures. Duplicated opening/depth are a legacy table-v1 compatibility constraint; future registry integration must reject disagreement, not pick an arbitrary precedence. UPocketProfileAsset, actual jaw contact/bottom capture and once-only pot events remain unimplemented/unvalidated.
