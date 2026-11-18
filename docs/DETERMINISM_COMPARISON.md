# Determinism Comparison

Run `node tools/compare.cjs expected.csv actual.csv [tolerance]` or pass two
simulation output directories. Default tolerance is zero. Directory mode checks
all CSV samples, then structured result.json fields including loaded bundle,
input, events and final balls. Embedded serialized snapshots are redundant with
structured fields and are excluded from tolerant JSON comparison.

Exit 0 means PASS, 1 means a data difference, 2 means invalid input/usage.
The first trajectory difference includes step, ball, field, expected and actual.
Event differences include event index and step/ball. Discrete state fields and
IDs remain exact regardless of numeric tolerance. Parameter bytes remain exact.

Tests explicitly corrupt state/event fields, verify exact/tolerant behavior and
rerun a short shot twice. Debug/Release outputs are additionally cross-compared.
These checks do not establish determinism on an untested platform/toolchain.
