# Color Respot

FindColorRespot places a red-phase color on its own spot when clear. If occupied,
it searches explicit table spots in descending value order: black, pink, blue,
brown, green, yellow. All occupied returns NoSpotAvailable; no occupied ball is
moved and no undocumented nearest longitudinal position is invented.

FindColorRespots resolves multiple colors high-value first and updates a local
occupied set so results are deterministic. Output is transactional. Applying the
returned definitions to a physics snapshot is a caller responsibility.
