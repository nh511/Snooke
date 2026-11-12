#include <Snooker/Math/CoreTypes.h>
#include <Snooker/Data/CoreConfig.h>
#include <Snooker/Physics/PhysicsSnapshot.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <locale>
#include <string>

int main(int argc, char** argv)
{
    using namespace Snooker;
    auto fail=[](const std::string& message) { std::cerr << "FAIL " << message << '\n'; return 1; };
    if (argc==3 && std::string(argv[1])=="--config")
    {
        std::ifstream file(argv[2]);
        if (!file) return fail("configuration file cannot be opened");
        const std::string text((std::istreambuf_iterator<char>(file)),{});
        CoreConfig config; std::string error;
        if (!CoreConfig::Deserialize(text,config,&error)) return fail(error);
        std::cout << "PASS configuration " << config.ParameterVersion << " balls=" << config.Balls.size()
                  << "; validation only, no shot simulated\n";
        return 0;
    }
    if (argc!=5 || std::string(argv[1])!="--parameters" || std::string(argv[3])!="--snapshot")
        return fail("usage: snooker_validation --parameters <file> --snapshot <file>");
    std::ifstream parameters(argv[2]);
    parameters.imbue(std::locale::classic());
    std::string magic, trailing;
    double tolerance=0;
    if (!(parameters >> magic >> tolerance) || magic!="validation-v1" || !IsFinite(tolerance) ||
        tolerance<=0 || tolerance>1e-6 || parameters>>trailing) return fail("invalid validation parameters");
    std::ifstream input(argv[4]);
    if (!input) return fail("snapshot file cannot be opened");
    const std::string text((std::istreambuf_iterator<char>(input)),{});
    FPhysicsSnapshot snapshot;
    std::string error;
    if (!FPhysicsSnapshot::Deserialize(text,snapshot,&error)) return fail(error);
    if (!snapshot.Balls.empty()) return fail("nonempty simulation requires the pending solver; not implemented");
    Quatd rotation;
    if (!Quatd::TryFromAxisAngle({0,0,1},Pi/2,rotation) ||
        !rotation.Rotate({1,0,0}).NearlyEquals({0,1,0},tolerance)) return fail("quaternion math");
    FPhysicsSnapshot restored;
    if (!FPhysicsSnapshot::Deserialize(snapshot.Serialize(),restored,&error) || !(restored==snapshot))
        return fail("empty snapshot round trip");
    std::cout << "PASS empty ball state and quaternion math; no nonempty physics simulation claimed\n";
}
