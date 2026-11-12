#include <Snooker/Physics/CushionContact.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>

#define CHECK(x) do { if(!(x)) { std::cerr<<"FAIL cushion contact line "<<__LINE__<<": "<<#x<<'\n'; return 1; } } while(false)
std::string Read(const char* p) {std::ifstream f(p);return {std::istreambuf_iterator<char>(f),{}};}
int main(int argc,char** argv)
{
    using namespace Snooker;CHECK(argc==4);
    TableDefinition table;CushionProfile cushion;PocketProfile pocket;TableGeometry g;
    CHECK(TableDefinition::Deserialize(Read(argv[1]),table));
    CHECK(CushionProfile::Deserialize(Read(argv[2]),cushion));CHECK(PocketProfile::Deserialize(Read(argv[3]),pocket));
    CHECK(GenerateTableGeometry(table,{cushion},{pocket},{},g));
    BallState ball;ball.Id=1;ball.PositionMeters={0,table.Cloth.Min.Y+0.02,ball.RadiusMeters};
    std::vector<CushionContact> contacts;
    CHECK(FindCushionContacts(ball,g,1e-8,contacts));
    bool edge=false,face=false,top=false;
    for(const auto& c:contacts) {
        edge|=c.Feature==TableFeature::ClothBoundary;
        face|=c.Feature==TableFeature::CushionFace;top|=c.Feature==TableFeature::CushionTop;
        CHECK(c.Normal.IsFinite());CHECK(NearlyEqual(c.Normal.Length(),1));
        if(c.Feature==TableFeature::ClothBoundary)CHECK(c.DiagnosticOnly);
        else {CHECK(!c.DiagnosticOnly);CHECK(c.ProfileId==cushion.Id);}
    }
    CHECK(edge && face && !top);
    ball.PositionMeters.Z=cushion.ContactHeight+0.01;
    CHECK(FindCushionContacts(ball,g,1e-8,contacts));face=false;top=false;
    for(const auto& c:contacts){face|=c.Feature==TableFeature::CushionFace;top|=c.Feature==TableFeature::CushionTop;}
    CHECK(top && !face);
    const auto transition=g.Segments[6];CHECK(transition.Feature==TableFeature::PocketTransition);
    ball.PositionMeters=(transition.Start+transition.End)*0.5+transition.InwardNormal*0.02;
    CHECK(FindCushionContacts(ball,g,1e-8,contacts));bool found=false;
    for(const auto& c:contacts)found|=c.Feature==TableFeature::PocketTransition;CHECK(found);
    // Endpoint distance, not an infinite plane: moving along the line beyond the finite end misses.
    TableGeometry isolated=g;isolated.Segments={g.Segments[4]};
    ball.PositionMeters=isolated.Segments[0].End+Vec3d{0.5,0,0};
    CHECK(FindCushionContacts(ball,isolated,1e-8,contacts));CHECK(contacts.empty());
    ball.PositionMeters={0,0,ball.RadiusMeters};CHECK(FindCushionContacts(ball,g,1e-8,contacts));CHECK(contacts.empty());
    ball.PositionMeters={table.Cloth.Max.X,table.Cloth.Max.Y,ball.RadiusMeters};
    CHECK(FindCushionContacts(ball,g,1e-8,contacts));
    for(const auto& c:contacts)CHECK(c.Feature==TableFeature::ClothBoundary && c.DiagnosticOnly);
    const auto saved=contacts;const auto original=ball.Serialize();
    CHECK(!FindCushionContacts(ball,g,-1,contacts));CHECK(contacts.size()==saved.size());CHECK(ball.Serialize()==original);
    isolated=g;isolated.Segments[0].End.X=std::numeric_limits<double>::infinity();
    CHECK(!FindCushionContacts(ball,isolated,1e-8,contacts));CHECK(contacts.size()==saved.size());
    CHECK(FindCushionContacts(ball,g,1e-8,contacts));CHECK(contacts.size()==saved.size());
    for(std::size_t i=0;i<saved.size();++i)CHECK(contacts[i].SegmentIndex==saved[i].SegmentIndex);
    std::cout<<"PASS finite cushion/transition/top-edge classification, pocket gap, diagnostic boundary and unchanged errors\n";
}
