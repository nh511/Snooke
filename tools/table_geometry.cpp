#include <Snooker/Table/TableGeometry.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <locale>
#include <stdexcept>

std::string Read(const char* path) {std::ifstream f(path);if(!f)throw std::runtime_error("cannot read geometry input");return {std::istreambuf_iterator<char>(f),{}};}
int main(int argc,char** argv)
{
    using namespace Snooker;
    try {
        if(argc!=5)throw std::runtime_error("usage: table cushion-profile pocket-profile output.svg");
        TableDefinition table;CushionProfile cushion;PocketProfile pocket;TableGeometry g;std::string error;
        if(!TableDefinition::Deserialize(Read(argv[1]),table,&error) || !CushionProfile::Deserialize(Read(argv[2]),cushion,&error) ||
           !PocketProfile::Deserialize(Read(argv[3]),pocket,&error) || !GenerateTableGeometry(table,{cushion},{pocket},{},g,&error))
            throw std::runtime_error(error);
        const double width=table.Table.Max.X-table.Table.Min.X,height=table.Table.Max.Y-table.Table.Min.Y;
        std::ofstream out(argv[4]);out.imbue(std::locale::classic());
        if(!out)throw std::runtime_error("cannot write SVG");
        out<<std::setprecision(17)<<"<svg xmlns='http://www.w3.org/2000/svg' width='1200' height='760' viewBox='"
           <<table.Table.Min.X-0.7<<' '<<-table.Table.Max.Y-0.7<<' '<<width+1.4<<' '<<height+1.4<<"'>\n"
           <<"<rect x='"<<table.Table.Min.X-0.7<<"' y='"<<-table.Table.Max.Y-0.7<<"' width='"<<width+1.4<<"' height='"<<height+1.4<<"' fill='#f0f3f5'/>\n";
        const auto rect=[&](const TableBounds& b,const char* color) {
            out<<"<rect x='"<<b.Min.X<<"' y='"<<-b.Max.Y<<"' width='"<<b.Max.X-b.Min.X<<"' height='"<<b.Max.Y-b.Min.Y<<"' fill='"<<color<<"'/>\n";
        };
        rect(g.Table,"#7e8991");rect(g.Cloth,"#164d3a");
        for(const auto& s:g.Segments) {
            const char* color=s.Feature==TableFeature::PocketTransition?"#ffd166":s.Feature==TableFeature::PocketJaw?"#e66a7a":s.Feature==TableFeature::ClothBoundary?"#b5d2c4":"#87d4ed";
            out<<"<line x1='"<<s.Start.X<<"' y1='"<<-s.Start.Y<<"' x2='"<<s.End.X<<"' y2='"<<-s.End.Y<<"' stroke='"<<color<<"' stroke-width='0.012'/>\n";
        }
        for(const auto& p:g.Pockets) {
            out<<"<polygon points='";
            for(const auto& v:p.Floor)out<<v.X<<','<<-v.Y<<' ';
            out<<"' fill='#493a72' fill-opacity='0.55' stroke='#9e8ccc' stroke-width='0.005'/>\n";
            out<<"<line x1='"<<p.MouthStart.X<<"' y1='"<<-p.MouthStart.Y<<"' x2='"<<p.MouthEnd.X<<"' y2='"<<-p.MouthEnd.Y<<"' stroke='#ffffff' stroke-width='0.009'/>\n";
        }
        for(const auto& ball:table.InitialBalls)out<<"<circle cx='"<<ball.InitialPositionMeters.X<<"' cy='"<<-ball.InitialPositionMeters.Y<<"' r='"<<ball.RadiusMeters<<"' fill='white'/>\n";
        out<<"<text x='"<<table.Table.Min.X<<"' y='"<<-table.Table.Max.Y-0.25<<"' font-family='sans-serif' font-size='0.08'>Finite table geometry / engineering fixture</text>\n</svg>\n";
        out.flush();if(!out)throw std::runtime_error("SVG write failed");
        std::cout<<"PASS geometry generated: segments="<<g.Segments.size()<<" pockets="<<g.Pockets.size()<<"; not collision simulation\n";
    }catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}
}
