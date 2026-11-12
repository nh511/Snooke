#include "Snooker/Validation/Log.h"
#include <fstream>
#include <sstream>
#include <iostream>
int main(int argc, char** argv)
{
    using namespace Snooker;
    if (argc != 2) return 2;
    std::ostringstream Console;
    Logger Log(Logger::StreamSink(Console));
    Log.SetMinimum(LogCategory::Physics, LogLevel::Warning);
    Log.SetMinimum(LogCategory::Rules, LogLevel::Off);
    Log.Write(LogCategory::Physics, LogLevel::Trace, 42, "hidden");
    Log.Write(LogCategory::Rules, LogLevel::Error, 42, "disabled");
    for (auto Category : {LogCategory::Physics, LogCategory::Shot, LogCategory::Replay,
                          LogCategory::Validation, LogCategory::Interaction})
        Log.Write(Category, LogLevel::Error, 42, "visible");
    const auto Text = Console.str();
    if (Text.find("hidden") != std::string::npos || Text.find("disabled") != std::string::npos) return 1;
    for (const auto* Name : {"Physics", "Shot", "Replay", "Validation", "Interaction"})
        if (Text.find(std::string("[") + Name + "][shot=42]") == std::string::npos) return 1;
    {
        std::ofstream File(argv[1]);
        if (!File) { std::cerr << "Cannot write log file: " << argv[1] << '\n'; return 1; }
        Logger FileLog(Logger::StreamSink(File));
        FileLog.Write(LogCategory::Rules, LogLevel::Info, 42, "score");
    }
    std::ifstream File(argv[1]);
    std::string Line;
    if (!std::getline(File, Line) || Line != "[Rules][shot=42][level=1] score") return 1;
    std::cout << "PASS log filters, shot correlation and real file sink\n";
}
