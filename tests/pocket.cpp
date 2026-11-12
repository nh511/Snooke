#include <Snooker/Data/PocketProfile.h>
#include <Snooker/Data/TableDefinition.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>

#define CHECK(x) do { if(!(x)) { std::cerr<<"FAIL pocket line "<<__LINE__<<": "<<#x<<'\n'; return 1; } } while(false)
std::string Read(const char* Path) { std::ifstream f(Path); if(!f) return {}; return {std::istreambuf_iterator<char>(f),{}}; }
int main(int argc,char** argv)
{
    using namespace Snooker; CHECK(argc==5);
    PocketProfile profiles[2];
    for(int i=0;i<2;++i) {
        const auto text=Read(argv[i+1]); CHECK(!text.empty()); std::string error;
        CHECK(PocketProfile::Deserialize(text,profiles[i],&error));
        TableDefinition table; CHECK(TableDefinition::Deserialize(Read(argv[i+3]),table,&error));
        for(const auto& pocket:table.Pockets) {
            CHECK(pocket.ProfileId==profiles[i].Id);
            CHECK(NearlyEqual(pocket.Opening,profiles[i].OpeningWidth)); CHECK(NearlyEqual(pocket.Depth,profiles[i].BottomDepth));
        }
        PocketProfile restored; CHECK(PocketProfile::Deserialize(profiles[i].Serialize(),restored));
        const auto saved=restored.Serialize(); CHECK(saved==profiles[i].Serialize());
        for(const auto& bad:{text+"junk",text.substr(0,text.find_last_of(' ')),std::string("snooker-pocket-v2")}) {
            CHECK(!PocketProfile::Deserialize(bad,restored,&error)); CHECK(!error.empty()); CHECK(restored.Serialize()==saved);
        }
    }
    CHECK(profiles[0].CaptureDepth!=profiles[1].CaptureDepth);
    auto bad=profiles[0]; bad.OpeningWidth=0; CHECK(!bad.IsValid());
    bad=profiles[0]; bad.JawRadius=-1; CHECK(!bad.IsValid());
    bad=profiles[0]; bad.JawAngle=Pi; CHECK(!bad.IsValid());
    bad=profiles[0]; bad.BottomSlope=Pi/2; CHECK(!bad.IsValid());
    bad=profiles[0]; bad.CaptureDepth=bad.BottomDepth+0.01; CHECK(!bad.IsValid());
    bad=profiles[0]; bad.JawRestitution=1.1; CHECK(!bad.IsValid());
    bad=profiles[0]; bad.EjectionSpeedThreshold=-1; CHECK(!bad.IsValid());
    bad=profiles[0]; bad.BottomDepth=std::numeric_limits<double>::infinity(); CHECK(!bad.IsValid());
    std::cout<<"PASS independent pocket presets, table references, geometry/capture/ejection bounds and unchanged output\n";
}
