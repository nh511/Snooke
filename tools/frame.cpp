#include <Snooker/Rules/FrameSave.h>
#include <fstream>
#include <iterator>
#include <iostream>
std::string Read(const char* p){std::ifstream f(p,std::ios::binary);if(!f)throw std::runtime_error("cannot read input");return {std::istreambuf_iterator<char>(f),{}};}void Write(const char* p,const std::string& t){std::ofstream f(p,std::ios::binary);if(!f||!(f<<t))throw std::runtime_error("cannot write output");}
int main(int argc,char** argv){using namespace Snooker;try{if(argc!=5||std::string(argv[1])!="--continue")throw std::runtime_error("usage: snooker_frame --continue save input output-save");FrameSave save,next;ShotInput input;std::string error;if(!FrameSave::Deserialize(Read(argv[2]),save,&error)||!ShotInput::Deserialize(Read(argv[3]),input,&error)||!ContinueFrame(save,input,next,nullptr,&error))throw std::runtime_error(error);Write(argv[4],next.Serialize());std::cout<<"PASS frame continued, shots="<<next.Shots.size()<<'\n';return 0;}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}
