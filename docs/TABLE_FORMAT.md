# Table Description v1

Standalone TableDefinition is a data model, not DAT-007 geometry or a cushion/pocket solver. Table and cloth have separate XY bounding rectangles (m). Each cushion has an ID, start/end XYZ (m), and material profile token. Each pocket has an ID, center XYZ, opening/depth (m), and profile token. Named ball spots and initial BallDefinition placements are explicit records.

File order: snooker-table-v1 parameter-version, table minX minY maxX maxY, cloth bounds, cushions count and cushion records, pockets count and pocket records, spots count and spot records, placements count and ball records, end. See data/table-standard.snooker and table-training.snooker for the exact field order. All numeric output uses the classic locale and full double precision. Invalid or truncated input does not alter the output object; record counts are capped at 4096 as an input resource safeguard.

These two files are deliberately partial engineering fixtures, NOT regulation six-pocket geometry or a standard opening rack. Changing dimensions/records requires no source changes. Full contour generation, collision heights, pocket jaws and calibrated geometry remain DAT-007 and the related physics tasks. Real UTableDefinitionAsset import and matching UE results have not been tested.
