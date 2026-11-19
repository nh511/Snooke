# Weighted AI Debug Assertion Fix

MSVC Debug CRT asserts when `std::normal_distribution` receives sigma=0. Hard
difficulty intentionally has exact direction error zero, so `SelectWeightedShot`
now does not construct or sample a normal distribution for that profile. Easy and
Medium still use a positive sigma and deterministic seed. This removes the popup
without weakening the difficulty contract or changing physical evaluation.

The weighted AI executable must be cleanly rebuilt after this source change.
The fixed target passed the weighted test and full Debug/Release CTest 90/90.
