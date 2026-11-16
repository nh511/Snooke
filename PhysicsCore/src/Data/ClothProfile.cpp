#include <Snooker/Data/ClothProfile.h>
#include <iomanip>
#include <locale>
#include <sstream>

namespace Snooker
{
namespace { bool Fail(std::string* e,const char* m){if(e)*e=m;return false;} }
std::string ClothProfile::Serialize() const
{
    std::ostringstream s;s.imbue(std::locale::classic());s<<std::setprecision(std::numeric_limits<Scalar>::max_digits10)
      <<"snooker-cloth-v1 "<<ParameterVersion<<' '<<Condition<<' '<<Sliding<<' '<<Rolling<<' '<<Spin<<' '
      <<SleepLinearSpeed<<' '<<SleepAngularSpeed<<' '<<SleepPositionTolerance<<' '<<SleepDuration;return s.str();
}
bool ClothProfile::Deserialize(const std::string& Text,ClothProfile& Output,std::string* Error)
{
    std::istringstream s(Text);s.imbue(std::locale::classic());ClothProfile c;std::string magic;
    if(!(s>>magic>>c.ParameterVersion>>c.Condition>>c.Sliding>>c.Rolling>>c.Spin>>c.SleepLinearSpeed
         >>c.SleepAngularSpeed>>c.SleepPositionTolerance>>c.SleepDuration) || magic!="snooker-cloth-v1")
        return Fail(Error,"invalid cloth schema or fields");
    s>>std::ws;if(!s.eof())return Fail(Error,"unexpected cloth profile tail");
    if(!c.IsValid(Error))return false;Output=std::move(c);return true;
}
}
