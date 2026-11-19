#include <Snooker/Rules/ColorRespot.h>
#include <fstream>
#include <iterator>
#include <iostream>

#define CHECK(x) do{if(!(x)){std::cerr<<"FAIL respot "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
using namespace Snooker;std::string Read(const char* p){std::ifstream f(p);return {std::istreambuf_iterator<char>(f),{}};}
BallDefinition Color(unsigned id,EBallType type){BallDefinition b;b.Id=id;b.Type=type;b.InitialPositionMeters={0,0,b.RadiusMeters};return b;}
int main(int argc,char** argv)
{
    std::string error;CHECK(argc==2);TableDefinition table;CHECK(TableDefinition::Deserialize(Read(argv[1]),table,&error));ColorRespotResult result;
    auto blue=Color(20,EBallType::Blue);CHECK(FindColorRespot(table,blue,{},result,&error));CHECK(result.Status==RespotStatus::OwnSpot&&result.SpotType==EBallType::Blue);
    auto blocker=Color(30,EBallType::Red);blocker.InitialPositionMeters=table.Spots[3].Position;CHECK(FindColorRespot(table,blue,{blocker},result,&error));CHECK(result.Status==RespotStatus::AlternateSpot&&result.SpotType==EBallType::Black);
    std::vector<BallDefinition> occupied;unsigned id=30;for(const auto& spot:table.Spots){auto b=Color(id++,EBallType::Red);b.InitialPositionMeters=spot.Position;occupied.push_back(b);}CHECK(FindColorRespot(table,blue,occupied,result,&error));CHECK(result.Status==RespotStatus::NoSpotAvailable);
    auto original=result;auto invalid=blue;invalid.Type=EBallType::Red;CHECK(!FindColorRespot(table,invalid,{},result,&error));CHECK(result.Status==original.Status);
    std::vector<ColorRespotResult> many;auto black=Color(21,EBallType::Black);CHECK(FindColorRespots(table,{blue,black},{},many,&error));CHECK(many.size()==2&&many[0].Ball.Type==EBallType::Black&&many[0].Status==RespotStatus::OwnSpot&&many[1].Status==RespotStatus::OwnSpot);
    CHECK(FindColorRespots(table,{blue,black},occupied,many,&error));CHECK(many[0].Status==RespotStatus::NoSpotAvailable&&many[1].Status==RespotStatus::NoSpotAvailable);
    std::cout<<"PASS own/highest alternate/no spot and deterministic high-value multi-color respot\n";
}
