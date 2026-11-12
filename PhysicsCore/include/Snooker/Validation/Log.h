#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <ostream>
#include <string>
#include <utility>

namespace Snooker
{
enum class LogCategory { Physics, Rules, Shot, Replay, Validation, Interaction, Count };
enum class LogLevel { Trace, Info, Warning, Error, Off };
struct LogEntry
{
    LogCategory Category;
    LogLevel Level;
    std::uint64_t ShotId;
    std::string Message;
};
inline const char* CategoryName(LogCategory Value)
{
    switch (Value) {
    case LogCategory::Physics: return "Physics";
    case LogCategory::Rules: return "Rules";
    case LogCategory::Shot: return "Shot";
    case LogCategory::Replay: return "Replay";
    case LogCategory::Validation: return "Validation";
    case LogCategory::Interaction: return "Interaction";
    default: return "Unknown";
    }
}
// Sink is synchronous; caller owns any stream or UE adapter captured by it.
class Logger
{
public:
    using Sink = std::function<void(const LogEntry&)>;
    explicit Logger(Sink Target) : Target(std::move(Target)) { Minimum.fill(LogLevel::Info); }
    void SetMinimum(LogCategory Category, LogLevel Level)
    {
        Minimum.at(static_cast<std::size_t>(Category)) = Level;
    }
    void Write(LogCategory Category, LogLevel Level, std::uint64_t ShotId, const std::string& Message) const
    {
        const auto Threshold = Minimum.at(static_cast<std::size_t>(Category));
        if (Target && Level != LogLevel::Off && Threshold != LogLevel::Off && Level >= Threshold)
            Target({Category, Level, ShotId, Message});
    }
    static Sink StreamSink(std::ostream& Output)
    {
        return [&Output](const LogEntry& Entry) {
            Output << '[' << CategoryName(Entry.Category) << "][shot=" << Entry.ShotId
                   << "][level=" << static_cast<int>(Entry.Level) << "] " << Entry.Message << '\n';
        };
    }
private:
    Sink Target;
    std::array<LogLevel, static_cast<std::size_t>(LogCategory::Count)> Minimum;
};
}
