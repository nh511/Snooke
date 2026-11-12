#pragma once
#include <Snooker/Data/TableDefinition.h>
#include <Snooker/Data/CushionProfile.h>
#include <Snooker/Data/PocketProfile.h>

namespace Snooker
{
enum class TableFeature { ClothBoundary, CushionFace, CushionTop, PocketTransition, PocketJaw };
struct GeometrySegment
{
    TableFeature Feature=TableFeature::ClothBoundary;
    std::uint32_t OwnerId=0;
    Vec3d Start, End, InwardNormal;
    std::string ProfileId, ProfileVersion;
};
struct PocketGeometry
{
    PocketDefinition Definition;
    PocketProfile Parameters;
    Vec3d Inward, Tangent, MouthStart, MouthEnd, BottomCenter;
    std::vector<Vec3d> LeftJaw, RightJaw;
    // Finite sloped floor strip from mouth to pocket bottom, four corners.
    std::vector<Vec3d> Floor;
    // Bottom support extends outward by twice the configured jaw radius.
    std::vector<Vec3d> Bottom;
};
struct TableGeometry
{
    std::string ParameterVersion;
    TableBounds Table, Cloth;
    std::vector<GeometrySegment> Segments;
    std::vector<PocketGeometry> Pockets;
    std::vector<CushionProfile> CushionMaterials;
    std::vector<PocketProfile> PocketMaterials;
};
struct GeometrySettings
{
    Meters CoordinateTolerance=1e-8;
    std::uint32_t JawSubdivisions=12;
};
// Table-v1 supports straight rails on axis-aligned cloth edges, with +Z up.
// Output is replaced only after complete validation. Inputs are not mutated.
bool GenerateTableGeometry(const TableDefinition& Table,const std::vector<CushionProfile>& Cushions,
    const std::vector<PocketProfile>& Pockets,const GeometrySettings& Settings,TableGeometry& Output,
    std::string* Error=nullptr);
}
