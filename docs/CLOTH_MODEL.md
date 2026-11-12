# Cloth Model Baseline

Homogeneous solid sphere, I=2/5 mr^2, flat stationary XY cloth. Gravity is an explicit ClothEnvironment parameter (engineering default 9.80665 m/s^2). No empirical calibration is claimed.

Slip is the planar projection of v+omega cross (0,0,-r). Sliding friction applies a bounded velocity change opposite slip and the corresponding angular impulse. It is capped at 2/7 of slip speed to avoid reversal. A nonspinning sphere approaches pure rolling at 5/7 v; sliding friction must NOT stop pure rolling by itself.

Rolling resistance acts only for slip below a numerical 1e-9 m/s threshold and negligible vertical velocity. It reduces speed by rollingCoefficient*g*dt without reversal and maintains omega.X=-v.Y/r and omega.Y=v.X/r. Vertical spin decay is PHY-006, not rolling resistance. This is an uncalibrated engineering constant-deceleration model.

Functions update velocity only, reject invalid input before mutation and leave sleeping bodies untouched. ApplyAngularUpdate integrates sliding contact torque exactly once and adds a vertical spin decay of Cloth.Spin rad/s^2, clamped at zero. Call it INSTEAD OF ApplySlidingFriction for that substep, then apply rolling resistance. Side spin is symmetric and cannot create translation on this ideal flat cloth. Follow spin can transfer rotational energy into translation; total energy, not linear speed alone, determines dissipation.

Position integration lives in the tests, not a completed PhysicsService. Sleep, transition hysteresis and events remain separate tasks. Analytical stopping distance is checked with an explicit 0.005 m finite-step integration tolerance, not a claimed industry standard. CoreConfig's spin field uses the same rad/s^2 meaning; solver callers must build ClothProfile from the loaded configuration rather than unrelated local constants.
