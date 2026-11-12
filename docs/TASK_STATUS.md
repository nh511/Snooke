# Task Status

Authority: DEVELOPMENT_PLAN.md and EXECUTABLE_TASKS.md (UE5 + standalone C++17).
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
| COR-006 | REVIEW | Single-ball CSV/viewer verified in real Chrome; cushion/pocket/collision event display and full shot timeline remain incomplete |
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
| PHY-018 | REVIEW | Finite jaw/rim/slope/bottom swept contact and rebound passed; complete pot/state acceptance in PHY-019 |
| PHY-019 | DONE | Pocket state transitions, once-only pot event, potted no-eject and explicit recovery event passed |

## Execution Rules

- One task at a time: IN_PROGRESS, REVIEW, then DONE only after all applicable acceptance criteria pass.
- UE-dependent acceptance is recorded separately and is never replaced by standalone tests.
- Initial failure plus at most three retries; stop after the fourth failure, record BLOCKED and notify the user.
- No automatic commits, identity changes, skipped tests, or mocked physics acceptance.

## Current Toolchain

CMake 4.4.4, VS2022 Build Tools, MSVC 19.44, Windows SDK 10.0.26100.0, Git and Git LFS.
UE5 is not installed/validated; no UE compilation or asset import is claimed.

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

