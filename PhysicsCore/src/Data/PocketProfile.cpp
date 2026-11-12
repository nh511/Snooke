#include <Snooker/Data/PocketProfile.h>
#include <iomanip>
#include <locale>
#include <sstream>

namespace Snooker
{
namespace
{
bool Fail(std::string* Error,const char* Message) { if(Error) *Error=Message; return false; }
bool Token(const std::string& Value) { return !Value.empty() && Value.find_first_of(" \t\r\n")==std::string::npos; }
}
bool PocketProfile::IsValid(std::string* Error) const
{
    if(!Token(Id) || !Token(ParameterVersion)) return Fail(Error,"invalid pocket id or version");
    if(!IsFinite(OpeningWidth) || OpeningWidth<=0 || !IsFinite(JawRadius) || JawRadius<=0 ||
       !IsFinite(JawAngle) || std::abs(JawAngle)>Pi/2 || !IsFinite(BottomDepth) || BottomDepth<=0 ||
       !IsFinite(BottomSlope) || BottomSlope<0 || BottomSlope>=Pi/2 ||
       !IsFinite(CaptureDepth) || CaptureDepth<=0 || CaptureDepth>BottomDepth ||
       !IsFinite(JawRestitution) || JawRestitution<0 || JawRestitution>1 ||
       !IsFinite(EjectionSpeedThreshold) || EjectionSpeedThreshold<0)
        return Fail(Error,"invalid pocket geometry, capture or ejection parameters");
    return true;
}
std::string PocketProfile::Serialize() const
{
    std::ostringstream s; s.imbue(std::locale::classic());
    s<<std::setprecision(std::numeric_limits<Scalar>::max_digits10)
     <<"snooker-pocket-v1 "<<Id<<' '<<ParameterVersion<<' '<<OpeningWidth<<' '<<JawRadius<<' '<<JawAngle<<' '
     <<BottomDepth<<' '<<BottomSlope<<' '<<CaptureDepth<<' '<<JawRestitution<<' '<<EjectionSpeedThreshold;
    return s.str();
}
bool PocketProfile::Deserialize(const std::string& Text,PocketProfile& Output,std::string* Error)
{
    std::istringstream s(Text); s.imbue(std::locale::classic()); PocketProfile p; std::string magic;
    if(!(s>>magic>>p.Id>>p.ParameterVersion>>p.OpeningWidth>>p.JawRadius>>p.JawAngle>>p.BottomDepth
          >>p.BottomSlope>>p.CaptureDepth>>p.JawRestitution>>p.EjectionSpeedThreshold) || magic!="snooker-pocket-v1")
        return Fail(Error,"invalid pocket schema or fields");
    s>>std::ws;
    if(!s.eof()) return Fail(Error,"unexpected data after pocket profile");
    if(!p.IsValid(Error)) return false;
    Output=std::move(p); return true;
}
}
