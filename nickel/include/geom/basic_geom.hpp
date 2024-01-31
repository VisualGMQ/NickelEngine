#pragma once

#include "common/cgmath.hpp"

namespace nickel {

namespace geom {

template <typename T, uint32_t N>
struct AABB final {
    cgmath::Vec<T, N> center;
    cgmath::Vec<T, N> halfLen;

    static AABB FromCenter(const cgmath::Vec<T, N>& center,
                           const cgmath::Vec<T, N>& halfLen) {
        return AABB{center, halfLen};
    }

    static AABB FromCorner(const cgmath::Vec<T, N>& corner,
                           const cgmath::Vec<T, N>& halfLen) {
        return AABB{corner + halfLen, halfLen};
    }

    static AABB FromMinMax(const cgmath::Vec<T, N>& min,
                           const cgmath::Vec<T, N>& max) {
        return AABB{(max + min) * 0.5, (max - min) * 0.5};
    }

    static AABB Identity() { return AABB{{}, {0.5}}; }
};

template <typename T, uint32_t N>
struct OBB final {
    cgmath::Vec<T, N> center;
    cgmath::Vec<T, N> halfLen;
    cgmath::Vec<T, N> axis;

    static OBB FromCenter(const cgmath::Vec<T, N>& center,
                          const cgmath::Vec<T, N>& halfLen,
                          const cgmath::Vec<T, N>& axis) {
        return {center, halfLen, axis};
    }
};

template <typename T, uint32_t N>
struct Circular final {
    cgmath::Vec<T, N> center;
    T radius;

    static Circular Identity() { return {{}, 0.5}; }

    static Circular Create(const cgmath::Vec<T, N>& center, T radius) {
        return {center, radius};
    }
};

template <typename T, uint32_t N>
struct Plane final {
    cgmath::Vec<T, N> pt;
    cgmath::Vec<T, N> normal;
};

template <typename T, uint32_t N>
struct Line {
    cgmath::Vec<T, N> p;
    cgmath::Vec<T, N> dir;  // must normaized

    static Line FromDir(const cgmath::Vec<T, N>& p,
                        const cgmath::Vec<T, N>& dir) {
        return {p, dir};
    }

    static Line FromPts(const cgmath::Vec<T, N>& start,
                        const cgmath::Vec<T, N>& end) {
        return {start, cgmath::Normalize(end - start)};
    }
};

template <typename T, uint32_t N>
struct Segment final : public Line<T, N> {
    T len;

    static Segment FromPts(const cgmath::Vec<T, N>& p1,
                           const cgmath::Vec<T, N>& p2) {
        Segment seg;
        seg.p = p1;
        seg.dir = p2 - p1;
        seg.len = seg.dir.Length();
        seg.dir.Normalize();
        return seg;
        return {p1, p2 - p1};
    }
};

template <typename T, uint32_t N>
struct Ray final : public Line<T, N> {
    static Ray FromDir(const cgmath::Vec<T, N>& p,
                       const cgmath::Vec<T, N>& dir) {
        return {p, dir};
    }

    static Ray FromPts(const cgmath::Vec<T, N>& start,
                       const cgmath::Vec<T, N>& end) {
        return {start, cgmath::Normalize(end - start)};
    }
};

template <typename T, uint32_t N>
struct Capsule final {
    Segment<T, N> seg;
    T radius;

    static Capsule Create(const Segment<T, N>& s, T radius) {
        return {s, radius};
    }

    static Capsule Create(const cgmath::Vec<T, N>& p1,
                          const cgmath::Vec<T, N>& p2, T radius) {
        return {Segment<T, N>::FromPts(p1, p2), radius};
    }
};

template <typename T, uint32_t N>
uint32_t GetSupportPt(const std::vector<cgmath::Vec<T, N>>& vertices,
                      cgmath::Vec<T, N>& dir) {
    uint32_t idx = 0;
    T dot = std::numeric_limits<T>::min();

    for (uint32_t i = 0; i < vertices.size(); i++) {
        T newDot = cgmath::Dot(vertices[i], dir);
        if (newDot > dot) {
            dot = newDot;
            idx = i;
        }
    }

    return idx;
}

// nearest
/**
 * get nearest point on/in AABB(if point in AABB, return point itself)
 */
template <typename T, uint32_t N>
cgmath::Vec<T, N> AABBNearestPt(const AABB<T, N>& a,
                                const cgmath::Vec<T, N>& pt) {
    cgmath::Vec<T, N> result;
    for (int i = 0; i < N; i++) {
        result[i] = std::clamp(pt[i], a.center[i] - a.halfLen[i],
                               a.center[i] + a.halfLen[i]);
    }
    return result;
}

/**
 * @brief the nearest point to circular(if pt in circular, return pt itself)
 * @return cgmath::Vec<T, N>
 */
template <typename T, uint32_t N>
cgmath::Vec<T, N> CircularNearestPt(const Circular<T, N>& c,
                                    const cgmath::Vec<T, N>& pt) {
    auto dir = pt - c.center;
    auto len = dir.Length();
    return len <= c.radius ? pt : dir / len * c.radius + c.center;
}

template <typename T, uint32_t N>
cgmath::Vec<T, N> LineNearestPt(const Line<T, N>& line,
                                const cgmath::Vec<T, N>& pt) {
    return (pt - line.p).Dot(line.dir) * line.dir + line.p;
}

template <typename T, uint32_t N>
T SegNearestPtParam(const Segment<T, N>& s, const cgmath::Vec<T, N>& pt) {
    return std::clamp<T>((pt - s.p).Dot(s.dir), 0, 1);
}

template <typename T, uint32_t N>
cgmath::Vec<T, N> SegNearestPt(const Segment<T, N>& s,
                               const cgmath::Vec<T, N>& pt) {
    auto proj = (pt - s.p).Dot(s.dir);
    if (proj >= 0 && proj <= s.len) {
        return s.p + s.dir * proj;
    } else if (proj < 0) {
        return s.p;
    } else {
        return s.p + s.dir * s.len;
    }
}

template <typename T, unsigned int N>
cgmath::Vec<T, N> RayNearestPt(const Ray<T, N>& s,
                               const cgmath::Vec<T, N>& pt) {
    auto proj = (pt - s.p).Dot(s.dir);
    if (proj > 0) {
        return s.p + s.dir * proj;
    } else {
        return s.p;
    }
}

template <typename T, uint32_t N>
cgmath::Vec<T, N> CapsuleNearestPt(const Capsule<T, N>& c,
                                   const cgmath::Vec<T, N>& pt) {
    auto proj = (pt - c.seg.p).Dot(c.seg.dir);
    if (proj >= 0 && proj <= c.seg.len) {
        auto nearestPtOnLine = c.seg.p + c.seg.dir * proj;
        auto lineNorm = pt - nearestPtOnLine;
        auto len = lineNorm.LengthSqrd();
        if (len <= c.radius * c.radius) {
            return pt;
        } else {
            lineNorm.Normalize();
            return nearestPtOnLine + cgmath::Normalize(lineNorm) * c.radius;
        }
    } else if (proj < 0) {
        return CircularNearestPt(Circular<T, N>::Create(c.seg.p, c.radius), pt);
    } else {
        return CircularNearestPt(
            Circular<T, N>::Create(c.seg.p + c.seg.dir * c.seg.len, c.radius),
            pt);
    }
}

// contain

template <typename T, uint32_t N>
bool IsCapsuleContain(const Capsule<T, N>& cap, const cgmath::Vec<T, N>& pt) {
    return SegNearestPt(cap.seg, pt).LengthSqrd() <= cap.radius * cap.radius;
}

template <typename T, uint32_t N>
bool IsCircularContain(const Circular<T, N>& c, const cgmath::Vec<T, N>& pt) {
    return (pt - c.center).LengthSqrd() <= c.radius * c.radius;
}

template <typename T, uint32_t N>
bool IsAABBContain(const AABB<T, N>& a, const cgmath::Vec<T, N>& b) {
    for (uint32_t i = 0; i < N; i++) {
        if (b[i] < a.center[i] - a.halfLen[i] ||
            b[i] > a.center[i] + a.halfLen[i]) {
            return false;
        }
    }
    return true;
}

// distance

/**
 * @brief get distance between point and line
 * @note this is a generic implementation. Use `geom2d::LinePtDist()` or
 * `geom3d::LinePtDistSqrd()` for more efficiency
 */
template <typename T, uint32_t N>
T LinePtDistSqrd(const Line<T, N>& l, const cgmath::Vec<T, N>& pt) {
    auto ptDir = pt - l.p;
    auto projDir = l.dir.Dot(l.dir) * l.dir;
    return (ptDir - projDir).LengthSqrd();
}

// intersect check

// fwd
template <typename T, uint32_t N>
T PtDistSqrd(const cgmath::Vec<T, N>& v1, const cgmath::Vec<T, N>& v2);

template <typename T, uint32_t N>
bool IsCircularIntersect(const Circular<T, N>& c1, const Circular<T, N>& c2) {
    return PtDistSqrd(c1.center, c2.center) <
           (c1.radius + c2.radius) * (c1.radius + c2.radius);
}

/**
 * @brief check two capsules area intersected
 * @ref https://wickedengine.net/2020/04/26/capsule-collision-detection/
 */
template <typename T, uint32_t N>
bool IsCapsuleIntersect(const Capsule<T, N>& c1, const Capsule<T, N>& c2) {
    auto p1A = c1.seg.p, p1B = c1.seg.p + c1.seg.dir * c1.seg.len,
         p2A = c2.seg.p, p2B = c2.seg.p + c2.seg.dir * c2.seg.len;
    auto v0 = p1A - p2A, v1 = p1B - p2A, v2 = p1A - p2B, v3 = p1B - p2B;
    auto d0 = v0.LengthSqrd(), d1 = v1.LengthSqrd(), d2 = v2.LengthSqrd(),
         d3 = v3.LengthSqrd();

    cgmath::Vec<T, 2> bestA;
    if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1) {
        bestA = p2B;
    } else {
        bestA = p2A;
    }

    auto bestB = SegNearestPt(c1.seg, bestA);
    bestA = SegNearestPt(c2.seg, bestB);

    return IsCircularIntersect(Circular<T, N>::Create(bestA, c1.radius),
                               Circular<T, N>::Create(bestB, c2.radius));
}

template <typename T, uint32_t N>
bool IsAABBIntersect(const AABB<T, N>& a, const AABB<T, N>& b) {
    for (int i = 0; i < N; i++) {
        if (a.center[i] > b.center[i] + b.halfLen[i] + a.halfLen[i] ||
            a.center[i] < b.center[i] - b.halfLen[i] - a.halfLen[i]) {
            return false;
        }
    }
    return true;
}

// intersect

// distance

template <typename T, uint32_t N>
T PtDistSqrd(const cgmath::Vec<T, N>& v1, const cgmath::Vec<T, N>& v2) {
    return (v2 - v1).LengthSqrd();
}

// misc

template <typename T, uint32_t N>
bool IsLineParallel(const Line<T, N>& l1, const Line<T, N>& l2,
                    T cosTol = 0.0) {
    return std::abs(l1.dir.Dot(l2.dir)) - 1.0 <= cosTol;
}

template <typename T, uint32_t N>
bool IsLineOrtho(const Line<T, N>& l1, const Line<T, N>& l2, T cosTol = 0.0) {
    return std::abs(l1.dir.Dot(l2.dir)) <= cosTol;
}

template <typename T, uint32_t N>
bool IsSamePt(const cgmath::Vec<T, N>& p1, const cgmath::Vec<T, N>& p2,
              T distTolSqrd = 0.0001) {
    return (p1 - p2).LengthSqrd() <= distTolSqrd;
}

}  // namespace geom

}  // namespace nickel
