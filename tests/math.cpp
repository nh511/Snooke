#include "Snooker/Math/CoreTypes.h"
#include <iostream>
#include <limits>

#define CHECK(x) do { if (!(x)) { std::cerr << "FAIL " << __LINE__ << ": " << #x << '\n'; return 1; } } while(false)
int main()
{
    using namespace Snooker;
    const Vec2d a{3,4}, b{2,-1};
    CHECK(NearlyEqual(a.Length(),5));
    CHECK(NearlyEqual(a.Dot(b),2));
    CHECK(NearlyEqual((a+b).Y,3));
    const Vec3d x{1,0,0}, y{0,1,0};
    CHECK(x.Cross(y).NearlyEquals(TableNormal));
    CHECK(NearlyEqual(x.Dot(y),0));
    Quatd rotation;
    CHECK(Quatd::TryFromAxisAngle({0,0,2},Pi/2,rotation));
    CHECK(rotation.IsUnit());
    CHECK(rotation.Rotate(x).NearlyEquals(y));
    CHECK(rotation.Conjugate().Rotate(y).NearlyEquals(x));
    CHECK((rotation*rotation).Rotate(x).NearlyEquals({-1,0,0}));
    const auto saved=rotation;
    CHECK(!Quatd::TryFromAxisAngle({},1,rotation));
    CHECK(NearlyEqual(rotation.W,saved.W));
    CHECK(!Quatd::TryFromAxisAngle(x,std::numeric_limits<double>::infinity(),rotation));
    Transformd transform{{2,-3,1},rotation,{2,3,4}};
    Vec3d inverse;
    const Vec3d point{1,2,3};
    CHECK(transform.TryInversePoint(transform.TransformPoint(point),inverse));
    CHECK(inverse.NearlyEquals(point));
    transform.Scale.Z=0;
    CHECK(!transform.TryInversePoint(point,inverse));
    CHECK(inverse.NearlyEquals(point));
    std::cout << "PASS vector, quaternion, transform and invalid input tests\n";
}
