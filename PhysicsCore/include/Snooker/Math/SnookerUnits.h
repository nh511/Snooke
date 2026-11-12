#pragma once

#include <cmath>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>
#include <string>

namespace Snooker
{
using Scalar = double;
using Meters = Scalar;
using Kilograms = Scalar;
using Seconds = Scalar;
using Radians = Scalar;
using BallId = std::uint32_t;
using ShotId = std::uint64_t;
using StepIndex = std::uint64_t;

constexpr Scalar Pi = 3.141592653589793238462643383279502884;
constexpr Meters CentimetersToMeters(const Scalar Centimeters)
{
    return Centimeters * 0.01;
}

constexpr Scalar MetersToCentimeters(const Meters Value) { return Value * 100.0; }
constexpr Scalar RadiansToDegrees(const Radians Value) { return Value * 180.0 / Pi; }

constexpr Radians DegreesToRadians(const Scalar Degrees)
{
    return Degrees * Pi / 180.0;
}

inline bool IsFinite(const Scalar Value)
{
    return std::isfinite(Value);
}

inline bool NearlyEqual(const Scalar Left, const Scalar Right, const Scalar Tolerance = 1e-9)
{
    return std::abs(Left - Right) <= Tolerance;
}

struct Vec3d
{
    Scalar X = 0.0;
    Scalar Y = 0.0;
    Scalar Z = 0.0;

    constexpr Vec3d() = default;
    constexpr Vec3d(const Scalar InX, const Scalar InY, const Scalar InZ)
        : X(InX), Y(InY), Z(InZ)
    {
    }

    constexpr Vec3d operator+(const Vec3d& Other) const
    {
        return {X + Other.X, Y + Other.Y, Z + Other.Z};
    }

    constexpr Vec3d operator-(const Vec3d& Other) const
    {
        return {X - Other.X, Y - Other.Y, Z - Other.Z};
    }

    constexpr Vec3d operator*(const Scalar Amount) const
    {
        return {X * Amount, Y * Amount, Z * Amount};
    }

    constexpr Vec3d& operator+=(const Vec3d& Other)
    {
        X += Other.X;
        Y += Other.Y;
        Z += Other.Z;
        return *this;
    }

    constexpr Scalar Dot(const Vec3d& Other) const
    {
        return X * Other.X + Y * Other.Y + Z * Other.Z;
    }

    constexpr Vec3d Cross(const Vec3d& Other) const
    {
        return {
            Y * Other.Z - Z * Other.Y,
            Z * Other.X - X * Other.Z,
            X * Other.Y - Y * Other.X,
        };
    }

    constexpr Scalar LengthSquared() const
    {
        return Dot(*this);
    }

    Scalar Length() const
    {
        return std::sqrt(LengthSquared());
    }

    bool IsFinite() const
    {
        return Snooker::IsFinite(X) && Snooker::IsFinite(Y) && Snooker::IsFinite(Z);
    }

    bool NearlyEquals(const Vec3d& Other, const Scalar Tolerance = 1e-9) const
    {
        return NearlyEqual(X, Other.X, Tolerance) &&
               NearlyEqual(Y, Other.Y, Tolerance) &&
               NearlyEqual(Z, Other.Z, Tolerance);
    }

    std::string ToString() const
    {
        std::ostringstream Stream;
        Stream.imbue(std::locale::classic());
        Stream << std::setprecision(std::numeric_limits<Scalar>::max_digits10)
               << X << ',' << Y << ',' << Z;
        return Stream.str();
    }
};

using FSnookerVector3 = Vec3d;
constexpr Vec3d TableNormal = {0.0, 0.0, 1.0};

// A reflection changes polar and axial vectors differently. Angular velocity is axial.
constexpr FSnookerVector3 CoreDirectionToUE(const FSnookerVector3& Value)
{
    return {Value.X, -Value.Y, Value.Z};
}
constexpr FSnookerVector3 UEDirectionToCore(const FSnookerVector3& Value)
{
    return CoreDirectionToUE(Value);
}
constexpr FSnookerVector3 CorePositionToUECentimeters(const FSnookerVector3& Value)
{
    return CoreDirectionToUE(Value) * 100.0;
}
constexpr FSnookerVector3 UEPositionCentimetersToCore(const FSnookerVector3& Value)
{
    return UEDirectionToCore(Value) * 0.01;
}
constexpr FSnookerVector3 CoreAngularVelocityToUE(const FSnookerVector3& Value)
{
    return {-Value.X, Value.Y, -Value.Z};
}
constexpr FSnookerVector3 UEAngularVelocityToCore(const FSnookerVector3& Value)
{
    return CoreAngularVelocityToUE(Value);
}

} // namespace Snooker
