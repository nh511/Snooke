#pragma once
#include "SnookerUnits.h"

namespace Snooker
{
struct Vec2d
{
    Scalar X = 0, Y = 0;
    constexpr Vec2d operator+(const Vec2d& b) const { return {X+b.X, Y+b.Y}; }
    constexpr Vec2d operator-(const Vec2d& b) const { return {X-b.X, Y-b.Y}; }
    constexpr Vec2d operator*(Scalar s) const { return {X*s, Y*s}; }
    constexpr Scalar Dot(const Vec2d& b) const { return X*b.X+Y*b.Y; }
    Scalar Length() const { return std::hypot(X,Y); }
    bool IsFinite() const { return Snooker::IsFinite(X) && Snooker::IsFinite(Y); }
};

struct Quatd
{
    Scalar X = 0, Y = 0, Z = 0, W = 1;
    bool IsFinite() const
    {
        return Snooker::IsFinite(X) && Snooker::IsFinite(Y) && Snooker::IsFinite(Z) && Snooker::IsFinite(W);
    }
    bool IsUnit() const { return IsFinite() && NearlyEqual(X*X+Y*Y+Z*Z+W*W, 1.0); }
    Quatd Conjugate() const { return {-X,-Y,-Z,W}; }
    Quatd operator*(const Quatd& b) const
    {
        return {W*b.X+X*b.W+Y*b.Z-Z*b.Y, W*b.Y-X*b.Z+Y*b.W+Z*b.X,
                W*b.Z+X*b.Y-Y*b.X+Z*b.W, W*b.W-X*b.X-Y*b.Y-Z*b.Z};
    }
    // Rotation APIs require a unit quaternion; callers can reject invalid inputs with IsUnit().
    Vec3d Rotate(const Vec3d& v) const
    {
        const Vec3d q{X,Y,Z};
        return v + q.Cross(v) * (2*W) + q.Cross(q.Cross(v))*2;
    }
    static bool TryFromAxisAngle(const Vec3d& axis, Radians angle, Quatd& output)
    {
        const Scalar length = std::hypot(axis.X, axis.Y, axis.Z);
        if (!axis.IsFinite() || !Snooker::IsFinite(angle) || !Snooker::IsFinite(length) || length <= 1e-12) return false;
        const auto s = std::sin(angle/2);
        output = {axis.X/length*s, axis.Y/length*s, axis.Z/length*s, std::cos(angle/2)};
        return true;
    }
};

struct Transformd
{
    Vec3d Translation;
    Quatd Rotation;
    Vec3d Scale{1,1,1};
    bool IsValid() const { return Translation.IsFinite() && Scale.IsFinite() && Rotation.IsUnit(); }
    Vec3d TransformPoint(const Vec3d& p) const
    {
        return Translation + Rotation.Rotate({p.X*Scale.X,p.Y*Scale.Y,p.Z*Scale.Z});
    }
    bool TryInversePoint(const Vec3d& p, Vec3d& output) const
    {
        if (!IsValid() || !p.IsFinite() || Scale.X == 0 || Scale.Y == 0 || Scale.Z == 0) return false;
        const auto v = Rotation.Conjugate().Rotate(p-Translation);
        const Vec3d candidate{v.X/Scale.X,v.Y/Scale.Y,v.Z/Scale.Z};
        if (!candidate.IsFinite()) return false;
        output = candidate;
        return true;
    }
};
}
