# PhysicsClock (PHY-002)

Default step 1/240 s; all simulation callbacks get exactly the configured step. Render delta only accumulates time. Completed step count determines simulation time; no variable-step callback or render-driven state write exists. A step-relative 1e-10 arithmetic tolerance prevents representational residue losing an otherwise exact boundary step.

Advance is budgeted (1024 steps by default), preserves unprocessed time and can continue with delta zero. Inputs reject NaN, negative time, zero budget, missing callback, counter overflow and more than one hour of pending debt before mutating the clock. Supported configured step range is 1e-6 to 1 second, a numerical safety bound, not a calibrated solver recommendation.

ClockState stores fixed step, accumulator and completed steps for restoration. It is captured by PhysicsSnapshot v2 and embedded in ShotRecord v1; event replay and a complete physics service remain pending. Callbacks must not throw or reenter the same clock. Tests compare real BallState position updates at 30/60/120 FPS and inspect byte-identical serialization; this is uniform-motion scheduler validation, not friction/collision physics or a claim of real engine FPS testing.
