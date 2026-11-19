# Task Status

Authority: DEVELOPMENT_PLAN.md and EXECUTABLE_TASKS.md (UE 5.8.4 + standalone C++17).
Unlisted tasks are TODO. A partial implementation is not DONE.

| ID | Status | Evidence / Remaining acceptance |
|---|---|---|
| FND-001 | DONE | Git initialized; independent source-copy Debug/Release configure, build and CTest passed (exit 0) |
| FND-002 | BLOCKED | Project/plugin/module skeleton and design created; descriptors checked; no UE editor for Development/DebugGame build |
| FND-003 | BLOCKED | Layout check passed; real UE example resource requires editor, README is not an asset |
| FND-004 | REVIEW | Local LFS hooks/attributes/ignore rules verified; clone/LFS recovery pending initial authorized commit and remote |
| FND-005 | DONE | SI/basis conversion specification and standalone tests passed; real UE wrappers deferred to COR-008 |
| FND-006 | REVIEW | Core category/filter/file sink tests passed; real UE log forwarding remains unvalidated |
| FND-007 | REVIEW | Independent CTest and nonzero failure exit passed; UE Automation reserved but not run |
| COR-001 | DONE | Public math headers in PhysicsCore/include/Snooker/Math; Debug/Release CTest 5/5 |
| COR-002 | DONE | Four standalone targets build separately; clean-copy Debug/Release CTest 7/7 |
| COR-003 | DONE | snooker_validation loads parameters and snapshot; empty-state/math tests passed in Debug/Release |
| COR-004 | REVIEW | Versioned text schema, defaults, loader and CLI passed; actual UE consumption not validated |
| DAT-001 | REVIEW | BallDefinition file persistence/reload/print passed for eight types; DataAsset import pending UE |
| PHY-001 | DONE | Public BallState copy independence, equality, contact validation and serialization passed |
| PHY-002 | DONE | Default 240 Hz clock; identical state at 30/60/120 FPS; debt, restore and invalid time tested |
| PHY-003 | DONE | Snapshot v2 restores clock remainder and ball state; persisted continuation matches uninterrupted execution |
| COR-005 | DONE | ShotRecord schema/sample and file round trip passed with embedded config/input/snapshots/events/result; not physical replay |
| DAT-003 | REVIEW | Shared cloth profile and bounds tests passed; real UClothProfileAsset remains unvalidated |
| PHY-004 | DONE | Contact sliding friction and combined slide-to-stop tests passed; no energy gain |
| PHY-005 | DONE | Rolling distance decreases with resistance; analytic stop distance and combined stable stop passed |
| PHY-006 | DONE | Sliding torque integration, follow/backspin and symmetric side-spin decay passed |
| PHY-007 | DONE | High-speed remains sliding; contact-friction transition and low-slip hysteresis passed |
| PHY-008 | DONE | Sustained sleep/wake/timer guards and settled state at 30/60/120 FPS passed |
| COR-006 | DONE | Multi-ball viewer/geometry/event jump/playback passed Chrome desktop/mobile plus Debug/Release parser regression 67/67 |
| DAT-002 | REVIEW | Two table files and round-trip/invalid-record tests passed; UE DataAsset/CLI parity unvalidated |
| DAT-004 | REVIEW | Versioned cushion files/table reference/bounds/locale tests passed; UCushionProfileAsset unvalidated |
| DAT-005 | REVIEW | Pocket file/geometry/capture/ejection bounds passed; UPocketProfileAsset and actual pocket solver unvalidated |
| DAT-006 | REVIEW | Cue presets/limits and shared config-v2 persistence passed; UCueProfileAsset unvalidated |
| PHY-009 | DONE | Same shot from memory/file/record produces identical impulse and settled trajectory |
| PHY-010 | DONE | Effective-mass impulse, center/offset rotation, cap, conservation and CLI parity passed |
| PHY-011 | DONE | Swept-AABB candidate detection passed high-speed contact oracle, stable order and duplicate checks |
| PHY-012 | DONE | Constant-velocity sphere first-contact query passed high-speed/tangent/overlap/analytic tests; service integration pending |
| PHY-013 | DONE | Normal/tangential contact response passed center/thin/slow/spin and conservation tests |
| PHY-014 | DONE | Actual sequential collisions persisted/reloaded with embedded material and exact pre/post response verification |
| PHY-015 | DONE | Touching relations/motion queried read-only; resting/separating contacts add no spurious events |
| DAT-007 | DONE | Finite profile-driven geometry, regenerated dimensions and normal tests passed; real SVG visualization inspected |
| PHY-016 | DONE | Finite contour contacts distinguish diagnostic boundary, face/top edge and transition; no CCD/service claim |
| PHY-017 | DONE | Profile-driven local cushion response, side-spin impulse and saved one/two-rail response reconstruction passed |
| DAT-008 | DONE | Deterministic 22-ball opening, training/custom placements and occupied color-spot errors passed |
| RUL-001 | DONE | Independent MatchState phases, flags, unsigned scores and file save/restore/print passed |
| RUL-003 | DONE | Deterministic opening rack, stationary snapshot, reset scores/flags and transactional restart passed |
| RUL-004 | DONE | Red, color-after-red, final-color cursor and finished Ball On queries passed; FreeBall explicitly unsupported |
| PHY-018 | DONE | Pocket geometry, six-pocket engineering bundle, snapshot v3/v2 migration, single-ball pocket descent/recovery service and deterministic mid-pocket restore passed; multi-ball/full-table scheduling remains PHY-021 |
| PHY-019 | DONE | Pocket state transitions, once-only pot event, potted no-eject and explicit recovery event passed |
| PHY-020 | DONE | Eight fault codes, checkpoint/diagnostic separation, per-step event budget, frozen failed step and independent speed/timeout/event-loop fault-file replay passed; Debug/Release 54/54 |
| PHY-021 | DONE | Unified service/CLI, closed six-pocket boundary, multi-contact scheduling, 22-ball opening/training pot and mid-shot continuation passed; clean-copy Debug/Release 56/56 |
| REP-001 | DONE | shot-v2 embeds bundle/input/initial/final/events; real 22-ball record file round trip and malformed/order/reference/version rejection passed Debug/Release 56/56 |
| VAL-003 | DONE | Fixed-step multi-ball CSV and JSON snapshots/parameters/events/geometry independently parsed by Node; Debug/Release 56/56 |
| VAL-004 | DONE | Center-strike 476-row short baseline, explicit speed perturbation and first differing step/ball/field report passed Debug/Release 57/57 |
| VAL-009 | DONE | Exact/tolerant trajectory, event and final-state comparison with first-field reports; contract tests and cross-config 22-ball/center outputs passed; Debug/Release 58/58 |
| REP-002 | DONE | Recomputed center/22-ball shot-v2 through shared service; exact events/final comparison and tampering rejection passed Debug/Release 59/59 |
| VAL-005 | DONE | Follow/draw collision, moving slide-to-roll transition, stop/spin traces and exact replay passed Debug/Release 61/61 |
| VAL-006 | DONE | Left/right cue offsets produce mirrored nonzero cushion deflection and axial spin; baselines/replay passed Debug/Release 62/62 |
| VAL-007 | DONE | One/two-rail, touching-rail and spin traces plus 30/60/120/144 FPS exact event/final/replay parity passed Debug/Release 66/66 |
| VAL-008 | DONE | Entry/jaw/mouth-stop/recovery/capture fixtures assert pot/no-pot and exact midpoint restore; Debug/Release 67/67 |
| REP-003 | DONE | Sparse solver checkpoints, exact step/time/event seek, first collision/pot and fault checkpoint tests passed Debug/Release 68/68 |
| RUL-002 | DONE | Read-only typed shot physics input validates metadata/events/pot transitions and cached result; real pot and corruption tests passed Debug/Release 69/69 |
| RUL-005 | DONE | Match-v2 nomination fields, normal red/color scoring, real physical pot integration, overflow and v1 migration passed Debug/Release 70/70 |
| RUL-006 | DONE | Cue-ball pot foul, ball-in-hand D placement, same-shot object-score suppression and invalid position/overlap rejection passed Debug/Release 71/71 |
| RUL-007 | DONE | Explicit legal/wrong/no-contact first-contact judgement by Ball On and ball metadata passed Debug/Release 72/72 |
| RUL-008 | DONE | Foul reasons and max(4,relevant value) opponent award including no-contact/cue-pot/illegal-pot/free-ball rejection passed Debug/Release 73/73 |
| RUL-009 | IN_PROGRESS | Adding same-shot multi-ball pot ordering and normal/foul/cue-pot combination classification |
| RUL-008 | IN_PROGRESS | Adding foul reason/value calculation with minimum four and maximum relevant ball value |
| RUL-007 | IN_PROGRESS | Adding first-contact legal Ball On judgement and no-contact classification |
| RUL-006 | IN_PROGRESS | Adding cue-ball pot foul result, ball-in-hand and D-area placement validation |

## Execution Rules

- One task at a time: IN_PROGRESS, REVIEW, then DONE only after all applicable acceptance criteria pass.
- UE-dependent acceptance is recorded separately and is never replaced by standalone tests.
- Initial failure plus at most three retries; stop after the fourth failure, record BLOCKED and notify the user.
- No identity changes, skipped tests, or mocked physics acceptance. User-authorized per-task commits and per-stage pushes follow the continuation plan below.

## Current Toolchain

CMake 4.4.4, VS2022 Build Tools, MSVC 19.44, Windows SDK 10.0.26100.0, Git and Git LFS.
Unreal Engine 5.8.4 is not installed/validated; no UE compilation or asset import is claimed.

## History

2026-11-09: UE5 and the C++ toolchain were unavailable under the old task definitions.
2026-11-10: C++ toolchain installed; old data tests built and passed in Debug and Release.
The earlier Unity/C# branch of the documents is superseded; no C# migration is being performed.

## Failure Ledger

- PHY-018 run p18: first Debug run 45/46. Constructed jaw test used an endpoint-only analytic expectation although the slanted segment interior contacted earlier. Retry 1 separates endpoint and perpendicular-segment analytic fixtures, without loosening tolerance or changing the sweep solver. Original failure retained in build-evidence/p18.

- PHY-017 first run p17: core_cushion_response failed expected side-spin sign. Bottom rail normal is +Y, lever -Y, positive Z spin yields +X slip and therefore -X friction. Corrected test sign and added an analytic tangential impulse assertion. Solver not altered; retry 1 passed Debug/Release 41/41. Original failed Debug evidence retained.

- DAT-007 initial run g7: Debug CTest 37/39, training geometry rejected conflicting table rail height 0.035 m versus referenced material 0.032 m. Training fixture corrected to 0.032 m, version table-training-v2. Retry 1 (g7r1) passed Debug/Release 39/39. Review then found inverted transition-normal tangent signs; corrected and added orthogonality/unit-normal tests; g7normal passed Debug/Release 39/39. Initial failure evidence retained.

- PHY-010 CLI patch transport: initial CMake diagnostic newline broke generated patch; retry 1 applied. PHY-011 patch transport: first status hunk was out of order; retry 1 split status and implementation. Neither was a numerical test failure.

- COR-006 single-ball CLI: first run hit the 120 s guard because fixture spin 1 rad/s at 0.005 rad/s^2 needs about 200 s to decay. Retry 1 uses a short-run fixture spin of 0.1 rad/s; solver coefficients and timeout guard are unchanged. The failed run is retained in build-evidence/cor006-single-ball.

- PHY-005 patch: first implementation insertion context did not match; state/declaration were already applied. Retry 1 adds a separate implementation file and tests; no missing implementation is counted as passed.

- Patch transport: two failures (path expansion, multiline argument); third attempt succeeded using UTF-8 native process arguments. No source/test failure was concealed.
- Verification script: execution policy blocked first invocation; second invocation with process-only Bypass succeeded. No system policy modified.
- FND-006: first CTest run failed because its file path contained an accidental control character; corrected test argument, retry 1 passed.
- COR-002: oversized patch failed to start; retry 1 with per-file patches succeeded.
- Parser repair: first patch had out-of-order status context and was rejected before changes; retry 1 split into small patches.
- Parser tests: first Debug run found a literal decimal expectation incompatible with max_digits10 formatting. Retry 1 uses exactly representable 1.5 and still checks invariant decimal point and full snapshot round trip.

- PHY-020 run `phy020-a0`: initial Debug/Release attempt exposed three issues: invalid fault records for malformed snapshots, fault state lost through transactional service copy, and the PHY-018 jaw fixture violating the new cloth safety check.
- PHY-020 retry 1 `phy020-a1`: compile failure because `PhysicsFault.cpp` omitted `<set>`; corrected, but the full run still found the same jaw fixture compatibility issue.
- PHY-020 retry 2 `phy020-a2`: fault-specific tests passed and bundle regeneration passed; only the PHY-018 jaw assertion remained because the test expected a negative tangent velocity that the configured jaw geometry did not produce.
- PHY-020 retry 3 `phy020-a3`: narrowed `LeftTable` to `OnCloth/Ejected` states; all fault tests and other regressions passed, but the jaw assertion still failed. Per execution rules, PHY-020 is now BLOCKED after the initial failure plus three retries. Do not start PHY-021 until the jaw fixture/response contract is corrected and a fresh Debug/Release run passes.

## Evidence (2026-11-10)

- PHY-018: Test-Current.ps1 -Run p18r1, exit 0; Debug/Release 46/46. Generated finite pocket geometry, jaw endpoint/interior contact, slope, bottom, high-speed cases, finite misses, malformed geometry, initial overlap and verified rebound passed. Initial endpoint timing expectation was corrected in retry 1; original failure retained. No scoring/pot event is emitted by contact query.

- RUL-003: Test-Current.ps1 -Run r3, exit 0; Debug/Release 44/44. Actual 22-ball opening factory, explicit breaker/frame, repeated restart after dirty scores/positions, resting cloth snapshot and unchanged invalid restart tested. Not a running match, score processor or save/replay service.

- RUL-001: Test-Current.ps1 -Run r1, exit 0; Debug/Release 43/43. All four phases with nondefault player/scores/flags saved, restored and printed; invalid enum/phase/red count/boolean/unsigned overflow/truncation/tail rejected without altering output. No scoring, transition, FreeBall or Miss rule implementation claimed.

- DAT-008: Test-Current.ps1 -Run d8, exit 0; Debug/Release 42/42. Explicit file-driven six color spots/rack layout generates exactly 15 reds+six colors+cue, repeated 100 times with byte-identical results; radius containment, nonoverlap, canonical ID sorting, training fixtures, occupied reset/missing spots/duplicate IDs invalid cases tested transactionally. No match scoring or official alternative-spot precedence claimed.

- PHY-017: Test-Current.ps1 -Run p17r1, exit 0; Debug/Release 41/41. Real generated finite rail contacts, normal restitution, analytic side-spin friction impulse, energy, separating no-op, stale normal/material rejection and saved one/two-rail contact reconstruction passed. Full-table stepping, CCD and complete replay remain unimplemented. Initial p17 failure is retained; retry 1 succeeded.

- PHY-016: Test-Current.ps1 -Run p16, exit 0; Debug/Release 40/40. Finite endpoint misses, face/top-edge distinction, transition contact, diagnostic-only pocket gap boundaries, stable output order and transactional invalid queries tested. Current-position query only; no high-speed cushion CCD or full top-face model claimed.

- DAT-007: Test-Current.ps1 -Run g7normal, exit 0; Debug/Release 39/39 after height conflict fix and transition-normal orthogonality correction. Generated two actual SVG files, rendered using local headless Chrome and inspected build-evidence/g7-images/standard.png and training.png. Tested configuration changes, deterministic regeneration, profile/height/size mismatch, duplicate profiles and transactional failure. Partial engineering fixtures, not calibrated six-pocket geometry or UE mesh creation.

- PHY-014: Test-Current.ps1 -Run p14, exit 0; Debug/Release 35/35. Two real TOI/response contacts through three balls, rich state/diagnostic file round trip, order, no-op exclusion, corrupted state/energy rejection and transactional failure tested. Independent event log, not integrated full ShotRecord replay.

- PHY-013: Test-Current.ps1 -Run p13, exit 0; Debug/Release 34/34. Actual profile file loading, TOI-split 1000 m/s head-on impact, equal/unequal mass, thin/slow/spinning contacts, momentum/angular momentum/energy and separating no-op tested. Invalid material/contact errors preserve both balls and diagnostics. No full multi-ball service or overlap correction claimed.

- PHY-012: Test-Current.ps1 -Run p12, exit 0; Debug/Release 33/33. Query hit/no-hit/error semantics, 1000 m/s crossing, first-contact separation, reverse ordering, tangency/miss, initial overlap, zero horizon and 500 analytic contacts tested. This is TOI query acceptance, not a completed nonpenetrating physics service.

- PHY-011: Test-Current.ps1 -Run p11, exit 0; Debug/Release 32/32. High-speed sweep, 1000 seeded constructed contacts, stable pair ordering, duplicate IDs and overflow/negative-horizon transactional rejection tested. Constant velocity per substep; rebuild after force/impulse changes.

- PHY-009/010: Test-Current.ps1 -Run p10-cli, exit 0; Debug/Release 31/31. Core analytic impulse/momentum/energy, high/low/left/right rotation, speed scaling, capped impulse, zero-speed no-op and invalid/moving target rejection passed. File and ShotRecord CLI entries produce byte-identical struck snapshots, cloth CSV and settled snapshots. No UE input/collision/full replay claim.

- PHY-009 input stage: Test-Current.ps1 -Run p9, exit 0; Debug/Release 29/29. File round trip, transverse coordinates, metric/normalized offset agreement, finite direction, ball/profile IDs and cue bounds tested. Multi-entry simulation not yet accepted; see PHY-010.

- DAT-006: Test-Current.ps1 -Run d6, exit 0; Debug/Release 28/28. Two file presets enforce distinct pullback/speed/offset limits, tip reach is bounded, config-v2 and embedded ShotRecord persist all new cue fields. Unsupported config-v1 is explicitly rejected. No rule state changes or UE import claimed.

- DAT-005: Test-Current.ps1 -Run d5, exit 0; Debug/Release CTest 27/27. Two profile files resolve table IDs/opening/depth, round-trip and reject invalid jaw/bottom/capture/ejection values, truncation and tail without changing output. No pot/ejection simulation or UE import claimed.

- DAT-004: Test-Current.ps1 -Run d4, exit 0; Debug/Release CTest 26/26. Two on-disk presets resolve table cushion IDs, round-trip with classic locale and reject bad bounds/schema/truncation/tail transactionally. This is parameter acceptance, not cushion collision or UE import.

- PHY-008: Test-Current.ps1 -Run phy008-sleep, exit 0; Debug/Release 24/24. Persistent timer round trip, no early sleep, stable sleeping state, wake, displacement and angular guards, invalid input and equal 30/60/120 FPS settled states tested. No claims of collision wake/service integration.

- PHY-007: Test-Current.ps1 -Run phy007-motion, exit 0; Debug/Release 23/23. High-speed no-spin ball does not instantly roll, real contact updates cause one transition, threshold-band noise preserves mode and classification leaves ball state unchanged. Classifier mode is caller-owned/diagnostic and is NOT currently a hidden solver state influencing snapshot continuation.

- PHY-006: Test-Current.ps1 -Run phy006-angular, exit 0; Debug/Release 22/22. Follow/backspin produce opposite contact torque and linear response, left/right spin decays symmetrically to zero without translation, composition applies sliding only once. This is velocity integration, not rendered orientation or empirical calibration.

- PHY-004/005: Test-Current.ps1 -Run phy005-rolling, exit 0; Debug/Release 21/21. Solid-sphere sliding reaches rolling, rolling resistance then reaches exactly zero velocity/spin for the no-initial-spin case; no reversal or energy increase. Rolling stopping distance compared with v^2/(2*mu*g) at an explicit 0.005 m integration tolerance; two coefficients give monotonic distance. Not empirical calibration.

- PHY-004: Test-Current.ps1 -Run phy004-sliding, exit 0; Debug/Release 20/20. Analytic initial deceleration, contact rolling limit 5/7 v, energy nonincrease, zero coefficient, tiny speed and invalid time tested. Sliding alone must not dissipate pure rolling; stop acceptance remains pending rolling integration, not misreported DONE.

- DAT-003: Test-Current.ps1 -Run dat003-cloth, exit 0; Debug/Release 19/19. Tested independently switchable versioned cloth conditions, friction and sleep thresholds; friction behavior acceptance follows PHY-004. No UE asset test performed.

- COR-005: Test-Current.ps1 -Run cor005-record, exit 0; Debug/Release 18/18. Real file save/reload preserves config/input/ordered events/final state. Rejects version mismatch, reversed events, invalid ball references, bad input limit, truncation and tail; output remains unchanged. Schema fixture is explicitly not a simulated shot.

- PHY-003: Test-Current.ps1 -Run phy003-snapshot, exit 0; Debug/Release 17/17. Mid-step file restore and continued 600 frames yield exact ball/clock state. V1 is explicitly rejected because it omits clock state. Current solver-state coverage only, not complete shot replay.

- PHY-002: Test-Current.ps1 -Run phy002-clock, exit 0; Debug/Release 16/16. Compared actual BallState updates at 30/60/120 FPS, tested 120/240/480 Hz, half-step residue, debt continuation, restoration and invalid inputs. No friction/collision model claimed.

- PHY-001: Test-Current.ps1 -Run phy001-state, exit 0; Debug/Release 15/15. Verified standalone DAT-001 output used under development plan section 7.0, not a nonexistent DataAsset. No UE state dependency exists.

- FND-001: powershell -NoProfile -ExecutionPolicy Bypass -File Tools/Verify-Standalone.ps1 -Run fnd001-source-copy; exit 0. Independent source copy in build-evidence/fnd001-source-copy/source, configure/build-Debug/build-Release/test-Debug/test-Release.txt alongside it. CTest 1/1 in each configuration.
- FND-003: PowerShell checked eight resource README locations and rejected source extensions under Content; exit 0. No real UE assets validated.
- FND-004: git lfs install --local and git lfs version; exit 0. No commit or user identity was created.
- FND-004: git check-attr confirmed LFS for .uasset/.umap; git check-ignore confirmed plugin Binaries/Intermediate and build evidence excluded, exit 0. Clean clone/LFS remote recovery not verified.
- FND-005: Verify-Standalone.ps1 -Run fnd005-units, exit 0; Debug and Release CTest 2/2. Evidence in build-evidence/fnd005-units. Contract includes position, polar/axial vector and rotation matrix basis mapping, not UE runtime validation.
- FND-006: Test-Current.ps1 -Run fnd006-log-retry1, exit 0; Debug/Release CTest 3/3. Original failure retained in build-evidence/fnd006-log; retry logs in build-evidence/fnd006-log-retry1. Retry 1 succeeded.
- FND-007: Test-Current.ps1 -Run fnd007-exit, exit 0; Debug/Release CTest 4/4. UnitsFailureExit checks an actual wrong numeric expectation (expected exit 1), not a skipped or inverted physics test. FND-008 remains TODO pending FND-006 acceptance.
- COR-001: Test-Current.ps1 -Run cor001-public-headers, exit 0; Debug/Release CTest 5/5. Vec2d/Vec3d, quaternion rotations, transforms, SI/ID/time aliases and invalid math inputs validated. FSnookerVector3 remains a compatibility alias, not a second vector implementation.
- COR-002: Test-Current.ps1 -Run cor002-targets and Verify-Standalone.ps1 -Run cor002-clean-copy; exit 0, Debug/Release 7/7. PhysicsCore, RulesCore, Tests, Tools separately built (exit 0). Evidence: build-evidence/cor002-clean-copy and build-evidence/cor002-targets. No complete scoring or solver claimed.
- Parser safety repair: Test-Current.ps1 -Run parser-safety-retry1, exit 0; Debug/Release 8/8. Tests cover truncated tokens, invalid enums, negative/overflow integer fields, excessive counts, header/trailing junk, locale-invariant round trip and unchanged output after failure. This does not complete PHY-003 or replay.
- COR-003: Test-Current.ps1 -Run cor003-runner, exit 0; Debug/Release 10/10. Runner loads validation-v1 parameters and real snapshot parser, checks empty state and quaternion calculation; nonempty physics is explicitly unsupported pending solver. Negative-input test expects rejection, not physics success.
- COR-004: Test-Current.ps1 -Run cor004-config, exit 0; Debug/Release 12/12. Shared schema/default file and CoreConfig loader tested for valid input, round trip, resource limits, invalid version, truncation and unchanged output. CLI reads the same file. UE consumption not tested. Historical whole-parent gating was subsequently superseded by the section 7.0 standalone dependency interpretation below.
- FND-002: Node JSON/module-path check exit 0 (structure only). Default Epic install/manifest and editor command lookup found no available engine. Missing editor/UBT prevents real Development/DebugGame acceptance. No repeated engine install attempts made.
- DAT-001: Test-Current.ps1 -Run dat001-file, exit 0; Debug/Release 13/13. Public BallDefinition and compatibility alias share one implementation; eight ball types persisted/restored/printed. No DataAsset import is claimed; standalone consumers use the verified C++ data output under section 7.0.
- DAT-002: Test-Current.ps1 -Run dat002-table, exit 0; Debug/Release 14/14. Two explicit table fixtures change dimensions without code, persist cushions/pockets/spots/placements, reject bad bounds/duplicate IDs/truncation/tail. Fixtures are partial engineering data, not full calibrated six-pocket geometry. Actual UE loading is unvalidated.

## Remaining Acceptance and Next Work

- DONE: FND-001, FND-005; COR-001, COR-002, COR-003, COR-005; PHY-001 through PHY-017; DAT-007/008; RUL-001/003/004 (28 task IDs). Individual evidence is recorded above; no milestone or project completion is claimed.
- FND-002/003 require actual engine compilation and example assets; engine absence is an external blocker, not a fourth-failure retry case.
- FND-004 still needs authorized initial commit and recoverable clone/LFS data. Attributes and local hooks alone do not prove recovery.
- FND-006/007 require actual UE forwarding/test verification before whole mixed-task acceptance. FND-008 remains TODO; portable logging output can support future independent debug configuration, while UE acceptance remains outstanding.
- COR-004 and DAT-001/002/003 retain whole-task REVIEW for UE acceptance. DEVELOPMENT_PLAN section 7.0 explicitly permits standalone consumers of their verified independent outputs. This supersedes earlier overly strict whole-parent gating; no UE acceptance is bypassed.
- COR-006 is REVIEW; DAT-003 is REVIEW. Unlisted DAT/PHY/RUL tasks remain TODO; existing preparation structs and score lookup do not complete them.
- No failure path reached four failures during this pass. Failures and successful retry evidence above remain visible. No automatic commit was created.
- Final source copy before PHY-001: Verify-Standalone.ps1 -Run final-source-copy, exit 0; Debug/Release 14/14, all five stage exit codes 0.
- PHY-001 status update patch had an out-of-order context failure; retry 1 replaces the small status document after verifying contents. No code was reverted.

## Current Stage Evidence

- PHY-008: Test-Current.ps1 -Run phy008-sleep, exit 0; Debug/Release 24/24 (sleep timing, wake guards and identical settled state).
- COR-006 partial: Test-Current.ps1 -Run cor006-single-ball-retry1, exit 0; Debug/Release 25/25. Node tests/trajectory-data.cjs build/trajectory.csv passed: 4381 actual rows, final step 4380, settled at 18.25 s with zero linear/angular velocity. No collision/pocket events are fabricated.
- Viewer: node tools/verify-viewer.cjs retry 1 passed in local Chrome; canvas 46 distinct sampled colors, previous/next/play functional, no JS exceptions, no desktop/mobile horizontal overflow. Evidence build-evidence/viewer-1794301328433/{desktop.png,mobile.png,result.json}.
- Viewer tooling failures: initial patch generation failed on nested template syntax; retry 1 applied. First browser verification invocation failed before browser startup due to regex escaping; retry 1 using string address parsing passed. Playback fractional-time loss was found and corrected before browser verification.

## Latest Clean Acceptance

- Command: powershell -NoProfile -ExecutionPolicy Bypass -File tools/Verify-Standalone.ps1 -Run st2. Final exit 0. Fresh independent source copy; configure, build-Debug, test-Debug, build-Release, test-Release all exit 0. Debug and Release CTest each 25/25. Logs and stage exit codes: build-evidence/st2.
- Prior attempt -Run standalone-stage-acceptance configured successfully but Debug failed with MSVC C1090 PDB API error code 3 (exit 1). Disk space was sufficient, no active cl/MSBuild found; retry 1 in a shorter fresh evidence directory succeeded without changing source or compiler settings. Path length or transient PDB failure suspected, NOT conclusively diagnosed. Original logs retained; no fourth-failure threshold reached. Prefer short run names for MSVC evidence directories.
- node tests/trajectory-data.cjs build/trajectory.csv: exit 0; parser rejection cases and actual generated 4381-row trajectory verified. This is separate from the 25 CTest cases.
- No UE compilation, clone/LFS server recovery, full cue/collision/pocket simulation or complete game acceptance was performed. COR-006 remains REVIEW.
- Historical next-work note superseded below: DAT-004/005/006 standalone portions and PHY-009 through PHY-015 have now been implemented and verified. Unstarted tasks are not marked IN_PROGRESS.

## Continuation Evidence: Profiles, Cue and Ball Contacts

- Final independent source-copy acceptance: powershell -NoProfile -ExecutionPolicy Bypass -File tools/Verify-Standalone.ps1 -Run c3; exit 0. Configure, Debug build/test and Release build/test all exit 0; CTest 36/36 in BOTH configurations. Evidence build-evidence/c3/{configure.txt,build-Debug.txt,test-Debug.txt,build-Release.txt,test-Release.txt,exit-codes.txt}. This supersedes the earlier 25-test stage, not its historical failure ledger.
- This continuation added seven DONE task IDs: PHY-009 through PHY-015. Overall recorded DONE count is 21. DAT-004/005/006 remain REVIEW for outstanding UE assets. No numerical test failure or build retry was needed in this continuation; two patch-format failures each succeeded on retry 1, as recorded above.
- Cross-entry additional check: Node CSV parser accepted build/shot-entry-Release/files.csv (476 rows, settled at 1.9791666666666667 s). SHA256 matched for files.csv/record.csv in both local Debug/Release builds. This fixed center-strike experiment is not cross-platform determinism, a real-table measurement or full shot replay acceptance.
- Historical next-step note superseded by this continuation: DAT-007/008, PHY-016/017 and RUL-001/003/004 are now implemented with separate evidence. Unstarted tasks stay TODO.

- DAT-004/005/006: runs d4/d5/d6, all stage exit codes 0; Debug/Release CTest 26/27/28 respectively. Whole tasks REVIEW pending actual UE DataAssets. Profiles are engineering examples, not calibration.
- PHY-015: Test-Current.ps1 -Run p15, exit 0; Debug/Release 36/36. 1000 resting queries/responses produce no events; separation produces none, approach produces one, repeat response produces no duplicate. Gap/velocity relation and overlap/invalid input tested.
- Command setup failures during this continuation: two rejected patch formats (PHY-010 CLI newline, PHY-011 out-of-order status context), each resolved on retry 1. No CTest failure or fourth-failure threshold in this continuation so far; older stage failures remain in the ledger.
- Next work after stage acceptance: DAT-007 parameter-driven table geometry/visualization, then PHY-016/017 cushion detection/response. Full-table chronological collision stepping and replay schema integration are still unimplemented; do not infer completion from pair tests.

## Continuation: Geometry, Cushion, Placement and Initial Rules

- Final independent source-copy verification: powershell -NoProfile -ExecutionPolicy Bypass -File tools/Verify-Standalone.ps1 -Run c4; exit 0. Configure, Debug build/test, Release build/test all exit 0. CTest 45/45 in each configuration; see build-evidence/c4/{configure.txt,build-Debug.txt,test-Debug.txt,build-Release.txt,test-Release.txt,exit-codes.txt}. Clean Debug test time 3.50 s, Release 2.23 s. No UE build or playable-game acceptance implied.
- Pure-core scan: 60 C++ header/source files contain no prohibited Unreal headers or control characters. Structural scan only, separate from compilation/tests. DEVELOPMENT_PLAN.md and EXECUTABLE_TASKS.md SHA256 unchanged from start of this continuation. No Git commit or identity change performed.

- RUL-004: Test-Current.ps1 -Run r4, exit 0; Debug/Release CTest 45/45. Red, each nominated color, color after last red, each final-color cursor, finished state, read-only behavior and transactional invalid/unsupported FreeBall cases tested. No first-contact judgement or scoring processing claimed.
- New DONE task IDs in this continuation: DAT-007, DAT-008, PHY-016, PHY-017, RUL-001, RUL-003, RUL-004 (seven IDs). Prior count 21; current total 28 at that checkpoint. Subsequent PHY-019 and RUL-004 status updates make the current exact DONE count 29; passing-test count is not task count.
- Two initial validation failures retained: geometry training fixture/material height mismatch (retry 1 passed); cushion side-spin test expected the wrong friction sign (analytic correction, retry 1 passed). Transition normal signs were additionally corrected during review and retested before DAT-007 completion. No path reached four failures.
- Next physics work: PHY-018 pocket continuous geometry detection, PHY-019 pot/ejection state and schema extension before they affect snapshots. RUL-002 needs actual pot-state outputs; no made-up physics result is used to unlock scoring. RUL-005/006/007 etc. remain TODO. UE-dependent acceptance remains outstanding.


- PHY-019 initial p19: Debug CTest 46/47. Test incorrectly supplied repeated jaw contact after entering InPocket; retry 1 changes only that call to inside-pocket observation with no contact. Original failure retained.
- PHY-019 retry 1 revealed repeated `HasReturnedToCloth` on an already Ejected state emitted Recovery repeatedly. Fixed transition event edge and queued retry 2; state behavior, not test expectation, changed. Previous logs retained.

- PHY-018/019: Test-Current.ps1 -Run p19r2, exit 0; Debug/Release CTest 47/47. Pocket contact passed finite jaw/rim/slope/bottom sweeps, high speed, finite misses, overlap and verified rebound. Pocket state then passed AtMouth/InPocket/Potted/Ejected transitions, once-only event edge, wrong-pocket/invalid/potted recovery rejection. PHY-018 remains REVIEW because complete integration into the physics service and replay/state schema is not done; PHY-019 DONE is the standalone state machine only. Initial p18 timing expectation and p19 state edge failures are retained in the failure ledger; each fixed on first retry.


## 无 UE5 执行计划（2026-11-12）

背景：公司内网暂时无法安装 UE5。本节规划不安装 UE5 时可以完整完成的 36 项任务。完成后目标完成数为 65/121。原有英文记录保留不动。

### 第 0 步：版本基线

- 本仓库局部提交身份：nh511 / 219069329+nh511@users.noreply.github.com（未改全局配置）。
- 初始提交 3ee68cb（187 个文件），已推送到 https://github.com/nh511/Snooke.git 的 main 分支。build/ 和 build-evidence/ 不上传。
- FND-004 证据：推送后 GitHub 连接超时（github.com:443 TCP 不通），改为从同一提交 3ee68cb 本地克隆到 build-evidence/fnd004-clone/repo，运行 Verify-Standalone.ps1 -Run fnd004，exit 0，Debug/Release CTest 47/47。FND-004 仍为 REVIEW：UE 项目文件生成和插件还原尚未验证，远程克隆需要网络恢复后补测。

### 执行顺序

阶段 1 物理闭环：PHY-018（收尾）→ PHY-020 → PHY-021

阶段 2 回放和验证：REP-001 → VAL-003 → VAL-004 → VAL-009 → REP-002 → VAL-005 → VAL-006 → VAL-007 → VAL-008 → COR-006 → REP-003

阶段 3 规则：RUL-002 → RUL-005 → RUL-006 → RUL-007 → RUL-008 → RUL-009 → RUL-010 → RUL-011 → RUL-012 → REP-004 → RUL-015 → RUL-013 → RUL-014 → REP-005

阶段 4 稳定性：REL-004 → REL-002 → REL-006

阶段 5 AI：AIC-001 → AIC-002 → AIC-003 → AIC-004 → AIC-005

### 本轮不做

- 只能部分完成（以后做，届时最多到 REVIEW）：FND-008、VAL-001、VAL-002、REL-001、VAL-010。
- 依赖 UE5：FND-002、FND-003、COR-007 至 COR-010、REL-003、REL-005，以及 INT、CHA、PRE 全部 33 项。
- 已 REVIEW、只差 UE 部分：FND-004、FND-006、FND-007、COR-004、DAT-001 至 DAT-006。

### 关键设计约定

- 物理参数包 snooker-physics-bundle-v1：内嵌球桌、库边、袋口、球碰球、布面、球杆、重力、步长、几何设置和服务限值；参数包版本即快照 PhysicsParameterVersion。
- 六袋工程样例桌 data/table-six-pocket.snooker：4 角袋 + 2 中袋，闭合边界；工程样例，不是实测校准。
- 快照 v3：每球增加落袋状态；v2 快照读入时所有球视为在台面上。
- 整杆模拟：固定步内按时间顺序处理球碰球、碰墙、越过袋口线和袋内接触；同时事件（时间差 ≤1e-12 s）按固定顺序处理；只有碰到袋底才发 Potted，且只发一次。
- 故障码：NonFinite、Penetration、VelocityExplosion、Timeout、EventLoop、LeftTable、PocketEscape、SolverError；故障中断模拟并写出可重放故障文件。
- 规则：比赛状态 v2；规则参数 snooker-rules-v1（match/practice）；最低罚分 4。

### Miss 判罚策略（用户选定：有直线可击才判）

比赛模式下同时满足以下三条才判 Miss：犯规原因是没有先碰到目标球；击球前至少有一个目标球两侧都能直线击到；击球方不需要对方做斯诺克就能追分。练习模式不判 Miss。判 Miss 后对方可选择从当前位置打，或让犯规方恢复到击球前快照重打。不实现三次 Miss 判负。

### 执行和提交规则

- 每个任务：标记 IN_PROGRESS → 实现和测试 → Test-Current.ps1 -Run <任务号>-a<次数> 的 Debug/Release CTest 全部通过 → 更新状态和中文证据 → 本地提交 "<任务号>: <中文摘要>"。
- 每个阶段结束：Verify-Standalone.ps1 -Run stage<N> 干净副本验证，然后 git push origin main。
- 失败处理：找到根本原因再修；不靠放宽容差或修改期望值过测，除非有解析依据并写入失败台账。同一任务第一次失败后最多重试 3 次，仍失败则标记 BLOCKED、提交并推送已完成部分、停止并报告。推送失败按同样规则计数。
- 禁止：强制推送、修改全局 Git 配置、提交 build/ 或 build-evidence/。
- 所有参数都是工程值，基线是回归基线，不是实测结果。Debug 与 Release 结果差超过 1e-9 视为缺陷。

### 本计划执行记录

（按任务追加）

### PHY-018 收尾记录（2026-11-16）

- 状态：DONE（独立核心范围）；UE5 适配和完整多球物理服务不在本任务验收内。
- 产物：`PhysicsSnapshot` schema v3（每球 `PocketStateData`、袋口 ID、一次性落袋标记和事件游标）；v2 快照可迁移为全 `OnCloth` v3；`ClothProfile` 的 `snooker-cloth-v1` 文件格式；`snooker-physics-bundle-v1` 可复现参数包；六袋工程桌 `data/table-six-pocket.snooker`；袋口单球模拟服务 `PocketSimulation`。
- 行为验收：袋角接触产生反弹且不触发落袋；进入袋底后只生成一次 `Potted`；袋内中途序列化、恢复和不中断运行的状态/事件一致；从袋内越过袋口可恢复到台面；非法快照和非法服务恢复保持目标对象不变。
- 证据：`tools/Test-Current.ps1 -Run phy018-close-final`，Debug/Release CTest 各 50/50；包括六袋几何和物理 bundle 逐字节再生成比较。证据目录：`build-evidence/phy018-close-final/`。
- 边界：本次服务是袋口单球实验服务，不负责球碰球、库边和多球事件全局调度；这些保持 PHY-020/PHY-021 的后续工作。
- 重试记录：`phy018-close-a2` 首次加入六袋数据时失败，原因是布面文件测试参数路径和旧几何测试假设；`phy018-close-a3` 修复后因证据目录复用未执行，随后用 `phy018-close-a4` 完成验证；新增 bundle 后最终验证通过。没有达到第四次代码失败。

### PHY-020 恢复与验收（2026-11-18）

- 用户明确要求继续后恢复任务。本轮 `phy020-resume-a0/a1` 分别暴露错误的世界坐标法线判据和回台球仍跨布边时的出界误报；`a2` Debug/Release 51/51；新增独立进程重放后 `a3` Debug/Release 54/54，全部阶段 exit 0。
- 袋角轮廓在 z=0，球心 z=r 时水平掠过可能只是切触，不能据此要求切向速度反向。恢复 z=r/2 的实际袋口迎面接触夹具，逐事件断言 vn_before<0、vn_after>0 和 vn_after=-e*vn_before（1e-10），不是删除反弹验收或放宽容差。AtMouth 不是布面支撑态；OnCloth/Ejected 出界按球心判断，刚回台球体可跨边界。
- 修复故障测试辅助 lambda 未将失败传播至 main 的缺陷；之前仅凭旧专项测试 PASS 不能证明全部异常路径通过。现在八种故障码检查与序列化均由 CHECK 传播，测试还覆盖 NaN/Inf 诊断、球间穿透、失败步回滚、故障后拒绝继续、事件历史不误占下一个步的预算。
- 故障格式升级 snooker-fault-v2，仍可读取 v1。有效故障前检查点、原始坏状态诊断、嵌入参数包分开保存；不再把修补坏值后的状态称作可复现输入。无有效输入的 NaN/Inf 拒绝只提供诊断，不承诺通过文本重新注入 NaN。
- snooker_fault_replay 分别从 physics-fault.txt、physics-timeout.txt、physics-eventloop.txt 独立进程读取并重算，同一故障码、步号和完整故障记录逐字节相同。自动测试通过，不依赖 UE。
- 本任务不等于 REL-004 的完整故障包收集系统，也不等于 PHY-021 的多球调度验收。证据保留于 build-evidence/phy020-resume-a3/。

### PHY-021 实现和验收（2026-11-18）

- 新增 ISnookerPhysicsService/CreatePhysicsService 和 snooker_simulate；BeginShot、Step、IsSettled、GetEvents、SaveSnapshot、RestoreSnapshot 可完成独立整杆计算。球对、库边、袋角、越口和袋内事件按最早时刻重建查询，状态按 ID 规范化；稳定事件游标跟随快照。
- 修正六袋工程桌为 6 条库边；参数包 v2 增加 RequireClosedBoundary。生成器连接真实库端与袋角外端，以虚拟袋口边做拓扑检查，拒绝断开、多回路、自交轮廓；虚拟袋口边不参与碰撞。服务拒绝未开启闭合验证的参数包。
- 台面采用不跳球模型：碰球/碰库产生的竖直速度由支撑吸收；库面和袋角按二维有限墙做 CCD，袋内仍为有限三维接触和分步重力。不是精确加速度 CCD，不是实测校准。
- phy021-a0 发现碰球切向冲量产生下向速度；a1 补台面约束后双配置 55/55。a2 加闭环、事件序列/FPS 对比通过；a3 的 CLI 比较发现 22 个 CR 字节差异，归因 Windows 文本模式，改二进制 LF 后 phy021-final 双配置 56/56。phy021-pot 又补真实 22 球训练局面的碰球→进袋→球停，双配置 56/56。
- 22 球开局中途恢复的后续事件及终态逐字节相同；30/60/120/144 FPS 事件和终态相同。高速 3 球连续碰撞同一步正确排序；带旋转两库、袋内捕获、失败输入状态不变和终局去重通过。独立进程 CLI 与 API 终态逐字节比较通过。
- 跳过完全无相对平移的球对及零平移球的墙/袋口查询；移动球撞静止球仍检测。Debug 核心服务测试由约 231 s 降到约 29 s，CLI 约 14 s；未改变物理阈值。
- 完整 record.shot/JSON/CSV 属 REP-001/VAL-003，当前简版 events.txt 不能替代完整单杆记录。
- 阶段 1 干净副本验证：Verify-Standalone.ps1 -Run stage1-n18，configure/Debug build/Debug test/Release build/Release test 全部 exit 0，双配置各 56/56，无跳过；本地最终 phy021-pot 亦双配置 56/56。Debug/Release 开局 final.snapshot 和 events.txt 的 SHA-256 分别一致。
- 当前 DONE=32/121、REVIEW=11、BLOCKED=2（UE）、TODO=76。阶段 1 已完成，准备提交并按授权推送；尚无 UE 安装/编译或实测校准结论。

### REP-001 验收（2026-11-18）

- PhysicsShotRecord/snooker-shot-v2 内嵌真实 PhysicsBundle v2、初始快照 v3、ShotInput、逐事件完整数据、终态和 Settled/fault 结果。旧 ShotRecord v1 保留，不静默升级成可回放记录。
- snooker_simulate 写出 record.shot；独立 CLI 的真实 22 球记录逐字节往返，缺块、未知版本、乱序事件、球/库/袋引用、负冲量、球属性变化和游标不一致均拒绝，失败不改目标。属于格式验收，重新运行物理与结果比对仍属 REP-002。
- Test-Current.ps1 -Run rep001-a0，Debug/Release 56/56（格式测试在 physics_service_cli 中调用），全部 exit 0。阶段 1 已推送 c073b11；阶段 2 按计划结束时推送。

### VAL-003 验收（2026-11-18）

- snooker_simulate 生成 record.shot、trajectory.csv、result.json、viewer.json、events.txt 和 final.snapshot。CSV 每固定步采样所有球，JSON 内嵌参数包、初终完整快照、输入、事件、库边和袋口几何。
- Test-Current.ps1 -Run val003-a1，双配置 56/56；Node 独立 JSON.parse/CSV 解析器检查行数、步号、ID、时间、事件各字段、终态和快照一致。22 球开局得到 233244 行、13 事件；完整轨迹约 23.5 MB，仅在 build 下，不提交为基线。
- 配置阶段必须找到 node，否则失败，不跳过导出验收。输出目录必须为空，错误运行不覆盖上一轮成功结果。val003-a0 是补丁传递失败后的旧版回归，不作为本任务验收；未删历史证据。短杆基线限制为每文件小于 1 MiB。

### VAL-004 验收（2026-11-18）

- 新建 data/baselines/center，真实 Release 中杆输入 0.2 m/s：475 步/1.9791666666666667 s 后静止，476 行 CSV、3 事件，终点 x=0.14873083962918993 m，CSV 57685 字节。参数包与输入/快照/完整记录一并关联，不是实测值。
- tools/compare.cjs 按步序逐字段比较，连续浮点容差可选，离散字段精确比较；首次偏差报告 step、ball、field、expected、actual。baseline_center 重算基线要求 1e-9 内一致，另将杆速增加 1e-6，要求 FAIL 且给出第一处差异。
- Test-Current.ps1 -Run val004-a0，Debug/Release 57/57，全部 exit 0。新建基线原因：统一服务首次中杆短程回归；后续更新必须说明原因，不用生成值替换失败期望。

### VAL-009 验收（2026-11-18）

- tools/compare.cjs 支持 CSV 或结果目录。目录比较先逐步轨迹，再参数/事件/终态 JSON；连续值容差可选，ID/步号/序号/状态精确比较，默认容差 0。首差异报告字段与数值，轨迹/事件还带步号和球。
- comparator_contract 人为扰动轨迹 x、事件时间/序号、速度和袋口状态，要求 FAIL；同输入重复运行目录要求零容差 PASS。Test-Current.ps1 -Run val009-a0，双配置 58/58，全部 exit 0。
- 交叉比较实际 Debug/Release 目录：中心短杆 476 行/3 事件、22 球开局 233244 行/13 事件，轨迹+事件+终态默认容差 0 均 PASS。本机/本工具链结论，不宣称跨平台逐位确定性。

### REP-002 验收（2026-11-18）

- ReplayShot/snooker_replay 从记录实际参数、快照、输入重新模拟，每次生成事件立即与记录比较，最终快照逐字节比较，不把已记录事件或终态写进物理状态。构建版本不支持时明确拒绝。
- Test-Current.ps1 -Run rep002-a0，Debug/Release 59/59。中心记录重算结果相同；改冲量/终点/构建版本则失败，原输出不变；真实 CLI 22 球开局 record.shot 在独立进程中重放通过。
- 当前范围：本机同求解器构建的确定性重放；未宣称跨平台一致。未来增加的高低杆/库边/袋口基线须继续纳入重放测试。旧 v1 不具备完整参数，不冒充可重放 v2。

### VAL-005 验收（2026-11-18）

- follow/draw 两组固定基线使用相同球桌和球杆，杆速 0.6 m/s，只改变 Z 打点 ±0.5r。目标球相距 0.075 m；高杆首次接触第 11 步后母球正向跟进，低杆第 13 步后母球出现负向速度，独立 Node 断言不是只看终点。
- 高杆移动中滑滚转换第 5 步，停止第 902 步/3.7583333333333333 s；低杆转换第 24 步，停止第 736 步/3.0666666666666664 s。CSV 每步包含全部速度/角速度，最终均严格零速度且睡眠。
- 两组 CSV 约 204 KB/165 KB，均小于 1 MiB，附初始快照、输入、完整物理记录。Test-Current.ps1 -Run val005-a0，Debug/Release 61/61，包含逐字段基线比较和独立物理重放。参数仍是工程样例。

### VAL-006 验收（2026-11-18）

- 左右塞杆速 0.4 m/s、Y 偏移 ±0.2r，初始球位在对称位置，经真实库边冲量产生非零横向速度。极向量反射 Y，轴向量反射 X/Z；2587 个逐步样本的镜像误差不超过 1e-9。
- 两组均第 2586 步/10.775 s 停止，轨迹约 351 KB/353 KB。Test-Current.ps1 -Run val006-a0，Debug/Release 62/62，含独立物理重放、逐步固定基线和镜像断言，不允许两组都无偏移来蒙混过关。

### VAL-007 验收（2026-11-18）

- 一库、两库、贴库分别建立完整记录和 CSV；旋转碰库复用 left 基线。每组按 30/60/120/144 FPS 重算，完整事件与终态逐字节相同，另做逐步容差 1e-9 基线和独立重放。贴库首次响应时间要求 ≤1e-12 s。
- 一库停止第 361 步、贴库第 557 步、两库第 3725 步且至少两次真实碰库；CSV 分别约 61/99/536 KB，均小于 1 MiB。首次两库夹具路线太长只碰一库，保留失败后改为右上区域两库路线，不降低事件数要求。
- 两库重试 1 发现斜面/袋边同时接触误报 stale pocket response contact，故障保留 build/two-rail-r1-generated/fault.txt。原因是响应重查任意最早接触替换了原片元；改为按原 Primitive 重验，保持点/法线/特征/穿透校验，伪造片元号仍拒绝。把故障步 206 检查点加入服务回归，不以改路线掩盖问题。
- Test-Current.ps1 -Run val007-final，最新相同源码 Debug/Release 66/66，全部 exit 0。既有全部基线未重生成且保持通过；val007-a2 构建期间补过测试，只作为中间证据。

### VAL-008 验收（2026-11-18）

- data/baselines/pocket 固定五组单球袋口实验输入、最终快照和完整事件。entry/capture 均恰好一次 Potted；jaw/mouth-stop/recovery 均零 Potted，jaw 按恢复系数验证真实反弹，recovery 恰好一次回台，停口保持静止无事件。
- 每组在一半步数处序列化再恢复，逐步快照和后续事件与连续运行逐字节相同；全套文件约 116 KB。正入实验经历多次斜面弹跳，只有袋底捕获才记落袋，不按入口触发计分。
- Test-Current.ps1 -Run val008-a0，Debug/Release 67/67，全部 exit 0。本任务是独立袋口核心实验，不冒充 UI/规则计分/实测校准；统一服务的 22 球进袋已在 PHY-021 验收。

### COR-006 多球查看器验收（2026-11-18）

- tools/shot-viewer.html 直接本地打开，读取 result.json+trajectory.csv，显示真实多球、轨迹、库/袋几何、接触点、事件列表、按事件跳转、步进、播放和调试开关；旧 trajectory.html 保留单球功能并链接新入口。所有文本用 textContent，不执行外部文件脚本。
- Chrome 原生 CDP 自动检查真实 22 球 10602 帧、13 事件加载，首次碰撞跳转、末步/前后步/播放，桌面 1280x800 和手机 390x844 无横向溢出；采样 1116 种画布颜色，无 JS 异常。证据 build-evidence/shot-viewer-1795001048505/{desktop.png,mobile.png,result.json}，截图已检查。
- Test-Current.ps1 -Run cor006-a0，Debug/Release 67/67；独立 Node 检查多球解析和坏数据拒绝。浏览器采用已有本机 Chrome/CDP，不宣称使用了未安装的 Playwright，也不需要下载在线资源。

### REP-003 验收（2026-11-18）

- ReplayController 先实际重算并校验记录，每 120 步保留求解器检查点，跳转至最近前置检查点后重算，不保存所有步/所有球的全量常驻缓存。SeekStep/SeekTime/SeekEvent/SeekFault 提供只读快照，时间定位为首个不早于目标时间的完成物理步。
- 测试逐个步号与连续模拟序列化快照一致、逆序事件跳转、首个真实碰撞、首个真实落袋，以及超时故障的真实故障前检查点。非法步/事件/NaN 时间或坏记录不会改变当前状态。
- Test-Current.ps1 -Run rep003-a0，Debug/Release 68/68，全部 exit 0。阶段 2 的 11 项已完成；准备干净副本验证和阶段推送。完成数 43/121（REVIEW=10，BLOCKED=2，TODO=66）。

### RUL-002 验收（2026-11-18）

- ShotPhysicsResult/BuildShotRuleInput 显式接收 BallDefinition，核对 ID/球类型/半径/质量/材质；不从 ID 或 Actor 猜球类型。只读投影物理事件得到母球首次碰球、顺序落袋列表和母球落袋标记，检查事件游标、状态转移和最终静止。
- 真实高杆碰撞记录、真实服务进红球验证；重复元数据、无母球、篡改 FirstContact/Potted 缓存、错误事件序号等拒绝，失败不修改输出。故障模拟不得计分。源码扫描无 Unreal 头文件。
- Test-Current.ps1 -Run rul002-a0，Debug/Release 69/69，全部 exit 0。它只提供规则输入，不提前宣称犯规/计分/回合已完成。

### RUL-005 验收（2026-11-19）

- MatchState 升级为 v2，增加 NominationPending、FreeBallNominated、FreeBallType、RespottedBlack；v1 文本可读取并迁移。ColorAfterRed 必须先 NominateColor，红阶段正常红球得 1 分并转入待点名彩，末红后彩球按分值计分并生成复位列表。
- EvaluateShot 只处理正常路径：要求物理事件输入已校验、首次接触等于 Ball On、母球未落袋、无犯规/FreeBall/最终彩球未实现状态；分数溢出、错误点名、错误阶段和状态输入事务性拒绝。
- rules_scoring 既有显式事件测试，也用真实服务生成的碰球/落袋记录检查红球加分。首轮失败仅为连续彩球夹具在进入 FinalColors 后仍使用红后彩路径；修正为每个彩球独立的 ColorAfterRed 状态后，Test-Current.ps1 -Run rul005-a1 Debug/Release 70/70 全部通过。

### RUL-006 验收（2026-11-19）

- EvaluateCueBallPotted 从 ShotPhysicsResult 只读生成 Foul/CueBallInHand/SuppressObjectScoring 和同杆目标球列表，不计算 RUL-008 罚分，也不把母球当目标球得分。输入必须是真实一致的 Potted 事件/终态；缓存矛盾被拒绝。
- PlaceCueBallInD 使用 TableDefinition 和显式 DLineX=-1.04、DCenterY=0、DRadius=0.292；检查半径高度、布面边界、D 区和与已占球的重叠。目标类型错误、越界和重叠均拒绝。
- Test-Current.ps1 -Run rul006-a2，Debug/Release 71/71。a0/a1 的失败是测试夹具把母球 Potted 列表顺序/缓存标记写矛盾，修正为真实事件顺序后通过；实现没有放宽物理或规则校验。

### RUL-007 验收（2026-11-19）

- FirstContactResult/EvaluateFirstContact 只读取 ShotPhysicsResult 和 MatchState：首次 BallBall 事件涉及母球时取另一球，按显式 BallDefinition 与当前 BallOn 匹配；无首次接触为 NoContact，类型不匹配为 WrongBall，二者均标为犯规候选。
- 测试覆盖红阶段合法/错误/未接触、ColorAfterRed 彩球、未知球引用、进行中和 Finished 状态拒绝，失败不改变已有结果。该任务不计算罚分、不切换回合。
- Test-Current.ps1 -Run rul007-a1，Debug/Release 72/72。a0 仅为测试错误使用不存在的 ShotPhysicsResult::Serialize，修正为保存结果字段后通过。

### RUL-008 验收（2026-11-19）

- FoulValueResult/CalculateFoulValue 输出原因、罚分、对手玩家和相关最高球分值。罚分为 `max(4, relevant value)`，相关值来自错误首次接触、母球落袋或非法目标球进袋；合法正常杆返回无犯规/零字段。
- 覆盖无首次接触最低 4、错碰黑球 7、母球进袋最低 4、对手归属、Free Ball 尚未实现时明确拒绝、非法事件缓存和状态校验。RUL-008 不修改 MatchState，不实现回合切换或 Miss。
- rul008-a0/a1/a2 的失败均是测试夹具：母球定义重复、未接触事件游标未同步、事件时间越过终态；最终将夹具改为合法事件序列后 `rul008-final` Debug/Release 73/73 全部通过。
