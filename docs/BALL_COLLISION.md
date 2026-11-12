# Ball Collision Development

## Touching Query (PHY-015)

QueryTouching is read-only and returns signed gap, normal speed, relation (separated/touching/overlapping) and motion (approaching/resting/separating). Explicit nonnegative gap/speed tolerances are engineering numerical bounds, not referee decisions. Identical IDs, coincident centers and arithmetic overflow fail without changing the output. This query neither emits collision events nor awards fouls. Playing away and touching-ball rules remain RulesCore work.

Repeated resting/separating contact does not append impulse events. A genuinely approaching contact can apply one physical response, after which separation prevents duplicate response events in the isolated pair tests. Multi-contact stack deduplication remains the future solver's responsibility.

PHY-011 uses swept AABBs expanded by actual ball radii, for constant translation over a supplied horizon. It includes sleeping bodies, since another ball can hit one. IDs are unique and sorted; one canonical pair is returned. Broad phase is conservative and may include false positives. Recompute after forces or impulses change trajectories. Pairwise O(n^2) is deliberate for the small snooker ball count; 4096 is a resource guard, not a supported performance target.

PHY-012 solves relative sphere contact in dimensionless u=t/horizon using scaled geometry and a cancellation-resistant early quadratic root. Query success with Hit=false means no contact; invalid inputs return false and do not change the output. Initial touch/overlap returns t=0; coincident centers have a deterministic ID-based diagnostic normal. Separation/moving-apart, exact tangency and zero horizon are explicit cases. This is constant-velocity TOI, not curved-path CCD; force integration must subdivide/rebuild queries.

The query does not advance or bounce the balls. The tests advance a high-speed ball only to the computed contact and independently verify separation equals summed radii, even though advancing the entire step would pass through the target. Pair response and rich event logging are implemented separately below; chronological multi-contact stepping, overlap handling and a full service remain pending and are not completed by the query. No cushion/pocket collisions are implemented here.

## Collision Log v1 (PHY-014)

CollisionLog embeds the complete BallCollisionProfile bytes and ordered events. Each applied contact records physical step/time, full before/after BallState for both IDs, point/normal, relative contact velocity, normal/tangential impulse and total kinetic energy before/after. The response wrapper generates diagnostics from real calculations, not caller-provided result flags. Separating contacts append nothing. Equal timestamps/steps preserve append order; decreasing times or steps fail transactionally.

The independent snooker-collisions-v1 format uses quoted profile/state records, classic-locale max_digits10 and an explicit end marker. Loader validates IDs/states, order and exact reconstruction of each recorded pair response before accepting it. Up to 100000 events and 64 MiB input are allowed for resource safety. Logs verify pair responses/order, NOT free-flight/cloth integration between events or cross-build bitwise determinism. Current ShotRecord event transport is still simpler; integration/versioning into a complete shot replay remains pending. Do not claim REP-002 completion.

Tests compute two actual chronological contacts through three balls using TOI and response, persist/reload events, reconstruct pre/post changes and reject reordered/corrupted states or diagnostics. A global earliest-event multi-ball stepping service, event deduplication across zero-time stacks and overlap correction are still separate work.

## Pair Response (PHY-013)

ResolveBallContact applies a normal restitution impulse and bounded Coulomb tangential impulse at touching solid spheres, including both angular velocities in contact slip. Configuration is the separate versioned ball-contact profile; fixture restitution/friction/tolerance are engineering values, not calibration. Response wakes affected balls, resets sleep timers and returns contact diagnostics without rule/UI coupling. Separating contacts are valid no-ops. Gross overlap, coincident centers, noncontact pairs, nonfinite arithmetic and invalid material bounds are errors with unchanged balls/result.

Tests split one high-speed step at TOI, apply response then finish the remaining step; target does not tunnel in that isolated pair test. Also tested: elastic equal/unequal masses, thin and slow contacts, spin transfer, linear/angular momentum and nonincreasing total kinetic energy. This does not complete a chronological multi-ball solver, contact-stack iteration, calibrated throw or global penetration correction. Full BallCollisionProfile bytes must be embedded in a future replay/config revision BEFORE collision simulation is accepted as replayable; current ShotRecord does not capture them.
