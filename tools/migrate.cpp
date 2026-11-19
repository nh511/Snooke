#include <Snooker/Rules/Migration.h>
#include <fstream>
#include <iterator>
#include <iostream>
int main(int argc,char**argv){using namespace Snooker;if(argc!=3){std::cerr<<"usage: snooker_migrate kind file\n";return 2;}std::ifstream f(argv[2],std::ios::binary);if(!f)return 2;const std::string t{std::istreambuf_iterator<char>(f),{}};MigrationReport r;std::string e;bool ok=false;if(std::string(argv[1])=="match"){MatchState x;ok=MigrateMatchText(t,x,r,&e);if(ok)std::cout<<x.Serialize()<<'\n';}else if(std::string(argv[1])=="snapshot"){PhysicsSnapshot x;ok=MigrateSnapshotText(t,x,r,&e);if(ok)std::cout<<x.Serialize()<<'\n';}else if(std::string(argv[1])=="shot")ok=InspectShotText(t,nullptr,r,&e);else if(std::string(argv[1])=="frame")ok=InspectFrameText(t,nullptr,r,&e);else return 2;std::cerr<<r.Message<<'\n';return ok?0:(r.Status==MigrationStatus::RecognizedIncompatible?3:1);}
