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
bool IsLineIntersect(const Line<T>& l1, const Line<T>& l2, T tol = 0.0001) {
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
 * @note this is a convenient method. If you need, use `LineIntersect()` and
 * check the return params yourself
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
                                             const Line<T>& l2, T tol) {
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
bool IsTriangleContain(const Container& tri, const cgmath::Vec<T, 2>& pt) {
    static_assert(
        std::is_same_v<typename Container::value_type, cgmath::Vec<T, 2>>);
    Assert(tri.size() == 3, "geometry is not a triangle");

    char s1 = cgmath::Sign((tri[0] - pt).Cross(tri[1] - tri[0])),
         s2 = cgmath::Sign((tri[1] - pt).Cross(tri[2] - tri[1])),
         s3 = cgmath::Sign((tri[2] - pt).Cross(tri[0] - tri[2]));
    return (s1 > 0 && s2 > 0 && s3 > 0) || (s1 < 0 && s2 < 0 && s3 < 0);
}

/**
 * @brief a quick way to check a pt in/on triangle
 */
template <typename T, typename Container>
bool IsTriangleContainStrict(const Container& tri,
                             const cgmath::Vec<T, 2>& pt) {
    static_assert(
        std::is_same_v<typename Container::value_type, cgmath::Vec<T, 2>>);
    Assert(tri.size() == 3, "geometry is not a triangle");

    char s1 = cgmath::Sign((tri[0] - pt).Cross(tri[1] - tri[0])),
         s2 = cgmath::Sign((tri[1] - pt).Cross(tri[2] - tri[1])),
         s3 = cgmath::Sign((tri[2] - pt).Cross(tri[0] - tri[2]));
    return (s1 >= 0 && s2 >= 0 && s3 >= 0) || (s1 <= 0 && s2 <= 0 && s3 <= 0);
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
    T max = polygon[0].Dot(dir);

    for (size_t i = 1; i < polygon.size(); i++) {
        auto value = polygon[i].Dot(dir);
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
    struct Vertex {
        T u;  // p1 * u + p2 * (1-u)
        size_t idx1;
        size_t idx2;
        cgmath::Vec<T, 2> p;  // p = p2 - p1
    };

    bool Contain(const cgmath::Vec<T, 2>& pt) const {
        return IsTriangleContainStrict(
            std::array{datas_[0].p, datas_[1].p, datas_[2].p}, pt);
    }

    bool HasVertex(const cgmath::Vec<T, 2>& pt, T tol = 0.001) const {
        return geom::IsSamePt(pt, datas_[0].p, tol) ||
               geom::IsSamePt(pt, datas_[1].p, tol) ||
               geom::IsSamePt(pt, datas_[2].p, tol);
    }

    const auto& operator[](size_t idx) const { return datas_[idx]; }

    auto& operator[](size_t idx) {
        return const_cast<Vertex&>(std::as_const(*this).operator[](idx));
    }

    cgmath::Vec<T, 2> GetSearchDir(const cgmath::Vec<T, 2>& p) {
        switch (count) {
            case 1:
                return p - datas_[0].p;
            case 2: {
                auto v = datas_[1].p - datas_[0].p;
                if (v.Cross(p - datas_[0].p) >= 0) {
                    return cgmath::Vec<T, 2>{-v.y, v.x};
                } else {
                    return cgmath::Vec<T, 2>{v.y, -v.x};
                }
            }
            default:
                Assert(false, "reached invalid switch case");
        }
        return cgmath::Vec<T, 2>{0, 0};
    }

    std::pair<cgmath::Vec<T, 2>, cgmath::Vec<T, 2>> GetWitnessPoints(
        const std::vector<cgmath::Vec<T, 2>>& polygon1,
        const std::vector<cgmath::Vec<T, 2>>& polygon2) const {
        cgmath::Vec<T, 2> p1, p2;
        auto s = 1.0 / divisor;
        for (int i = 0; i < count; i++) {
            p1 += datas_[i].u * polygon1[datas_[datas_[i].idx1]];
            p2 += datas_[i].u * polygon2[datas_[datas_[i].idx2]];
        }

        return {p1, p2};
    }

    int count = 0;
    T divisor = 1.0;

private:
    std::array<Vertex, 3> datas_;
};

/**
 * @brief use GJK(Gilbert–Johnson–Keerthi) algorithm to check whether two
 * polygon is intersected
 *
 * @ref https://dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
 */
template <typename T>
bool Gjk(const std::vector<cgmath::Vec<T, 2>>& polygon1,
         const std::vector<cgmath::Vec<T, 2>>& polygon2) {
    if (polygon1.empty() || polygon2.empty()) {
        return false;
    }

    // init simplex
    Simplex<T, 2> simplex;
    auto dir = cgmath::Vec<T, 2>{1, 0};
    simplex[0].p = polygon1[GetSupportPt(polygon1, dir)] -
                   polygon2[GetSupportPt(polygon2, -dir)];
    simplex[1].p = simplex[0].p;
    simplex[2].p = simplex[0].p;
    dir = -simplex[0].p;

    while (true) {
        auto supportPt = polygon1[GetSupportPt(polygon1, dir)] -
                         polygon2[GetSupportPt(polygon2, -dir)];
        if (simplex.HasVertex(supportPt)) {
            return false;
        }

        // overlap check
        if (supportPt.Dot(dir) <= 0) {
            return false;
        }

        // contain check
        simplex[2].p = supportPt;
        if (simplex.Contain(cgmath::Vec2{0, 0})) {
            return true;
        }

        // nearest simplex calculate
        auto v1 = cgmath::Vec<T, 3>{simplex[0].p - supportPt},
             v2 = cgmath::Vec<T, 3>{simplex[1].p - supportPt};
        auto n1 = cgmath::Vec<T, 3>{cgmath::Normalize(v1)}.Cross(
                 cgmath::Vec<T, 3>(-supportPt)),
             n2 = cgmath::Vec<T, 3>{cgmath::Normalize(v2)}.Cross(
                 cgmath::Vec<T, 3>(-supportPt));

        if (n1.LengthSqrd() == 0 || n2.LengthSqrd() == 0) {
            return true;
        }

        if (n1.LengthSqrd() <= n2.LengthSqrd()) {
            simplex[1].p = supportPt;
            dir = cgmath::Vec2{n1.Cross(v1)};
        } else {
            simplex[0].p = supportPt;
            dir = cgmath::Vec2{n2.Cross(v2)};
        }
    }
}

template <typename T>
struct NearestPtInfo final {
    std::pair<size_t, size_t> edge;
    T param1;
    T param2;

    cgmath::Vec<T, 2> GetPt(const std::vector<cgmath::Vec<T, 2>>& polygon) {
        return polygon[edge.first] * param1 + polygon[edge.second] * param2;
    }
};

// some function for GJKNearestPt
namespace internal {

template <typename T>
void gjkSolve2(Simplex<T, 2>& simplex, const cgmath::Vec<T, 2>& pt) {
    auto axis = simplex[1].p - simplex[0].p;
    auto lenSqrd = axis.LengthSqrd();

    if (lenSqrd == 0) {
        simplex.count = 1;
        simplex[0].u = 1;
        simplex.divisor = 1;
        return;
    }

    auto u = (pt - simplex[0].p).Dot(axis);
    auto v = (pt - simplex[1].p).Dot(-axis);

    if (u < 0) {
        simplex.count = 1;
        simplex[0].u = 1;
        simplex.divisor = 1;
        return;
    }

    if (v < 0) {
        simplex.count = 1;
        simplex[0] = simplex[1];
        simplex[0].u = 1;
        simplex.divisor = 1;
        return;
    }

    simplex[0].u = v;
    simplex[1].u = u;
    simplex.divisor = lenSqrd;
    simplex.count = 2;
}

template <typename T>
void gjkSolve3(Simplex<T, 2>& simplex, const cgmath::Vec<T, 2>& pt) {
    auto AB = simplex[1].p - simplex[0].p;
    auto BC = simplex[2].p - simplex[1].p;
    auto CA = simplex[0].p - simplex[2].p;

    auto uAB = (pt - simplex[0].p).Dot(AB);
    auto vAB = (pt - simplex[1].p).Dot(-AB);
    auto uBC = (pt - simplex[1].p).Dot(BC);
    auto vBC = (pt - simplex[2].p).Dot(-BC);
    auto uCA = (pt - simplex[2].p).Dot(CA);
    auto vCA = (pt - simplex[0].p).Dot(-CA);

    // at A
    if (uAB <= 0 && vCA <= 0) {
        simplex.count = 1;
        simplex[0].u = 1;
        simplex.divisor = 1.0;
        return;
    }

    // at B
    if (vAB <= 0 && uBC <= 0) {
        simplex.count = 1;
        simplex[0] = simplex[1];
        simplex[0].u = 1;
        simplex.divisor = 1.0;
        return;
    }

    // at C
    if (uCA <= 0 && vBC <= 0) {
        simplex.count = 1;
        simplex[0] = simplex[2];
        simplex[0].u = 1;
        simplex.divisor = 1.0;
        return;
    }

    // calculate barycentric params
    auto area = (simplex[1].p - simplex[0].p).Cross(simplex[2].p - simplex[0].p);
    auto uABC = (simplex[1].p - pt).Cross(simplex[2].p - pt);
    auto vABC = (simplex[2].p - pt).Cross(simplex[0].p - pt);
    auto wABC = (simplex[0].p - pt).Cross(simplex[1].p - pt);

    // at AB
    if (uAB > 0 && vAB > 0 && wABC * area <= 0) {
        simplex.count = 2;
        simplex[0].u = vAB;
        simplex[1].u = uAB;
        simplex.divisor = AB.LengthSqrd();
        return;
    }

    // at BC
    if (uBC > 0 && vBC > 0 && uABC * area <= 0) {
        simplex.count = 2;
        simplex[0] = simplex[1];
        simplex[1] = simplex[2];
        simplex[0].u = vBC;
        simplex[1].u = uBC;
        simplex.divisor = BC.LengthSqrd();
        return;
    }

    // at CA
    if (uCA > 0 && vCA > 0 && vABC * area <= 0) {
        simplex.count = 2;
        simplex[1] = simplex[0];
        simplex[0] = simplex[2];
        simplex[0].u = vCA;
        simplex[1].u = uCA;
        simplex.divisor = CA.LengthSqrd();
        return;
    }

    // in ABC
    simplex[0].u = uABC;
    simplex[1].u = vABC;
    simplex[2].u = wABC;
    simplex.divisor = area;
    simplex.count = 3;
}

}  // namespace internal

/**
 * use GJK algorithm to get nearest point between polygons
 *
 * @ref theory ref: https://dyn4j.org/2010/04/gjk-distance-closest-points/
 *                  https://box2d.org/files/ErinCatto_GJK_GDC2010.pdf
 * @ref code ref: https://box2d.org/files/ErinCatto_GJK_Source.zip
 */
template <typename T>
std::optional<std::pair<NearestPtInfo<T>, NearestPtInfo<T>>> GjkNearestPt(
    const std::vector<cgmath::Vec<T, 2>>& polygon1,
    const std::vector<cgmath::Vec<T, 2>>& polygon2) {
    Simplex<T, 2> simplex;
    cgmath::Vec<T, 2> dir{1, 0};
    simplex[0].idx1 = GetSupportPt(polygon1, dir);
    simplex[0].idx2 = GetSupportPt(polygon1, -dir);
    simplex[0].p = polygon1[simplex[0].idx1] - polygon2[simplex[0].idx2];
    simplex[0].u = 1;
    simplex.count = 1;

    size_t save1[3] = {0};  // saved idx1
    size_t save2[3] = {0};  // saved idx2
    int saveCount = 0;

    while (true) {
        saveCount = simplex.count;
        for (int i = 0; i < simplex.count; i++) {
            save1[i] = simplex[i].idx1;
            save2[i] = simplex[i].idx2;
        }

        switch (simplex.count) {
            case 1:
                break;
            case 2:
                internal::gjkSolve2(simplex, cgmath::Vec2{0, 0});
                break;
            case 3:
                internal::gjkSolve3(simplex, cgmath::Vec2{0, 0});
                break;
            default:
                Assert(false, "reached invalid switch case");
        }

        // triangle contain origin, polygons are intersected
        if (simplex.count == 3) {
            return std::nullopt;
        }

        dir = simplex.GetSearchDir(cgmath::Vec<T, 2>{});

        if (dir.LengthSqrd() == 0) {
            return std::nullopt;
        }

        auto& newVertex = simplex[simplex.count];
        newVertex.idx1 = GetSupportPt(polygon1, dir);
        newVertex.idx2 = GetSupportPt(polygon2, -dir);
        newVertex.p = polygon1[newVertex.idx1] - polygon2[newVertex.idx2];

        bool duplicate = false;
        for (int i = 0; i < saveCount; i++) {
            if (newVertex.idx1 == save1[i] && newVertex.idx2 == save2[i]) {
                duplicate = true;
                break;
            }
        }

        if (duplicate) {
            break;
        }

        simplex.count ++;
    }

    NearestPtInfo<T> info1, info2;
    if (simplex.count == 1) {
        info1.param1 = 1;
        info1.param2 = 0;
        info1.edge.first = simplex[0].idx1;
        info1.edge.second = simplex[1].idx1;

        info2.param1 = 1;
        info2.param2 = 0;
        info2.edge.first = simplex[0].idx2;
        info2.edge.second = simplex[1].idx2;
    } else if (simplex.count == 2) {
        auto s = 1.0 / simplex.divisor;
        info1.param1 = simplex[0].u * s;
        info1.param2 = simplex[1].u * s;
        info1.edge.first = simplex[0].idx1;
        info1.edge.second = simplex[1].idx1;

        info2.param1 = simplex[0].u * s;
        info2.param2 = simplex[1].u * s;
        info2.edge.first = simplex[0].idx2;
        info2.edge.second = simplex[1].idx2;
    } else {
        Assert(false, "simplex has 3 vertex when quit GJK!");
    }

    return std::make_pair(info1, info2);
}

/**
 * @brief Minimal Translation Vector for SAT
 */
template <typename T, CGMATH_LEN_TYPE N>
struct MTV final {
    cgmath::Vec<T, N> v;
    T len = 0.0;
};

namespace internal {

template <typename T>
std::pair<T, T> projectPolygonOnVec(
    const cgmath::Vec<T, 2>& start, const cgmath::Vec<T, 2>& v,
    const std::vector<cgmath::Vec<T, 2>>& polygon) {
    std::pair<T, T> range = {std::numeric_limits<T>::max(),
                             std::numeric_limits<T>::lowest()};

    for (int i = 0; i < polygon.size(); i++) {
        auto& p1 = polygon[i];
        auto& p2 = polygon[(i + 1) % polygon.size()];

        auto proj1 = (p1 - start).Dot(v);
        auto proj2 = (p2 - start).Dot(v);

        auto min = std::min(proj1, proj2);
        auto max = std::max(proj1, proj2);
        if (min < range.first) {
            range.first = min;
        }

        if (max > range.second) {
            range.second = max;
        }
    }

    return range;
}

template <typename T>
std::optional<MTV<T, 2>> checkSAT2Polygon(
    const std::vector<cgmath::Vec<T, 2>>& polygon1,
    const std::vector<cgmath::Vec<T, 2>>& polygon2) {
    cgmath::Vec<T, 2> info;
    T len = std::numeric_limits<T>::max();

    for (int i = 0; i < polygon1.size(); i++) {
        auto axis = cgmath::Normalize(polygon1[(i + 1) % polygon1.size()] -
                                      polygon1[i]);
        auto perp = cgmath::PerpendicVec(axis);

        auto range1 =
            internal::projectPolygonOnVec(polygon1[i], perp, polygon1);
        auto range2 =
            internal::projectPolygonOnVec(polygon1[i], perp, polygon2);

        if (!cgmath::IsOverlap(range1.first, range1.second, range2.first,
                               range2.second)) {
            return std::nullopt;
        } else {
            auto minLen = std::min(std::abs(range1.second - range2.first),
                                   std::abs(range2.second - range1.first));
            if (minLen < len) {
                len = minLen;
            }
        }
    }

    return MTV<T, 2>{info, len};
}

}  // namespace internal

/**
 * @brief use Separate Axis Theory to detect polygon's depth vector
 */
template <typename T>
std::optional<MTV<T, 2>> SAT(const std::vector<cgmath::Vec<T, 2>>& polygon1,
                             const std::vector<cgmath::Vec<T, 2>>& polygon2) {
    auto mtv1 = internal::checkSAT2Polygon(polygon1, polygon2);
    if (!mtv1) {
        return std::nullopt;
    }
    auto mtv2 = internal::checkSAT2Polygon(polygon2, polygon1);
    if (!mtv2) {
        return std::nullopt;
    }

    if (mtv1->len < mtv2->len) {
        return mtv1;
    }

    return mtv2;
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
    template <typename Container>
    static Barycentric FromTriangle(const Container& tri,
                                    const cgmath::Vec<T, 2>& p) {
        static_assert(
            std::is_same_v<typename Container::value_type, cgmath::Vec<T, 2>>);
        T area = (tri[1] - tri[0]).Cross(tri[2] - tri[0]);
        T a = (tri[1] - p).Cross(tri[2] - p) / area;
        T b = (tri[2] - p).Cross(tri[0] - p) / area;
        T c = (tri[0] - p).Cross(tri[1] - p) / area;

        return {a, b, c};
    }

    /**
     * @brief create barycentric from a triangle and a point
     * @note the p must in triangle, it will more quickly to `FromTriangle()`
     * @see FromTriangle
     */
    static Barycentric FromOuterTriangle(
        const std::vector<cgmath::Vec<T, 2>>& tri, const cgmath::Vec<T, 2>& p) {
        T area = (tri[1] - tri[0]).Cross(tri[2] - tri[0]);
        auto a = (tri[1] - p).Cross(tri[2] - p) / area;
        auto b = (tri[2] - p).Cross(tri[0] - p) / area;

        return {a, b};
    }

    operator bool() const {
        return a >= 0 && b >= 0 && c >= 0 && a + b + c == 1;
    }

    T a = 0, b = 0, c = 0;
};

template <typename T, typename Container>
cgmath::Vec<T, 2> TriangleNearestPt(const Container& tri,
                                    const cgmath::Vec<T, 2>& pt) {
    static_assert(
        std::is_same_v<typename Container::value_type, cgmath::Vec<T, 2>>);
    auto bary = Barycentric<T>::FromTriangle(tri, pt);
    if (bary) {
        return pt;
    }

    if (bary.a < 0) {
        auto v = tri[2] - tri[1];
        T param = (pt - tri[1]).Dot(v) / v.LengthSqrd();
        if (param <= 0) {
            return tri[1];
        } else if (param >= 1) {
            return tri[2];
        } else {
            return tri[1] + v * param;
        }
    } else if (bary.b < 0) {
        auto v = tri[2] - tri[0];
        T param = (pt - tri[0]).Dot(v) / v.LengthSqrd();
        if (param <= 0) {
            return tri[0];
        } else if (param >= 1) {
            return tri[2];
        } else {
            return tri[0] + v * param;
        }
    } else {
        auto v = tri[1] - tri[0];
        T param = (pt - tri[0]).Dot(v) / v.LengthSqrd();
        if (param <= 0) {
            return tri[0];
        } else if (param >= 1) {
            return tri[1];
        } else {
            return tri[0] + v * param;
        }
    }
}

template <typename T>
Line<T> PerpendicularLine(const Line<T>& l) {
    return Line<T>::FromDir(l.p, cgmath::Vec<T, 2>{l.dir.y, -l.dir.x});
}

}  // namespace geom2d

}  // namespace nickel
