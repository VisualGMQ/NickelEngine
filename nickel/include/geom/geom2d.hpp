#pragma once

#include "core/cgmath.hpp"
#include "geom/basic_geom.hpp"

namespace nickel {

namespace geom2d {

template <typename T>
using AABB = geom::AABB<T, 2>;

template <typename T>
using Circle = geom::Circular<T, 2>;

template <typename T>
using Plane = geom::Plane<T, 2>;

template <typename T>
using Line = geom::Line<T, 2>;

template <typename T>
using Segment = geom::Segment<T, 2>;

template <typename T>
using Ray = geom::Ray<T, 2>;

template <typename T>
using Capsule = geom::Capsule<T, 2>;

// function fwd

template <typename T>
bool IsLineParallel(const Line<T>& l1, const Line<T>& l2, T tol = 0.0001);

template <typename T>
std::optional<std::pair<T, T>> LineIntersect(const Line<T>&, const Line<T>&,
                                             T tol = 0.0001);

// distance

/**
 * @brief get distance between point and line in 2D
 */
template <typename T>
T LinePtDist(const Line<T>& l, const cgmath::Vec<T, 2>& pt) {
    return (pt - l.p).Cross(l.dir);
}

template <typename T>
T LineDist(const Line<T>& l1, const Line<T>& l2) {
    return LinePtDist(l1, l2.p);
}

// intersection check

/**
 * @brief quickly check whether lines intersect(without get intersection)
 * @note this is a quick check, if you want to get intersection same time, use
 * `LineIntersect()`
 */
template <typename T>
bool IsLineIntersect(const Line<T>& l1, const Line<T>& l2,
                     T tol = 0.0001) {
    return !IsLineParallel(l1, l2, tol);
}

/**
 * @brief quick check whether segments intersect(without get intersection)
 */
template <typename T>
bool IsSegIntersect(const Segment<T>& l1, const Segment<T>& l2) {
    auto p1 = l1.p + l1.dir * l1.len;
    auto p2 = l2.p + l2.dir * l2.len;

    return (l1.p - l2.p).Cross(l2.dir) * (p1 - l2.p).Cross(l2.dir) <= 0 &&
           (l2.p - l1.p).Cross(l1.dir) * (p2 - l1.p).Cross(l1.dir) <= 0;
}

/**
 * @brief check whether rays intersect(without get intersection)
 * @note this is a convient method. If you need, use `LineIntersect()` and check
 * the return params yourself
 */
template <typename T>
bool IsRayIntersect(const Ray<T>& l1, const Ray<T>& l2) {
    if (auto result = LineIntersect(l1, l2); result) {
        auto&& [param1, param2] = result.value();
        return param1 >= 0 && param2 >= 0;
    }
    return false;
}

// intersection

/**
 * @brief get intersection between point and line, get param in l1 & l2
 * optionally
 *
 * @tparam T
 * @param l1 line1
 * @param l2 line2
 * @param tol tolerance
 * @return std::optional<std::pair<T, T>> the param on l1 and l2
 */
template <typename T>
std::optional<std::pair<T, T>> LineIntersect(const Line<T>& l1,
                                             const Line<T>& l2,
                                             T tol) {
    auto det = l1.dir.Cross(-l2.dir);
    if (cgmath::IsSameValue<T>(det, 0, tol)) {
        return std::nullopt;
    }

    auto diffP = l2.p - l1.p;
    auto param1 = diffP.Cross(-l2.dir) / det;
    auto param2 = l1.dir.Cross(diffP) / det;

    return std::pair<T, T>(param1, param2);
}

/**
 * @brief a quick way to check a pt in triangle
 */
template <typename T, typename Container>
bool IsTriangleContain(const Container& tri,
                       const cgmath::Vec<T, 2>& pt) {
    static_assert(std::is_same_v<typename Container::value_type, cgmath::Vec<T, 2>>);
    Assert(tri.size() == 3, "geomentry is not a triangle");

    char s1 = cgmath::Sign((tri[0] - pt).Cross(tri[1] - tri[0])),
         s2 = cgmath::Sign((tri[1] - pt).Cross(tri[2] - tri[1])),
         s3 = cgmath::Sign((tri[2] - pt).Cross(tri[0] - tri[2]));
    return (s1 > 0 && s2 > 0 && s3 > 0) || (s1 < 0 && s2 < 0 && s3 < 0);
}

// misc

template <typename T>
bool IsLineParallel(const Line<T>& l1, const Line<T>& l2, T tol) {
    return cgmath::IsSameValue<T>(l1.dir.Cross(l2.dir), 0, tol);
}

template <typename T>
size_t GetSupportPt(const std::vector<cgmath::Vec<T, 2>>& polygon,
                    const cgmath::Vec<T, 2>& dir) {
    Assert(!polygon.empty(), "polygon has no vertex");

    size_t idx = 0;
    T max = std::numeric_limits<T>::min();

    for (size_t i = 1; i < polygon.size(); i++) {
        auto value = (polygon[i] - polygon[0]).Dot(dir);
        if (value > max) {
            max = value;
            idx = i;
        }
    }

    return idx;
}

template <typename T, size_t N>
class Simplex;

template <typename T>
class Simplex<T, 2> final {
public:
    bool Contain(const cgmath::Vec<T, 2>& pt) const {
        return IsTriangleContain(datas_, pt);
    }

    bool HasVertex(const cgmath::Vec<T, 2>& pt, T tol = 0.001) const {
        return geom::IsSamePt(pt, datas_[0], tol) ||
               geom::IsSamePt(pt, datas_[1], tol) ||
               geom::IsSamePt(pt, datas_[2], tol);
    }

    const cgmath::Vec<T, 2>& operator[](size_t idx) const {
        return datas_[idx];
    }

    cgmath::Vec<T, 2>& operator[](size_t idx) {
        return const_cast<cgmath::Vec<T, 2>&>(
            std::as_const(*this).operator[](idx));
    }

private:
    std::array<cgmath::Vec<T, 2>, 3> datas_;
};

template <typename T>
bool Gjk(const std::vector<cgmath::Vec<T, 2>>& polygon1,
         const std::vector<cgmath::Vec<T, 2>>& polygon2) {
    if (polygon1.empty() || polygon2.empty()) {
        return false;
    }

    // init original based polygon
    std::vector<cgmath::Vec<T, 2>> oPolygon1 = polygon1, oPolygon2 = polygon2;
    cgmath::Vec<T, 2> center1 = std::accumulate(oPolygon1.begin(),
                                                oPolygon1.end(), cgmath::Vec<T, 2>{0, 0}) /
                                oPolygon1.size(),
                      center2 = std::accumulate(oPolygon2.begin(),
                                                oPolygon2.end(), cgmath::Vec<T, 2>{0, 0}) /
                                oPolygon2.size();

    for (auto& elem : oPolygon1) {
        elem -= center1;
    }

    for (auto& elem : oPolygon2) {
        elem -= center2;
    }

    // init simplex
    Simplex<T, 2> simplex;
    auto dir = cgmath::Vec<T, 2>{1, 0};
    simplex[0] = polygon1[GetSupportPt(oPolygon1, dir)] -
                 polygon2[GetSupportPt(oPolygon2, -dir)];
    simplex[1] = simplex[0];
    dir = -simplex[0];

    cgmath::Vec<T, 2> supportPt;
    bool isTwoPts = true;

    while (true) {
        auto supportPt = polygon1[GetSupportPt(oPolygon1, dir)] -
                         polygon2[GetSupportPt(oPolygon2, -dir)];

        if (simplex.HasVertex(supportPt)) {
            return false;
        }

        // overlap check
        if (supportPt.Dot(dir) <= 0) {
            return false;
        }

        // contain check
        simplex[2] = supportPt;
        if (simplex.Contain(cgmath::Vec2{0, 0})) {
            return true;
        }

        // nearest simplex calcualte
        if (isTwoPts) {
            auto v1 = cgmath::Vec3{simplex[0] - supportPt};
            auto v2 = cgmath::Vec3{-supportPt};
            dir = cgmath::Vec2{cgmath::TripleCross(v1, v2, v1)};
            if (dir == cgmath::Vec<T, 2>{0, 0}) {
                return true;
            }
            simplex[1] = supportPt;
            isTwoPts = false;
        } else {
            auto v1 = cgmath::Vec<T, 3>{simplex[0] - supportPt},
                 v2 = cgmath::Vec<T, 3>{simplex[1] - supportPt};
            auto n1 = cgmath::Vec<T, 3>{cgmath::Normalize(v1)}.Cross(
                     cgmath::Vec<T, 3>(-supportPt)),
                 n2 = cgmath::Vec<T, 3>{cgmath::Normalize(v2)}.Cross(
                     cgmath::Vec<T, 3>(-supportPt));

            if (n1 == cgmath::Vec<T, 3>{0, 0} || n2 == cgmath::Vec<T, 3>{0, 0}) {
                return true;
            }

            if (n1.LengthSqrd() <= n2.LengthSqrd()) {
                simplex[1] = supportPt;
                dir = cgmath::Vec2{n1.Cross(v1)};
            } else {
                simplex[0] = supportPt;
                dir = cgmath::Vec2{n2.Cross(v2)};
            }
        }
    }

    return false;
}

template <typename T>
struct Barycentric final {
    Barycentric() = default;

    Barycentric(T a, T b, T c) : a(a), b(b), c(c) {}

    Barycentric(T a, T b) : a(a), b(b), c(1.0 - a - b) {}

    /**
     * @brief create barycentric from a triangle and a point
     * @note the p may not in triangle
     * @see FromOuterTriangle
     */
    static Barycentric FromTriangle(const std::vector<cgmath::Vec<T, 2>>& tri,
                                    const cgmath::Vec<T, 2>& p) {
        T area = std::abs(tri[1] - tri[0]).Cross(tri[2] - tri[0]);
        a = std::abs((p - tri[0]).Cross(p - tri[1])) / area;
        b = std::abs((p - tri[1]).Cross(p - tri[2])) / area;
        c = std::abs((p - tri[2]).Cross(p - tri[0])) / area;

        return {a, b, c};
    }

    /**
     * @brief create barycentric from a triangle and a point
     * @note the p must in triangle, it will more quickly to `FromTriangle()`
     * @see FromTriangle
     */
    static Barycentric FromOuterTriangle(
        const std::vector<cgmath::Vec<T, 2>>& tri, const cgmath::Vec<T, 2>& p) {
        T area = std::abs(tri[1] - tri[0]).Cross(tri[2] - tri[0]);
        a = std::abs((p - tri[0]).Cross(p - tri[1])) / area;
        b = std::abs((p - tri[1]).Cross(p - tri[2])) / area;

        return {a, b};
    }

    operator bool() const {
        return a >= 0 && b >= 0 && c >= 0 && a + b + c == 1;
    }

    T a = 0, b = 0, c = 0;
};

}  // namespace geom2d

}  // namespace nickel
