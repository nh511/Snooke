#include <Snooker/Data/BallDefinition.h>
#include <fstream>
#include <iostream>
#include <iterator>
int main()
{
    using namespace Snooker;
    for (int type=0;type<8;++type) {
        BallDefinition value;
        value.Id=static_cast<BallId>(type+1);
        value.Type=static_cast<EBallType>(type);
        value.InitialPositionMeters={0.125,0.25,0.02625};
        value.MaterialProfileId="test-material-v1";
        {
            std::ofstream file("ball-definition.txt");
            if (!file || !(file<<value.Serialize())) return 1;
        }
        std::ifstream file("ball-definition.txt");
        if (!file) return 1;
        const std::string text((std::istreambuf_iterator<char>(file)),{});
        BallDefinition restored;
        if (!BallDefinition::Deserialize(text,restored) || !(restored==value) || text!=value.Serialize()) return 1;
        std::cout << text << '\n';
    }
    std::cout << "PASS complete ball definitions persisted, restored and printed for all eight types\n";
}
