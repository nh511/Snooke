#include <Snooker/Data/CushionProfile.h>
#include <Snooker/Data/TableDefinition.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <locale>

#define CHECK(x) do { if(!(x)) { std::cerr<<"FAIL cushion line "<<__LINE__<<": "<<#x<<'\n'; return 1; } } while(false)
std::string Read(const char* Path) { std::ifstream f(Path); if(!f) return {}; return {std::istreambuf_iterator<char>(f),{}}; }
struct Comma : std::numpunct<char> { char do_decimal_point() const override { return ','; } };
int main(int argc,char** argv)
{
    using namespace Snooker;
    CHECK(argc==5);
    CushionProfile profiles[2];
    for(int i=0;i<2;++i) {
        const auto text=Read(argv[i+1]); CHECK(!text.empty()); std::string error;
        CHECK(CushionProfile::Deserialize(text,profiles[i],&error));
        TableDefinition table; CHECK(TableDefinition::Deserialize(Read(argv[i+3]),table,&error));
        for(const auto& segment:table.Cushions) CHECK(segment.ProfileId==profiles[i].Id);
        CushionProfile restored; CHECK(CushionProfile::Deserialize(profiles[i].Serialize(),restored));
        const auto original=restored.Serialize(); CHECK(original==profiles[i].Serialize());
        for(const auto& bad:{text+"junk",text.substr(0,text.find_last_of(' ')),std::string("snooker-cushion-v2")}) {
            CHECK(!CushionProfile::Deserialize(bad,restored,&error)); CHECK(!error.empty()); CHECK(restored.Serialize()==original);
        }
    }
    CHECK(profiles[0].Restitution!=profiles[1].Restitution);
    CHECK(profiles[0].PocketTransitionAngle!=profiles[1].PocketTransitionAngle);
    auto invalid=profiles[0]; invalid.Restitution=1.1; CHECK(!invalid.IsValid());
    invalid=profiles[0]; invalid.TangentialFriction=-1; CHECK(!invalid.IsValid());
    invalid=profiles[0]; invalid.ContactHeight=0; CHECK(!invalid.IsValid());
    invalid=profiles[0]; invalid.PocketTransitionLength=-1; CHECK(!invalid.IsValid());
    invalid=profiles[0]; invalid.PocketTransitionAngle=Pi; CHECK(!invalid.IsValid());
    invalid=profiles[0]; invalid.Restitution=std::numeric_limits<double>::quiet_NaN(); CHECK(!invalid.IsValid());
    const auto saved=profiles[0].Serialize(), oldLocale=std::locale().name();
    const std::locale previous;
    std::locale::global(std::locale(previous,new Comma));
    CushionProfile restored; const auto text=profiles[0].Serialize();
    const bool loaded=CushionProfile::Deserialize(text,restored);
    std::locale::global(previous);
    CHECK(loaded); CHECK(text==saved); CHECK(restored.Serialize()==saved);
    std::cout<<"PASS cushion files, table references, independent presets, bounds and locale round trips\n";
}
