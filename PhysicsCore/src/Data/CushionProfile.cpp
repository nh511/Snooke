#include <Snooker/Data/CushionProfile.h>
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
bool CushionProfile::IsValid(std::string* Error) const
{
    if(!Token(Id) || !Token(ParameterVersion)) return Fail(Error,"invalid cushion id or version");
    if(!IsFinite(Restitution) || Restitution<0 || Restitution>1 ||
       !IsFinite(TangentialFriction) || TangentialFriction<0 ||
       !IsFinite(ContactHeight) || ContactHeight<=0 ||
       !IsFinite(PocketTransitionLength) || PocketTransitionLength<0 ||
       !IsFinite(PocketTransitionAngle) || std::abs(PocketTransitionAngle)>Pi/2)
        return Fail(Error,"invalid cushion parameter bounds");
    return true;
}
std::string CushionProfile::Serialize() const
{
    std::ostringstream s; s.imbue(std::locale::classic());
    s<<std::setprecision(std::numeric_limits<Scalar>::max_digits10)
     <<"snooker-cushion-v1 "<<Id<<' '<<ParameterVersion<<' '<<Restitution<<' '<<TangentialFriction<<' '
     <<ContactHeight<<' '<<PocketTransitionLength<<' '<<PocketTransitionAngle;
    return s.str();
}
bool CushionProfile::Deserialize(const std::string& Text,CushionProfile& Output,std::string* Error)
{
    std::istringstream s(Text); s.imbue(std::locale::classic()); CushionProfile c; std::string magic;
    if(!(s>>magic>>c.Id>>c.ParameterVersion>>c.Restitution>>c.TangentialFriction>>c.ContactHeight
          >>c.PocketTransitionLength>>c.PocketTransitionAngle) || magic!="snooker-cushion-v1")
        return Fail(Error,"invalid cushion schema or fields");
    s>>std::ws;
    if(!s.eof()) return Fail(Error,"unexpected data after cushion profile");
    if(!c.IsValid(Error)) return false;
    Output=std::move(c); return true;
}
}
