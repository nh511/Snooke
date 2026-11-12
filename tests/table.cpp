#include <Snooker/Data/TableDefinition.h>
#include <fstream>
#include <iostream>
#include <iterator>

#define CHECK(x) do { if (!(x)) { std::cerr << "FAIL table line " << __LINE__ << ": " << #x << '\n'; return 1; } } while(false)
int main(int argc,char** argv)
{
    using namespace Snooker;
    CHECK(argc==3);
    TableDefinition tables[2];
    for(int i=0;i<2;++i) {
        std::ifstream file(argv[i+1]); CHECK(bool(file));
        const std::string text((std::istreambuf_iterator<char>(file)),{});
        std::string error;
        CHECK(TableDefinition::Deserialize(text,tables[i],&error));
        CHECK(!tables[i].Cushions.empty()); CHECK(!tables[i].Pockets.empty());
        CHECK(!tables[i].Spots.empty()); CHECK(!tables[i].InitialBalls.empty());
        TableDefinition restored;
        CHECK(TableDefinition::Deserialize(tables[i].Serialize(),restored,&error));
        CHECK(restored.Serialize()==tables[i].Serialize());
        const auto original=restored.Serialize();
        for(const auto& bad:{text+"junk",text.substr(0,text.find("end")),std::string("snooker-table-v2")}) {
            CHECK(!TableDefinition::Deserialize(bad,restored,&error));
            CHECK(!error.empty()); CHECK(restored.Serialize()==original);
        }
        auto invalid=tables[i]; invalid.Cushions[0].End=invalid.Cushions[0].Start; CHECK(!invalid.IsValid());
        invalid=tables[i]; invalid.Pockets.push_back(invalid.Pockets[0]); CHECK(!invalid.IsValid());
        invalid=tables[i]; invalid.Cloth.Max.X=invalid.Table.Max.X+1; CHECK(!invalid.IsValid());
        invalid=tables[i]; invalid.InitialBalls[0].InitialPositionMeters.X=1000; CHECK(!invalid.IsValid());
        std::cout << tables[i].ParameterVersion << " cloth length=" << tables[i].Cloth.Max.X-tables[i].Cloth.Min.X << '\n';
    }
    CHECK(!NearlyEqual(tables[0].Cloth.Max.X,tables[1].Cloth.Max.X));
    std::cout << "PASS two independent table configurations, full round trips and invalid records\n";
}
