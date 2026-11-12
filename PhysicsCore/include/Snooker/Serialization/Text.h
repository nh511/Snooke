#pragma once
#include <charconv>
#include <istream>
#include <string>
#include <type_traits>

namespace Snooker
{
template<class UInt> bool ReadUnsigned(std::istream& Input, UInt& Output)
{
    static_assert(std::is_unsigned<UInt>::value, "Unsigned integer required");
    std::string Token;
    if (!(Input >> Token) || Token.empty()) return false;
    UInt Candidate{};
    const auto Result = std::from_chars(Token.data(), Token.data()+Token.size(), Candidate);
    if (Result.ec != std::errc{} || Result.ptr != Token.data()+Token.size()) return false;
    Output = Candidate;
    return true;
}
}
