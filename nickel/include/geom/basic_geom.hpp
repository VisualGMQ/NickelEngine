#pragma once

#include "core/cgmath.hpp"

namespace nickel {

namespace geom {

template <typename T, size_t N>
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

template <typename T, size_t N>
bool IsAABBIntersect(const AABB<T, N>& a, const AABB<T, N>& b) {
    for (int i = 0; i < N; i++) {
        if (a.center[i] > b.center[i] + b.halfLen[i] + a.halfLen[i] ||
            a.center[i] < b.center[i] - b.halfLen[i] - a.halfLen[i]) {
            return false;
        }
    }
    return true;
}

template <typename T, size_t N>
cgmath::Vec<T, N> AABBNearestPoint(const AABB<T, N>& a,
                                   const cgmath::Vec<T, N>& pt) {
    cgmath::Vec<T, N> result;
    for (int i = 0; i < N; i++) {
        result[i] = std::clamp(pt[i], a.center[i] - a.halfLen[i],
                               a.center[i] + a.halfLen[i]);
    }
    return result;
}

template <typename T, size_t N>
struct Circular final {
    cgmath::Vec<T, N> center;
    T radius;

    static Circular Identity() {
        return {{}, 0.5};
    }
};

template <typename T, size_t N>
T PtDistanceSqrd(const cgmath::Vec<T, N>& v1, const cgmath::Vec<T, N>& v2) {
    return (v2 - v1).LengthSqrd();
}

template <typename T, size_t N>
bool IsCircularIntersect(const Circular<T, N>& c1, const Circular<T, N>& c2) {
    return PtDistanceSqrd(c1.center, c2.center) <
           (c1.radius + c2.radius) * (c1.radius + c2.radius);
}

template <typename T, size_t N>
struct Plane final {
    cgmath::Vec<T, N> pt;
    cgmath::Vec<T, N> normal;
};

template <typename T, size_t N>
struct OBB final {
    cgmath::Vec<T, N> axis;

    union {
        AABB<T, N> aabb;

        struct {
            cgmath::Vec<T, N> center;
            cgmath::Vec<T, N> halfLen;
        };
    };
};

template <typename T, size_t N>
struct Line {
    cgmath::Vec<T, N> p;
    cgmath::Vec<T, N> dir;  // must normaized

    static Line FromDir(const cgmath::Vec<T, N>& p,
                        const cgmath::Vec<T, N>& dir) {
        return {p, dir};
    }
};

template <typename T, size_t N>
cgmath::Vec<T, N> LineNearestPt(const Line<T, N>& line, const cgmath::Vec<T, N>& pt) {
    return (pt - line.p).Dot(line.dir) * line.dir;
}

template <typename T, size_t N>
struct Segment final : public Line<T, N> {
    T len;

    static Segment FromPts(const cgmath::Vec<T, N>& p1,
                           const cgmath::Vec<T, N>& p2) {
        Segment seg;
        seg.p = pt;
        seg.dir = p2 - p1;
        seg.len = seg.dir.Length();
        seg.dir.Normalize();
        return seg;
        return {pt, p2 - p1};
    }
};

template <typename T, size_t N>
struct Ray final : public Line<T, N> {};

}  // namespace geom

}  // namespace nickel