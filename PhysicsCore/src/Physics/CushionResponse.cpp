#include <Snooker/Physics/CushionResponse.h>
#include <Snooker/Physics/BallResponse.h>
#include <algorithm>

namespace Snooker
{
bool ResolveCushionContact(BallState& Ball,const TableGeometry& Geometry,const CushionContact& Contact,
    Meters ContactTolerance,CushionResponse& Output,std::string* Error)
{
    const auto fail=[&](const char* message){if(Error)*Error=message;return false;};
    std::vector<CushionContact> candidates;
    if(!FindCushionContacts(Ball,Geometry,ContactTolerance,candidates,Error))return false;
    const CushionContact* actual=nullptr;
    for(const auto& c:candidates)if(c.SegmentIndex==Contact.SegmentIndex)actual=&c;
    if(!actual || actual->Feature!=Contact.Feature || actual->OwnerId!=Contact.OwnerId ||
       actual->ProfileId!=Contact.ProfileId || actual->ProfileVersion!=Contact.ProfileVersion ||
       !actual->Point.NearlyEquals(Contact.Point,1e-12) || !actual->Normal.NearlyEquals(Contact.Normal,1e-12) ||
       !NearlyEqual(actual->Gap,Contact.Gap,1e-12) || actual->DiagnosticOnly!=Contact.DiagnosticOnly)
        return fail("stale or inconsistent cushion contact");
    if(actual->DiagnosticOnly)return fail("diagnostic cloth boundary is not a physical cushion");
    if(actual->Gap < -ContactTolerance)return fail("penetration exceeds response tolerance; correction required");
    const CushionProfile* material=nullptr;
    for(const auto& p:Geometry.CushionMaterials)if(p.Id==actual->ProfileId) {
        if(material)return fail("duplicate cushion material id");
        material=&p;
    }
    if(!material || !material->IsValid(Error) || material->ParameterVersion!=actual->ProfileVersion)
        return fail("unresolved or mismatched cushion material");
    CushionResponse r;r.Contact=*actual;r.EnergyBefore=BallKineticEnergy(Ball);r.EnergyAfter=r.EnergyBefore;
    const auto n=actual->Normal;const auto lever=n*(-Ball.RadiusMeters);
    const auto relative=Ball.LinearVelocityMetersPerSecond+Ball.AngularVelocityRadiansPerSecond.Cross(lever);
    const Scalar speed=relative.Dot(n),inertia=0.4*Ball.MassKg*Ball.RadiusMeters*Ball.RadiusMeters;
    if(!relative.IsFinite() || !IsFinite(speed) || !IsFinite(inertia) || inertia<=0 || !IsFinite(r.EnergyBefore))
        return fail("invalid cushion contact dynamics");
    if(speed>=0) {Output=r;return true;}
    r.NormalImpulse=-(1+material->Restitution)*speed*Ball.MassKg;
    const auto tangent=relative-n*speed;
    const Scalar tangentSpeed=std::hypot(tangent.X,tangent.Y,tangent.Z);
    const Scalar inverseTangentMass=1/Ball.MassKg+Ball.RadiusMeters*Ball.RadiusMeters/inertia;
    const Scalar frictionLimit=material->TangentialFriction*r.NormalImpulse;
    if(!IsFinite(tangentSpeed) || !IsFinite(inverseTangentMass) || !IsFinite(r.NormalImpulse) || !IsFinite(frictionLimit))
        return fail("cushion impulse overflow");
    if(tangentSpeed>0)r.TangentialImpulse=tangent*(-std::min(tangentSpeed/inverseTangentMass,frictionLimit)/tangentSpeed);
    const auto impulse=n*r.NormalImpulse+r.TangentialImpulse;
    auto candidate=Ball;
    candidate.LinearVelocityMetersPerSecond+=impulse*(1/Ball.MassKg);
    candidate.AngularVelocityRadiansPerSecond+=lever.Cross(impulse)*(1/inertia);
    candidate.Sleeping=false;candidate.SleepTimeSeconds=0;
    candidate.Contact={EContactKind::Cushion,0,n};
    if(!candidate.IsValid(Error))return false;
    r.EnergyAfter=BallKineticEnergy(candidate);
    if(!IsFinite(r.EnergyAfter) || r.EnergyAfter>r.EnergyBefore+1e-10*std::max(1.0,r.EnergyBefore))
        return fail("cushion response increased total energy");
    r.Applied=true;Ball=candidate;Output=r;return true;
}
}
