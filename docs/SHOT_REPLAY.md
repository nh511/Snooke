# Physical Shot Replay

snooker_replay record.shot loads shot-v2 and recomputes with the embedded bundle,
initial snapshot and cue input. Recorded events/final state are comparison targets
only. ReplayShot compares each generated event immediately, reports its first
divergent step/sequence, and compares the final snapshot byte-for-byte.

On failure the caller's output record is unchanged. Unsupported solver builds,
malformed transport, event/final tampering and excess replay work are explicit
failures. Exact replay is tested for the center baseline and the independently
generated 22-ball opening. No engine, network or prerecorded motion drives replay.

This is same-solver, same-toolchain reproducibility, not untested cross-platform
bitwise compatibility. Additional baselines must be included in replay tests as
they are introduced.
