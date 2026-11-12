#include <Snooker/Data/CoreConfig.h>
#include <Snooker/Serialization/Text.h>
#include <iomanip>
#include <locale>
#include <sstream>

namespace Snooker
{
namespace
{
bool Fail(std::string* Error, const char* Message) { if (Error) *Error=Message; return false; }
bool Positive(Scalar v) { return IsFinite(v) && v>0; }
bool Nonnegative(Scalar v) { return IsFinite(v) && v>=0; }
bool Tag(std::istream& Stream, const char* Expected) { std::string Value; return bool(Stream>>Value) && Value==Expected; }
}
bool CoreConfig::IsValid(std::string* Error) const
{
    if (ParameterVersion.empty() || ParameterVersion.find_first_of(" \t\r\n")!=std::string::npos)
        return Fail(Error,"invalid parameter version token");
    if (!Positive(Table.Length) || !Positive(Table.Width) ||
        !Nonnegative(Cloth.Sliding) || !Nonnegative(Cloth.Rolling) || !Nonnegative(Cloth.Spin) ||
        !Nonnegative(Cushion.Restitution) || Cushion.Restitution>1 || !Nonnegative(Cushion.TangentialFriction) ||
        !Positive(Cushion.ContactHeight) || !Positive(Pocket.Opening) || !Positive(Pocket.JawRadius) || !Positive(Pocket.Depth) ||
        !Positive(Solver.FixedStep) || Solver.Iterations==0 || Solver.Iterations>1024 ||
        !Nonnegative(Solver.SleepLinearSpeed) || !Nonnegative(Solver.SleepAngularSpeed) || !Nonnegative(Solver.SleepDuration))
        return Fail(Error,"nonfinite or out-of-range configuration parameter");
    if (!Cue.IsValid(Error)) return false;
    if (Balls.size()>4096) return Fail(Error,"ball resource limit exceeded");
    for (std::size_t i=0;i<Balls.size();++i) {
        if (!Balls[i].IsValid(Error)) return false;
        for (std::size_t j=0;j<i;++j) if (Balls[i].Id==Balls[j].Id) return Fail(Error,"duplicate ball id");
    }
    return true;
}
std::string CoreConfig::Serialize() const
{
    std::ostringstream s; s.imbue(std::locale::classic());
    s << std::setprecision(std::numeric_limits<Scalar>::max_digits10)
      << "snooker-config-v2 " << ParameterVersion
      << "\ntable " << Table.Length << ' ' << Table.Width
      << "\ncloth " << Cloth.Sliding << ' ' << Cloth.Rolling << ' ' << Cloth.Spin
      << "\ncushion " << Cushion.Restitution << ' ' << Cushion.TangentialFriction << ' ' << Cushion.ContactHeight
      << "\npocket " << Pocket.Opening << ' ' << Pocket.JawRadius << ' ' << Pocket.Depth
      << "\ncue " << Cue.Id << ' ' << Cue.LengthMeters << ' ' << Cue.MassKg << ' ' << Cue.TipRadiusMeters << ' '
      << Cue.MaxPullbackMeters << ' ' << Cue.MaxImpulseNewtonSeconds << ' ' << Cue.ParameterVersion << ' '
      << Cue.MaxForwardSpeedMetersPerSecond << ' ' << Cue.MaxOffsetRadiusRatio << ' ' << Cue.TipRestitution
      << "\nsolver " << Solver.FixedStep << ' ' << Solver.Iterations << ' ' << Solver.SleepLinearSpeed << ' '
      << Solver.SleepAngularSpeed << ' ' << Solver.SleepDuration << "\nballs " << Balls.size();
    for (const auto& ball:Balls) s << "\nball " << ball.Serialize();
    s << "\nend\n";
    return s.str();
}
bool CoreConfig::Deserialize(const std::string& Text, CoreConfig& Output, std::string* Error)
{
    std::istringstream s(Text); s.imbue(std::locale::classic());
    CoreConfig c; std::size_t count=0;
    if (!Tag(s,"snooker-config-v2") || !(s>>c.ParameterVersion) ||
        !Tag(s,"table") || !(s>>c.Table.Length>>c.Table.Width) ||
        !Tag(s,"cloth") || !(s>>c.Cloth.Sliding>>c.Cloth.Rolling>>c.Cloth.Spin) ||
        !Tag(s,"cushion") || !(s>>c.Cushion.Restitution>>c.Cushion.TangentialFriction>>c.Cushion.ContactHeight) ||
        !Tag(s,"pocket") || !(s>>c.Pocket.Opening>>c.Pocket.JawRadius>>c.Pocket.Depth) ||
        !Tag(s,"cue") || !(s>>c.Cue.Id>>c.Cue.LengthMeters>>c.Cue.MassKg>>c.Cue.TipRadiusMeters>>c.Cue.MaxPullbackMeters>>c.Cue.MaxImpulseNewtonSeconds>>c.Cue.ParameterVersion>>c.Cue.MaxForwardSpeedMetersPerSecond>>c.Cue.MaxOffsetRadiusRatio>>c.Cue.TipRestitution) ||
        !Tag(s,"solver") || !(s>>c.Solver.FixedStep) || !ReadUnsigned(s,c.Solver.Iterations) ||
        !(s>>c.Solver.SleepLinearSpeed>>c.Solver.SleepAngularSpeed>>c.Solver.SleepDuration) ||
        !Tag(s,"balls") || !ReadUnsigned(s,count) || count>4096) return Fail(Error,"invalid config schema or fields");
    for (std::size_t i=0;i<count;++i) {
        std::string line;
        if (!Tag(s,"ball") || !std::getline(s,line)) return Fail(Error,"missing ball definition");
        FBallDefinition ball;
        if (!FBallDefinition::Deserialize(line,ball,Error)) return false;
        c.Balls.push_back(ball);
    }
    if (!Tag(s,"end")) return Fail(Error,"missing config terminator");
    s >> std::ws;
    if (!s.eof()) return Fail(Error,"unexpected data after configuration");
    if (!c.IsValid(Error)) return false;
    Output=std::move(c);
    return true;
}
}
