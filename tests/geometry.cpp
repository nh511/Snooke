#include <Snooker/Table/TableGeometry.h>
#include <fstream>
#include <iostream>
#include <iterator>

#define CHECK(x) do { if(!(x)) { std::cerr<<"FAIL geometry line "<<__LINE__<<": "<<#x<<'\n'; return 1; } } while(false)
std::string Read(const char* path) {std::ifstream f(path);return {std::istreambuf_iterator<char>(f),{}};}
int main(int argc,char** argv)
{
    using namespace Snooker;CHECK(argc==7);
    TableGeometry generated[2];
    for(int i=0;i<2;++i) {
        TableDefinition table;CushionProfile cushion;PocketProfile pocket;std::string error;
        CHECK(TableDefinition::Deserialize(Read(argv[1+i*3]),table,&error));
        CHECK(CushionProfile::Deserialize(Read(argv[2+i*3]),cushion,&error));
        CHECK(PocketProfile::Deserialize(Read(argv[3+i*3]),pocket,&error));
        CHECK(GenerateTableGeometry(table,{cushion},{pocket},{},generated[i],&error));
        const auto& g=generated[i];CHECK(g.Pockets.size()==table.Pockets.size());
        for(const auto& segment:g.Segments) {
            if(segment.Feature==TableFeature::PocketJaw)continue;
            CHECK(NearlyEqual(segment.InwardNormal.Length(),1));
            CHECK(NearlyEqual(segment.InwardNormal.Dot(segment.End-segment.Start),0));
        }
        CHECK(g.Segments.size()==4+table.Cushions.size()*4+table.Pockets.size()*24);
        const auto& first=g.Segments[4];CHECK(first.Feature==TableFeature::CushionFace);
        CHECK(NearlyEqual(first.Start.Z,cushion.ContactHeight));
        CHECK(NearlyEqual((first.End-first.Start).Length(),(table.Cushions[0].End-table.Cushions[0].Start).Length()-2*cushion.PocketTransitionLength));
        for(const auto& p:g.Pockets) {
            CHECK(NearlyEqual((p.MouthEnd-p.MouthStart).Length(),pocket.OpeningWidth));
            CHECK(NearlyEqual(p.BottomCenter.Z,-pocket.BottomDepth));CHECK(p.Floor.size()==4);
            CHECK(NearlyEqual((p.BottomCenter-p.Definition.Center).Dot(p.Inward),-pocket.BottomDepth/std::tan(pocket.BottomSlope)));
            for(const auto* jaw:{&p.LeftJaw,&p.RightJaw})CHECK(jaw->size()==13);
        }
        TableGeometry repeated;CHECK(GenerateTableGeometry(table,{cushion},{pocket},{},repeated));
        CHECK(repeated.Segments.size()==g.Segments.size());
        for(std::size_t j=0;j<g.Segments.size();++j)CHECK(repeated.Segments[j].Start.NearlyEquals(g.Segments[j].Start,0));
        const auto saved=g.ParameterVersion;const auto size=g.Segments.size();
        CHECK(!GenerateTableGeometry(table,{}, {pocket},{},generated[i],&error));CHECK(!error.empty());
        CHECK(generated[i].ParameterVersion==saved && generated[i].Segments.size()==size);
        auto mismatch=cushion;mismatch.ContactHeight+=0.01;
        CHECK(!GenerateTableGeometry(table,{mismatch},{pocket},{},repeated));
        CHECK(!GenerateTableGeometry(table,{cushion,cushion},{pocket},{},repeated));
        auto wrong=pocket;wrong.OpeningWidth+=0.01;CHECK(!GenerateTableGeometry(table,{cushion},{wrong},{},repeated));
        auto bad=GeometrySettings{};bad.JawSubdivisions=0;CHECK(!GenerateTableGeometry(table,{cushion},{pocket},bad,repeated));
        auto changed=table;changed.ParameterVersion="geometry-change-v1";
        changed.Table.Max.X+=0.5;changed.Cloth.Max.X+=0.5;
        for(auto& p:changed.Pockets)if(NearlyEqual(p.Center.X,table.Cloth.Max.X))p.Center.X+=0.5;
        CHECK(GenerateTableGeometry(changed,{cushion},{pocket},{},repeated));
        CHECK(repeated.Cloth.Max.X!=g.Cloth.Max.X);
        CHECK(repeated.Pockets.back().Definition.Center.X!=g.Pockets.back().Definition.Center.X);
    }
    CHECK(generated[0].Cloth.Max.X!=generated[1].Cloth.Max.X);
    std::cout<<"PASS file-driven finite geometry, profile resolution, reproducibility and transactional errors\n";
}
