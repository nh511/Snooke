#include <Snooker/Data/CoreConfig.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <locale>

#define CHECK(x) do { if (!(x)) { std::cerr << "FAIL config line " << __LINE__ << ": " << #x << '\n'; return 1; } } while(false)
int main(int argc,char** argv)
{
    using namespace Snooker;
    CHECK(argc==2);
    std::ifstream file(argv[1]); CHECK(bool(file));
    const std::string text((std::istreambuf_iterator<char>(file)),{});
    CoreConfig config; std::string error;
    CHECK(CoreConfig::Deserialize(text,config,&error));
    CHECK(config.ParameterVersion=="engineering-v1"); CHECK(config.Balls.size()==1);
    CHECK(NearlyEqual(config.Solver.FixedStep,1.0/240));
    CHECK(NearlyEqual(config.Table.Length,3.569));
    CoreConfig restored;
    CHECK(CoreConfig::Deserialize(config.Serialize(),restored,&error));
    CHECK(config.Serialize()==restored.Serialize());
    const auto original=config.Serialize();
    for (const auto& bad: {text+"junk",std::string("snooker-config-v1"),std::string("snooker-config-v3"),text.substr(0,text.find("end"))}) {
        CHECK(!CoreConfig::Deserialize(bad,config,&error)); CHECK(config.Serialize()==original); CHECK(!error.empty());
    }
    auto invalid=restored; invalid.Cloth.Rolling=-1; CHECK(!invalid.IsValid());
    invalid=restored; invalid.Solver.Iterations=0; CHECK(!invalid.IsValid());
    invalid=restored; invalid.Balls.push_back(invalid.Balls.front()); CHECK(!invalid.IsValid());
    std::string overflow=text; auto start=overflow.find("balls 1"); overflow.replace(start,7,"balls 9999999999999999999");
    CHECK(!CoreConfig::Deserialize(overflow,config,&error)); CHECK(config.Serialize()==original);
    std::cout << "PASS versioned config file load, bounds, round trip and unchanged-output tests\n";
}
