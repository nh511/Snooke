# Pocket Laboratory Baselines

Five real PocketSimulation fixtures: entry, jaw, mouth-stop, recovery, capture.
Only entry/capture may emit Potted (exactly once). Jaw must reverse the incoming
normal speed by configured restitution; recovery emits one RecoveredToCloth;
mouth-stop stays asleep without events. Every case compares uninterrupted output
against a serialized/restored midpoint, including event sequence/time bytes.

These are single-ball pocket laboratory cases, not match scoring or measured
physical calibration. Generation is explicit via snooker_test_pocket_baseline
bundle baseline-directory --generate. CTest never rewrites the saved baselines.
