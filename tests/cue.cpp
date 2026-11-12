#include <Snooker/Shot/CueProfile.h>
#include <Snooker/Data/CoreConfig.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>

#define CHECK(x) do { if(!(x)) { std::cerr<<"FAIL cue line "<<__LINE__<<": "<<#x<<'\n'; return 1; } } while(false)
std::string Read(const char* Path) { std::ifstream f(Path); if(!f) return {}; return {std::istreambuf_iterator<char>(f),{}}; }
int main(int argc,char** argv)
{
    using namespace Snooker; CHECK(argc==4);
    CueProfile standard,training; std::string error;
    CHECK(CueProfile::Deserialize(Read(argv[1]),standard,&error));
    CHECK(CueProfile::Deserialize(Read(argv[2]),training,&error));
    const double radius=0.02625;
    CHECK(standard.AllowsInput(0.2,3,0.012,radius));
    CHECK(!training.AllowsInput(0.2,3,0.012,radius));
    CHECK(!training.AllowsInput(0.13,1,0,radius));
    CHECK(!training.AllowsInput(0.1,2.1,0,radius));
    CHECK(!training.AllowsInput(0.1,1,0.012,radius));
    CHECK(training.AllowsInput(0.1,1,0.005,radius));
    CHECK(!standard.AllowsInput(0.1,1,0,radius/10));
    CHECK(!standard.AllowsInput(-1,1,0,radius));
    CHECK(!standard.AllowsInput(0.1,std::numeric_limits<double>::infinity(),0,radius));
    for(const auto& p:{standard,training}) {
        CueProfile restored; CHECK(CueProfile::Deserialize(p.Serialize(),restored));
        const auto saved=restored.Serialize(); CHECK(saved==p.Serialize());
        for(const auto& bad:{saved+" junk",saved.substr(0,saved.find_last_of(' ')),std::string("snooker-cue-v2")}) {
            CHECK(!CueProfile::Deserialize(bad,restored,&error)); CHECK(!error.empty()); CHECK(restored.Serialize()==saved);
        }
    }
    auto bad=standard; bad.TipRestitution=1.1; CHECK(!bad.IsValid());
    bad=standard; bad.MaxOffsetRadiusRatio=1; CHECK(!bad.IsValid());
    bad=standard; bad.MaxForwardSpeedMetersPerSecond=0; CHECK(!bad.IsValid());
    bad=standard; bad.ParameterVersion=""; CHECK(!bad.IsValid());
    CoreConfig config; CHECK(CoreConfig::Deserialize(Read(argv[3]),config,&error));
    CHECK(config.Cue.Serialize()==standard.Serialize());
    config.Cue=training; CoreConfig restored;
    CHECK(CoreConfig::Deserialize(config.Serialize(),restored,&error));
    CHECK(restored.Cue.Serialize()==training.Serialize());
    CHECK(!CoreConfig::Deserialize("snooker-config-v1",restored,&error));
    std::cout<<"PASS cue presets change pullback/speed/offset limits and all new fields persist in shared config\n";
}
