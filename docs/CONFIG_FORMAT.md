# Core Configuration v2 (COR-004 / DAT-006)

Shared, engine-free UTF-8 text input: data/default.snooker. Header snooker-config-v2 is the schema version; the next token is the immutable physical parameter version. V1 is rejected explicitly because it omitted cue speed/offset/restitution/version fields. Values use the classic locale decimal point and SI units. Tokens are whitespace-separated; sections appear in the documented order. Unknown versions, missing fields, trailing data, duplicate ball IDs, nonfinite/out-of-range values and more than 4096 balls are rejected transactionally.

Sections and fields:

- table: cloth length and width (m), centered XY rectangle. Detailed cushion/pocket geometry belongs to DAT-002/DAT-007, not this material configuration schema.
- cloth: sliding coefficient, rolling resistance coefficient, vertical spin deceleration in rad/s^2 (engineering model, calibration pending).
- cushion: restitution [0,1], tangential friction, contact height (m).
- pocket: opening width, jaw radius, bottom depth (m).
- cue: profile ID, length (m), mass (kg), tip radius (m), max pullback (m), max impulse (N s), cue parameter version, max forward speed (m/s), max offset/radius ratio [0,1), tip restitution [0,1].
- solver: fixed step (s), bounded positive iteration count, sleep linear speed (m/s), angular speed (rad/s), sustained duration (s).
- balls: count, followed by count ball records (ID, type, initial XYZ center in m, radius in m, mass in kg, material token).
- end: explicit terminator; only whitespace may follow.

All defaults are engineering examples, NOT calibrated measurements or a claim of exact snooker table/pocket geometry. Model field semantics must be versioned when changed. This common C++ loader can be called by CLI or UE adapter without copying its implementation; actual UE DataAsset import and UE compilation remain unverified.
