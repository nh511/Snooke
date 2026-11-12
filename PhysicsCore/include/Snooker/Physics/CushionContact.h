#pragma once
#include <Snooker/Table/TableGeometry.h>
#include <Snooker/Physics/BallState.h>

namespace Snooker
{
struct CushionContact
{
    TableFeature Feature=TableFeature::ClothBoundary;
    std::uint32_t OwnerId=0;
    std::size_t SegmentIndex=0;
    Vec3d Point, Normal; // Point on finite contour; normal towards ball center.
    Meters Gap=0; // Center-to-contour distance minus radius; negative = penetration.
    std::string ProfileId, ProfileVersion;
    bool DiagnosticOnly=true;
};
// Current-position query, not CCD. Top refers to the finite upper contour edge, not a top face.
// Pocket jaws are intentionally excluded and belong to PHY-018.
bool FindCushionContacts(const BallState& Ball,const TableGeometry& Geometry,Meters ContactTolerance,
                         std::vector<CushionContact>& Output,std::string* Error=nullptr);
}
