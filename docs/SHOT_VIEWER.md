# Multi-Ball Shot Inspector

Open tools/shot-viewer.html directly and select result.json plus trajectory.csv
from the same successful simulation output. Files remain local. The inspector
shows finite table geometry, all unpotted balls, paths, contacts, event timeline,
step controls and playback. Top and projected world views share the same imported
data. Velocity vectors, angular-velocity vectors, contact normals and sleeping
state highlighting can be toggled independently. Selecting an event jumps to its completed fixed step.
The original single-ball viewer remains at tools/trajectory.html.

Parsing is transactional and validates ordered frames/IDs, geometry, numeric
states, event bounds and final-state consistency. Large file/resource limits
prevent unbounded input work. Canvas motion comes exclusively from real CSV data.

Run node tools/verify-shot-viewer.cjs result-directory for isolated headless Chrome
CDP checks and desktop/mobile screenshots. The helper closes only its own browser
process/profile; it does not depend on Playwright or a network development server.
