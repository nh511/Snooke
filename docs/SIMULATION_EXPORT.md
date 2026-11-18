# Standalone Simulation Export

snooker_simulate takes bundle, initial snapshot, shot-input file and empty output
directory. Successful output includes record.shot, result.json, viewer.json,
trajectory.csv, events.txt and final.snapshot. Failure returns nonzero, possibly
leaving a partial diagnostic CSV and fault.txt, never successful result.json.

CSV rows are ordered by fixed step then ball ID, include every ball and carry
position, linear/angular velocity, sleep/pocket flags and event cursor. The first
sample is post-cue; pre-strike snapshot/input are in JSON and record.shot.
JSON includes loaded parameter bytes/version, input, initial/final serialized and
structured snapshots, all events and geometry. viewer.json currently contains
the same metadata, with playback samples in the separate trajectory.csv.
Output uses classic locale, max_digits10 precision and binary/LF writes.

CMake requires Node. The CLI fixture independently parses CSV and JSON, verifies
all event fields and final numeric state against events.txt/final.snapshot.
These are regression data, not measured physical truth. Short committed baseline
trajectories must remain under 1 MiB; full opening exports stay in build outputs.
