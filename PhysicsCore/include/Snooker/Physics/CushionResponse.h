#pragma once
#include <Snooker/Physics/CushionContact.h>

namespace Snooker
{
struct CushionResponse
{
    bool Applied=false;
    Scalar NormalImpulse=0, EnergyBefore=0, EnergyAfter=0;
    Vec3d TangentialImpulse;
    CushionContact Contact;
};
// Requeries the actual contour to reject stale/forged contacts. One contact only;
// resolving simultaneous contacts/overlap correction is a future service responsibility.
bool ResolveCushionContact(BallState& Ball,const TableGeometry& Geometry,const CushionContact& Contact,
    Meters ContactTolerance,CushionResponse& Output,std::string* Error=nullptr);
}
