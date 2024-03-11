// Copyright 2023 VisualGMQ

#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <numeric>
#include <utility>
#include <vector>

#include "common/assert.hpp"

namespace nickel {

namespace cgmath {

// some forward declare for Vec

#ifndef CGMATH_NUMERIC_TYPE
#define CGMATH_NUMERIC_TYPE float
#endif

#ifndef CGMATH_LEN_TYPE
#define CGMATH_LEN_TYPE uint32_t
#endif

constexpr CGMATH_NUMERIC_TYPE PI =
    static_cast<CGMATH_NUMERIC_TYPE>(3.14159265358979);

/**
 * @brief check whether two values are the same
 */
template <typename T>
bool IsSameValue(T value1, T value2, T tol) {
    return std::abs(value1 - value2) <= std::numeric_limits<T>::epsilon() + tol;
}

template <typename T>
T Rad2Deg(T radians) {
    return radians * 180.0 / PI;
}

template <typename T>
T Deg2Rad(T degrees) {
    return static_cast<T>(degrees * PI / 180.0);
}

template <typename T>
using MustArithmetic = std::enable_if_t<std::is_arithmetic_v<T>>;

template <typename T, CGMATH_LEN_TYPE N>
class Vec;
template <typename T, CGMATH_LEN_TYPE Col, CGMATH_LEN_TYPE Row, typename>
class Mat;

using Vec2 = Vec<CGMATH_NUMERIC_TYPE, 2>;
using Vec3 = Vec<CGMATH_NUMERIC_TYPE, 3>;
using Vec4 = Vec<CGMATH_NUMERIC_TYPE, 4>;
using Color = Vec4;

template <typename T, CGMATH_LEN_TYPE N>
std::ostream& operator<<(std::ostream& o, const Vec<T, N>& v) {
    o << "Vec" << N << "(";
    for (int i = 0; i < N; i++) {
        o << v.data[i] << " ";
    }
    o << ")";
    return o;
}

template <typename T, CGMATH_LEN_TYPE N>
Vec<T, N> operator+(const Vec<T, N>&, const Vec<T, N>&);
template <typename T, CGMATH_LEN_TYPE N>
Vec<T, N> operator-(const Vec<T, N>&, const Vec<T, N>&);
template <typename T, typename U, CGMATH_LEN_TYPE N,
          typename = MustArithmetic<U>>
Vec<T, N> operator*(T, const Vec<T, N>&);
template <typename T, typename U, CGMATH_LEN_TYPE N,
          typename = MustArithmetic<U>>
Vec<T, N> operator*(const Vec<T, N>&, T);
template <typename T, typename U, CGMATH_LEN_TYPE N,
          typename = MustArithmetic<U>>
Vec<T, N> operator/(const Vec<T, N>&, T);
template <typename T, CGMATH_LEN_TYPE N>
Vec<T, N> operator*(const Vec<T, N>&, const Vec<T, N>&);
template <typename T, CGMATH_LEN_TYPE N>
Vec<T, N> operator/(const Vec<T, N>&, const Vec<T, N>&);
template <typename T, CGMATH_LEN_TYPE N>
bool operator!=(const Vec<T, N>& v1, const Vec<T, N>& v2);
template <typename T, CGMATH_LEN_TYPE N>
bool operator==(const Vec<T, N>& v1, const Vec<T, N>& v2);
template <typename T, CGMATH_LEN_TYPE N>
T LengthSqrd(const Vec<T, N>& v);
template <typename T, CGMATH_LEN_TYPE N>
T Length(const Vec<T, N>& v);
template <typename T, CGMATH_LEN_TYPE N>
T Project(const Vec<T, N>& src, const Vec<T, N>& des);
template <typename T>
T Cross(const Vec<T, 2>& v1, const Vec<T, 2>& v2);
template <typename T>
Vec<T, 3> Cross(const Vec<T, 3>& v1, const Vec<T, 3>& v2);
template <typename T, CGMATH_LEN_TYPE N>
Vec<T, N> Normalize(const Vec<T, N>& v);

// Vec function implementations

template <typename T, CGMATH_LEN_TYPE N>
Vec<T, N> operator+(const Vec<T, N>& v1, const Vec<T, N>& v2) {
    Vec<T, N> result;
    for (CGMATH_LEN_TYPE i = 0; i < N; i++) {
        result.data[i] = v1.data[i] + v2.data[i];
    }
    return result;
}

template <typename T, CGMATH_LEN_TYPE N>
Vec<T, N> operator-(const Vec<T, N>& v1, const Vec<T, N>& v2) {
    Vec<T, N> result;
    for (CGMATH_LEN_TYPE i = 0; i < N; i++) {
        result.data[i] = v1.data[i] - v2.data[i];
    }
    return result;
}

template <typename T, typename U, CGMATH_LEN_TYPE N,
          typename = MustArithmetic<U>>
Vec<T, N> operator*(U value, const Vec<T, N>& v) {
    Vec<T, N> result;
    for (CGMATH_LEN_TYPE i = 0; i < N; i++) {
        result.data[i] = v.data[i] * value;
    }
    return result;
}

template <typename T, typename U, CGMATH_LEN_TYPE N,
          typename = MustArithmetic<U>>
Vec<T, N> operator*(const Vec<T, N>& v, U value) {
    return value * v;
}

template <typename T, typename U, CGMATH_LEN_TYPE N,
          typename = MustArithmetic<U>>
Vec<T, N> operator/(const Vec<T, N>& v, U value) {
    Vec<T, N> result;
    for (CGMATH_LEN_TYPE i = 0; i < N; i++) {
        result.data[i] = v.data[i] / value;
    }
    return result;
}

template <typename T, CGMATH_LEN_TYPE N>
Vec<T, N> operator*(const Vec<T, N>& v1, const Vec<T, N>& v2) {
    Vec<T, N> result;
    for (CGMATH_LEN_TYPE i = 0; i < N; i++) {
        result.data[i] = v1.data[i] * v2.data[i];
    }
    return result;
}

template <typename T, CGMATH_LEN_TYPE N>
Vec<T, N> operator/(const Vec<T, N>& v1, const Vec<T, N>& v2) {
    Vec<T, N> result;
    for (CGMATH_LEN_TYPE i = 0; i < N; i++) {
        result.data[i] = v1.data[i] / v2.data[i];
    }
    return result;
}

template <typename T, CGMATH_LEN_TYPE N>
T Dot(const Vec<T, N>& v1, const Vec<T, N>& v2) {
    T sum{};
    for (CGMATH_LEN_TYPE i = 0; i < N; i++) {
        sum += v1.data[i] * v2.data[i];
    }
    return sum;
}

template <typename T, CGMATH_LEN_TYPE N>
bool operator==(const Vec<T, N>& v1, const Vec<T, N>& v2) {
    for (CGMATH_LEN_TYPE i = 0; i < N; i++) {
        if (v1.data[i] != v2.data[i]) {
            return false;
        }
    }
    return true;
}

template <typename T, CGMATH_LEN_TYPE N>
bool operator!=(const Vec<T, N>& v1, const Vec<T, N>& v2) {
    return !(v1 == v2);
}

template <typename T>
T Cross(const Vec<T, 2>& v1, const Vec<T, 2>& v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

template <typename T>
Vec<T, 3> Cross(const Vec<T, 3>& v1, const Vec<T, 3>& v2) {
    Vec<T, 3> result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    return result;
}

/**
 * @brief triple cross product: v1 x v2 x v3
 */
template <typename T>
Vec<T, 3> TripleCross(const Vec<T, 3>& v1, const Vec<T, 3>& v2,
                      const Vec<T, 3>& v3) {
    return Cross(Cross(v1, v2), v3);
}

/**
 * @brief mixed product: (v1 x v2) * v3
 */
template <typename T>
Vec<T, 3> MixedProduct(const Vec<T, 3>& v1, const Vec<T, 3>& v2,
                       const Vec<T, 3>& v3) {
    return Dot(Cross(v1, v2), v3);
}

template <typename T, CGMATH_LEN_TYPE N>
T LengthSqrd(const Vec<T, N>& v) {
    return Dot(v, v);
}

template <typename T, CGMATH_LEN_TYPE N>
T Length(const Vec<T, N>& v) {
    return std::sqrt(LengthSqrd(v));
}

template <typename T, CGMATH_LEN_TYPE N>
T Project(const Vec<T, N>& src, const Vec<T, N>& des) {
    return Dot(src, des) / Length(des);
}

template <typename T, CGMATH_LEN_TYPE N>
Vec<T, N> Normalize(const Vec<T, N>& v) {
    return v / Length(v);
}

template <typename T>
cgmath::Vec<T, 2> PerpendicVec(const cgmath::Vec<T, 2>& v) {
    return {-v.y, v.x};
}

// basic vector class

namespace internal {

template <typename T, CGMATH_LEN_TYPE N>
struct CommonVecOperations {
    using underlying_type = Vec<T, N>;

    T Dot(const underlying_type& o) const {
        return cgmath::Dot(static_cast<const underlying_type&>(*this), o);
    }

    T operator[](CGMATH_LEN_TYPE idx) const {
        return static_cast<const underlying_type&>(*this).data[idx];
    }

    T& operator[](CGMATH_LEN_TYPE idx) {
        return static_cast<underlying_type&>(*this).data[idx];
    }

    underlying_type operator-() const {
        underlying_type result;
        for (CGMATH_LEN_TYPE i = 0; i < N; i++) {
            result.data[i] =
                -static_cast<const underlying_type&>(*this).data[i];
        }
        return result;
    }

    underlying_type& operator+=(const underlying_type& o) {
        auto& v = static_cast<underlying_type&>(*this);
        v = v + o;
        return v;
    }

    underlying_type& operator-=(const underlying_type& o) {
        auto& v = static_cast<underlying_type&>(*this);
        v = v - o;
        return v;
    }

    underlying_type& operator*=(const underlying_type& o) {
        auto& v = static_cast<underlying_type&>(*this);
        v = v * o;
        return v;
    }

    underlying_type& operator/=(const underlying_type& o) {
        auto& v = static_cast<underlying_type&>(*this);
        v = v / o;
        return v;
    }

    underlying_type& operator*=(T value) {
        auto& v = static_cast<underlying_type&>(*this);
        v = v * value;
        return v;
    }

    underlying_type& operator/=(T value) {
        auto& v = static_cast<underlying_type&>(*this);
        v = v / value;
        return v;
    }

    T LengthSqrd() const {
        return cgmath::LengthSqrd(static_cast<const underlying_type&>(*this));
    }

    T Length() const {
        return cgmath::Length(static_cast<const underlying_type&>(*this));
    }

    void Normalize() {
        static_cast<underlying_type&>(*this) =
            cgmath::Normalize(static_cast<const underlying_type&>(*this));
    }

    underlying_type& operator=(const underlying_type& o) {
        auto& v = static_cast<underlying_type&>(*this);
        for (CGMATH_LEN_TYPE i = 0; i < N; i++) {
            v.data[i] = o.data[i];
        }
        return v;
    }
};

}  // namespace internal

template <typename T, CGMATH_LEN_TYPE N>
class Vec final : public internal::CommonVecOperations<T, N> {
public:
    static_assert(N > 1, "vector dimension must > 1");

    T data[N];

    Vec() { memset(data, 0, sizeof(data)); }

    template <typename U, CGMATH_LEN_TYPE N2>
    explicit Vec(const Vec<U, N2>& other) {
        int i = 0;
        for (; i < std::min(N, N2); i++) {
            data[i] = static_cast<T>(other.data[i]);
        }

        while (i < N) {
            data[i++] = T{};
        }
    }

    Vec(const std::initializer_list<T>& initList) {
        auto it = initList.begin();
        CGMATH_LEN_TYPE idx = 0;
        while (it != initList.end() && idx < N) {
            data[idx] = *it;
            it++;
            idx++;
        }
        while (idx < N) {
            data[idx++] = 0;
        }
    }
};

template <typename T>
class Vec<T, 2> final : public internal::CommonVecOperations<T, 2> {
public:
    union {
        struct {
            T x, y;
        };

        struct {
            T w, h;
        };

        T data[2];
    };

    Vec() : x{}, y{} {}

    explicit Vec(T x) : x(x), y{} {}

    template <typename U, CGMATH_LEN_TYPE N2>
    explicit Vec(const Vec<U, N2>& other) {
        x = other.data[0];
        y = other.data[1];
    }

    Vec(T x, T y) : x(x), y(y) {}

    Vec(const Vec&) = default;
    Vec& operator=(const Vec&) = default;

    void Set(T x, T y) {
        this->x = x;
        this->y = y;
    }

    auto Cross(const Vec<T, 2>& o) const { return cgmath::Cross(*this, o); }
};

template <typename T>
class Vec<T, 3> final : public internal::CommonVecOperations<T, 3> {
public:
    union {
        struct {
            T x, y, z;
        };

        struct {
            T r, s, t;
        };

        T data[3];
    };

    Vec() : x{}, y{}, z{} {}

    template <typename U, CGMATH_LEN_TYPE N2>
    explicit Vec(const Vec<U, N2>& other) {
        Set(other);
    }

    explicit Vec(T x) : x(x), y{}, z{} {}

    Vec(T x, T y) : x(x), y(y), z{} {}

    Vec(T x, T y, T z) : x(x), y(y), z(z) {}

    Vec(const Vec&) = default;
    Vec& operator=(const Vec&) = default;

    void Set(T x, T y, T z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    template <typename U, CGMATH_LEN_TYPE N2>
    void Set(const Vec<U, N2>& other) {
        x = other.data[0];
        y = other.data[1];
        z = N2 >= 3 ? other.data[2] : T{};
    }

    auto Cross(const Vec<T, 3>& o) const { return cgmath::Cross(*this, o); }
};

template <typename T>
class Vec<T, 4> final : public internal::CommonVecOperations<T, 4> {
public:
    union {
        struct {
            T x, y, z, w;
        };

        struct {
            T r, g, b, a;
        };

        T data[4];
    };

    Vec() : x{}, y{}, z{}, w{} {}

    explicit Vec(T x) : x(x), y{}, z{}, w{} {}

    template <typename U, CGMATH_LEN_TYPE N2>
    explicit Vec(const Vec<U, N2>& other) {
        Set(other);
    }

    Vec(T x, T y) : x(x), y(y), z{}, w{} {}

    Vec(T x, T y, T z) : x(x), y(y), z(z), w{} {}

    Vec(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

    Vec(const Vec&) = default;
    Vec& operator=(const Vec&) = default;

    void Set(T x, T y, T z, T w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    template <typename U, CGMATH_LEN_TYPE N2>
    void Set(const Vec<U, N2>& other) {
        x = other.data[0];
        y = other.data[1];
        z = N2 >= 3 ? other.data[2] : T{};
        w = N2 >= 4 ? other.data[3] : T{};
    }
};

// basic Mat class

template <typename T, CGMATH_LEN_TYPE Col, CGMATH_LEN_TYPE Row,
          typename = std::enable_if_t<std::is_arithmetic_v<T>>>
class Mat {
public:
    T data[Col * Row] = {T{}};

    static Mat Zeros() {
        Mat mat;
        memset(mat.data, 0, sizeof(mat.data));
        return mat;
    }

    static Mat FromCol(const std::initializer_list<Vec<T, Row>>& initVecs) {
        Mat mat;
        auto it = initVecs.begin();
        CGMATH_LEN_TYPE x = 0;
        while (it != initVecs.end()) {
            for (CGMATH_LEN_TYPE y = 0; y < Row; y++) {
                mat.Set(x, y, it->data[y]);
            }
            it++;
            x++;
        }
        return mat;
    }

    static Mat FromRow(const std::initializer_list<Vec<T, Row>>& initVecs) {
        Mat mat;
        auto it = initVecs.begin();
        CGMATH_LEN_TYPE y = 0;
        while (it != initVecs.end()) {
            for (CGMATH_LEN_TYPE x = 0; x < Col; x++) {
                mat.Set(x, y, it->data[x]);
            }
            it++;
            y++;
        }
        return mat;
    }

    static Mat FromCol(const std::initializer_list<T>& initList) {
        auto it = initList.begin();
        CGMATH_LEN_TYPE idx = 0;
        Mat m;

        while (it != initList.end() && idx < Col * Row) {
            m.Set(idx / Row, idx % Row, *it);
            it++;
            idx++;
        }

        return m;
    }

    static Mat FromRow(const std::initializer_list<T>& initList) {
        auto it = initList.begin();
        CGMATH_LEN_TYPE idx = 0;
        Mat m;

        while (it != initList.end() && idx < Col * Row) {
            m.Set(idx % Col, idx / Col, *it);
            it++;
            idx++;
        }

        return m;
    }

    static Mat Identity() {
        static_assert(Row == Col);

        Mat mat = Zeros();
        for (int i = 0; i < Row; i++) {
            mat.Set(i, i, 1);
        }
        return mat;
    }

    static Mat Ones() {
        Mat mat;
        memset(mat.data, 1, sizeof(mat.data));
        return mat;
    }

    T& Get(int x, int y) {
        Assert(x >= 0 && y >= 0 && x < Col && y < Row,
               "access matrix out of bound");

#ifdef CGMATH_MATRIX_ROW_FIRST
        return data[x + y * Col];
#else
        return data[y + x * Row];
#endif
    }

    T Get(int x, int y) const {
        assert(x >= 0 && y >= 0 && x < Col && y < Row);

#ifdef CGMATH_MATRIX_ROW_FIRST
        return data[x + y * Col];
#else
        return data[y + x * Row];
#endif
    }

    void Set(int x, int y, T value) { Get(x, y) = value; }

    constexpr int W() const { return Col; }

    constexpr int H() const { return Row; }

    Mat() = default;
};

template <typename T, CGMATH_LEN_TYPE Common, CGMATH_LEN_TYPE Mat1Row,
          CGMATH_LEN_TYPE Mat2Col>
auto operator*(const Mat<T, Common, Mat1Row>& m1,
               const Mat<T, Mat2Col, Common>& m2) {
    auto result = Mat<T, Mat2Col, Mat1Row>::Zeros();
    for (CGMATH_LEN_TYPE i = 0; i < Mat1Row; i++) {
        for (CGMATH_LEN_TYPE j = 0; j < Mat2Col; j++) {
            T sum{};
            for (CGMATH_LEN_TYPE k = 0; k < Common; k++) {
                sum += m1.Get(k, i) * m2.Get(j, k);
            }
            result.Set(j, i, sum);
        }
    }
    return result;
}

template <typename T, typename U, CGMATH_LEN_TYPE Col, CGMATH_LEN_TYPE Row>
auto operator*(const Mat<T, Col, Row>& m, U value) {
    auto result = Mat<T, Col, Row>::Zeros();
    for (CGMATH_LEN_TYPE i = 0; i < Col * Row; i++) {
        result.data[i] = m.data[i] * value;
    }
    return result;
}

template <typename T, typename U, CGMATH_LEN_TYPE Col, CGMATH_LEN_TYPE Row>
auto operator*(U value, const Mat<T, Col, Row>& m) {
    return m * value;
}

template <typename T, typename U, CGMATH_LEN_TYPE Col, CGMATH_LEN_TYPE Row>
auto operator/(const Mat<T, Col, Row>& m, T value) {
    Mat<T, Col, Row> result;
    for (CGMATH_LEN_TYPE i = 0; i < Col * Row; i++) {
        result.data[i] = m.data[i] / value;
    }
    return result;
}

template <typename T, CGMATH_LEN_TYPE Col, CGMATH_LEN_TYPE Row>
auto operator+(const Mat<T, Col, Row>& m1, const Mat<T, Col, Row>& m2) {
    auto result = Mat<T, Col, Row>::Zeros();
    for (CGMATH_LEN_TYPE i = 0; i < Col * Row; i++) {
        result.data[i] = m1.data[i] + m2.data[i];
    }
    return result;
}

template <typename T, CGMATH_LEN_TYPE Col, CGMATH_LEN_TYPE Row>
auto operator-(const Mat<T, Col, Row>& m1, const Mat<T, Col, Row>& m2) {
    auto result = Mat<T, Col, Row>::Zeros();
    for (CGMATH_LEN_TYPE i = 0; i < Col * Row; i++) {
        result.data[i] = m1.data[i] - m2.data[i];
    }
    return result;
}

template <typename T, CGMATH_LEN_TYPE Col, CGMATH_LEN_TYPE Row>
auto operator*(const Mat<T, Col, Row>& m, const Vec<T, Col>& v) {
    Vec<T, Row> result;
    for (CGMATH_LEN_TYPE y = 0; y < Row; y++) {
        T sum{};
        for (CGMATH_LEN_TYPE x = 0; x < Col; x++) {
            sum += m.Get(x, y) * v.data[x];
        }
        result.data[y] = sum;
    }
    return result;
}

template <typename T, CGMATH_LEN_TYPE Col, CGMATH_LEN_TYPE Row>
auto MulEach(const Mat<T, Col, Row>& m1, const Mat<T, Col, Row>& m2) {
    auto result = Mat<T, Col, Row>::Zeros();
    for (CGMATH_LEN_TYPE i = 0; i < Col * Row; i++) {
        result.data[i] = m1.data[i] * m2.data[i];
    }
    return result;
}

template <typename T, CGMATH_LEN_TYPE Col, CGMATH_LEN_TYPE Row>
auto DivEach(const Mat<T, Col, Row>& m1, const Mat<T, Col, Row>& m2) {
    auto result = Mat<T, Col, Row>::Zeros();
    for (CGMATH_LEN_TYPE i = 0; i < Col * Row; i++) {
        result.data[i] = m1.data[i] / m2.data[i];
    }
    return result;
}

template <typename T, CGMATH_LEN_TYPE Col, CGMATH_LEN_TYPE Row>
auto Transpose(const Mat<T, Col, Row>& m) {
    auto result = Mat<T, Col, Row>::Zeros();
    for (CGMATH_LEN_TYPE x = 0; x < Col; x++) {
        for (CGMATH_LEN_TYPE y = 0; y < Row; y++) {
            result.Set(x, y, m.Get(y, x));
        }
    }
    return result;
}

using Mat22 = Mat<CGMATH_NUMERIC_TYPE, 2, 2>;
using Mat33 = Mat<CGMATH_NUMERIC_TYPE, 3, 3>;
using Mat44 = Mat<CGMATH_NUMERIC_TYPE, 4, 4>;

inline Mat44 CreatePersp(float fov, float aspect, float near, float far,
                         bool GLCoord = false) {
    float focal = 1.0 / std::tan(fov * 0.5);

    // clang-format off
    return Mat44::FromRow({
        focal / aspect, 0, 0, 0,
        0, (GLCoord ? 1 : -1) *focal, 0, 0,
        0, 0, 2.f * near / (far - near), 2.f * near * far / (far - near),
        0, 0, -1, 0,
    });
    // clang-format on
}

inline Mat44 CreateOrtho(float left, float right, float top, float bottom,
                         float near, float far) {
    // clang-format off
    return Mat44::FromRow({
        2.0f / (right - left),                  0.0f,                0.0f, (left + right) / (left - right),
                         0.0f, 2.0f / (top - bottom),                0.0f, (bottom + top) / (bottom - top),
                         0.0f,                  0.0f, 2.0f / (far - near),     (near + far) / (near - far),
                         0.0f,                  0.0f,                0.0f,                            1.0f,
    });
    // clang-format on
}

inline Mat44 CreateTranslation(const Vec3& position) {
    // clang-format off
    return Mat44::FromRow({
        1.0f, 0.0f, 0.0f, position.x,
        0.0f, 1.0f, 0.0f, position.y,
        0.0f, 0.0f, 1.0f, position.z,
        0.0f, 0.0f, 0.0f, 1.0f,
    });
    // clang-format on
}

inline Mat44 CreateZRotation(float radians) {
    float cos = std::cos(radians);
    float sin = std::sin(radians);
    // clang-format off
    return Mat44::FromRow({
         cos, -sin, 0.0f, 0.0f,
         sin,  cos, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    });
    // clang-format on
}

inline Mat44 LookAt(const cgmath::Vec3& target, const cgmath::Vec3& srcPos,
                    const cgmath::Vec3& up) {
    Assert(up.LengthSqrd() == 1, "lookat param up must be normalized");

    auto zAxis = Normalize(srcPos - target);
    auto xAxis = nickel::cgmath::Normalize(up.Cross(zAxis));
    auto yAxis = zAxis.Cross(xAxis);

    // clang-format off
    return Mat44::FromRow({
        xAxis.x, xAxis.y, xAxis.z, -xAxis.Dot(srcPos),
        yAxis.x, yAxis.y, yAxis.z, -yAxis.Dot(srcPos),
        zAxis.x, zAxis.y, zAxis.z, -zAxis.Dot(srcPos),
             0,      0,       0,                    1,
    });
    // clang-format on
}

inline Mat44 CreateXRotation(float radians) {
    float cos = std::cos(radians);
    float sin = std::sin(radians);
    // clang-format off
    return Mat44::FromRow({
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f,  cos, -sin, 0.0f,
        0.0f,  sin,  cos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    });
    // clang-format on
}

inline Mat44 CreateYRotation(float radians) {
    float cos = std::cos(radians);
    float sin = std::sin(radians);
    // clang-format off
    return Mat44::FromRow({
         cos, 0.0f,  sin, 0.0f,
        0.0f, 1.0f,  0.0, 0.0f,
        -sin, 0.0f,  cos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    });
    // clang-format on
}

inline Mat44 CreateXYZRotation(const cgmath::Vec3& r) {
    return CreateXRotation(r.x) * CreateYRotation(r.y) * CreateZRotation(r.z);
}

inline Mat22 CreateRotation2D(float radians) {
    auto cos = std::cos(radians);
    auto sin = std::sin(radians);
    return Mat22::FromRow({
        cos,
        -sin,
        sin,
        cos,
    });
}

inline Mat44 CreateScale(const Vec3& scale) {
    // clang-format off
    return Mat44::FromRow({
        scale.x,     0.0,    0.0f, 0.0f,
           0.0f, scale.y,    0.0f, 0.0f,
           0.0f,    0.0f, scale.z, 0.0f,
           0.0f,    0.0f,    0.0f, 1.0f,
    });
    // clang-format on
}

struct Rect {
    Vec2 position, size;

    Rect() = default;

    Rect(const Vec2& position, const Vec2& size)
        : Rect(position.x, position.y, size.w, size.h) {}

    Rect(float x, float y, float w, float h) : position{x, y}, size{w, h} {}

    Rect(const Rect&) = default;
    Rect& operator=(const Rect&) = default;

    static Rect FromCenter(const Vec2& center, const Vec2& halfSize) {
        return Rect{center - halfSize, halfSize * 2.0};
    }

    bool IsPtIn(const cgmath::Vec2& v) const {
        return v.x > position.x && v.x < position.x + size.w &&
               v.y > position.y && v.y < position.y + size.h;
    }

    /**
     * @brief return two rects intersect
     * @note if rects are not intersect, will return a invalid rect
     */
    Rect Intersect(const cgmath::Rect& o) const {
        auto minX = std::max(position.x, o.position.x);
        auto minY = std::max(position.y, o.position.y);
        auto maxX = std::min(position.x + size.w, o.position.x + o.size.w);
        auto maxY = std::min(position.y + size.h, o.position.y + o.size.h);
        return Rect(minX, minY, maxX - minX, maxY - minY);
    }

    bool IsIntersect(const cgmath::Rect& o) const {
        return !(position.x > o.position.x + o.size.w ||
                 position.x + size.w < o.position.x ||
                 position.y > o.position.y + o.size.h ||
                 position.y + size.h < o.position.y);
    }
};

template <typename T>
class DynMat final {
public:
    DynMat() = default;

    DynMat(uint32_t col, uint32_t row)
        : datas_{col * row, T{}}, size_{col, row} {}

    auto& Size() const { return size_; }

    auto Col() const { return size_.w; }

    auto Row() const { return size_.h; }

    const T& Get(uint32_t col, uint32_t row) const {
        Assert(row * col < datas_.size(), "out of range");
        return datas_[col * size_.h + row];
    }

    T& Get(uint32_t col, uint32_t row) {
        return const_cast<T&>(std::as_const(*this).Get(col, row));
    }

    void Set(uint32_t col, uint32_t row, const T& value) {
        datas_[col * size_.h + row] = value;
    }

    void Set(uint32_t col, uint32_t row, T&& value) {
        datas_[col * size_.h + row] = std::move(value);
    }

    void Resize(uint32_t col, uint32_t row) {
        std::vector<T> newData{col * row, T{}};
        auto minRow = std::min(row, size_.h);
        auto minCol = std::min(col, size_.w);
        for (int c = 0; c < minCol; c++) {
            for (int r = 0; r < minRow; r++) {
                newData[c * row + r] = Get(c, r);
            }
        }
        datas_ = std::move(newData);
    }

private:
    std::vector<T> datas_;
    Vec<uint32_t, 2> size_;
};

struct SRT final {
    Vec3 position;
    Vec3 scale;
    Vec3 rotation;

    static auto Identity() { return SRT{{}, {}, {}}; }

    Mat44 Mat() const {
        return CreateScale(scale) * CreateXYZRotation(rotation) *
               CreateTranslation(position);
    }
};

template <typename T>
int Sign(T value) {
    return value < 0 ? -1 : (value > 0 ? 1 : 0);
}

/**
 * @brief check whether two section is overlapped
 */
template <typename T>
bool IsOverlap(T min1, T max1, T min2, T max2) {
    return !((min1 >= max2) || (min2 >= max1));
}

template <typename T>
T Wrap(T value, T min, T max) {
    while (value < min || value > max) {
        if (value < min) {
            value = max - (min - value);
        } else if (value > max) {
            value = min + (value - max);
        }
    }
    return value;
}

/**
 * @brief Get degree between v1 & v2 in [0, 2 * PI)
 * @note v1 & v2 are both normalized
 */
template <typename T>
T GetRadianIn360(const Vec<T, 2>& v1, const Vec<T, 2>& v2) {
    auto cos = Dot(v1, v2);
    auto sin = Cross(v1, v2);

    return Wrap<double>(std::acos(cos) * Sign(sin), 0, 2 * PI);
}

/**
 * @brief Get degree between v1 & v2 in [-PI, PI)
 * @note v1 & v2 are both normalized
 */
template <typename T>
T GetRadianIn180Signed(const Vec<T, 2>& v1, const Vec<T, 2>& v2) {
    auto cos = Dot(v1, v2);
    auto sin = Cross(v1, v2);
    return std::acos(cos) * Sign(sin);
}

template <typename T>
struct Quaternion final {
    Vec<T, 3> v;
    T w;

    Quaternion(const Vec<T, 3>& v, T w) : v{v}, w{w} {}

    Quaternion(T x, T y, T z, T w) : v{x, y, z}, w{w} {}

    // only for unit quaternion
    Quaternion Conjugate() const {
        Assert(std::abs(LengthSqrd() - 1) <= 0.00001,
               "conjugate only for unit quaternion");
        return {-v, w};
    }

    // only for unit quaternion
    Quaternion Inverse() const {
        Assert(std::abs(LengthSqrd() - 1) <= 0.00001,
               "inverse only for unit quaternion");
        return Conjugate();
    }

    auto LengthSqrd() const { return v.LengthSqrd() + w * w; }

    auto Length() const { return std::sqrt(v.LengthSqrd() + w * w); }

    // clang-format off
    Mat44 ToMat() const {
        auto x2 = v.x * v.x;
        auto y2 = v.y * v.y;
        auto z2 = v.z * v.z;
        auto xy = v.x * v.y;
        auto yz = v.y * v.z;
        auto xz = v.x * v.z;
        auto xw = v.x * w;
        auto yw = v.y * w;
        auto zw = v.z * w;
        return Mat44::FromCol({
            1 - 2 * (y2 + z2),     2 * (xy + zw),     2 * (xz - yw), 0,
               2 * (xy  - zw), 1 - 2 * (x2 + z2),     2 * (yz + xw), 0,
                2 * (xz + yw),     2 * (yz - xw), 1 - 2 * (x2 + y2), 0,
                            0,                 0,                 0, 1
        });
    }

    // clang-format on
};

template <typename T>
Quaternion<T> operator*(const Quaternion<T>& q1, const Quaternion<T>& q2) {
    return {q1.w * q2.v + q2.w * q1.v + q1.v.Cross(q2.v),
            q1.w * q2.w - q1.v.Dot(q2.v)};
}

using Quat = Quaternion<CGMATH_NUMERIC_TYPE>;

template <typename T>
Quaternion<T> CreateQuatByRotate(const Vec<T, 3>& axis, T radians) {
    auto half = radians * 0.5;
    return {axis * std::sin(half), std::cos(half)};
}

template <typename T>
cgmath::Vec<T, 3> RotateByAxis(const cgmath::Vec<T, 3>& v,
                               const Quaternion<T>& q) {
    return q *
           Quaternion<T>{
               {v.x, v.y, v.z},
               0
    } *
           q.Inverse().v;
}

template <typename T>
std::pair<Vec<T, 3>, Vec<T, 3>> GetNormalMapTB(Vec<T, 3> p1, Vec<T, 3> p2,
                                               Vec<T, 3> p3, Vec<T, 2> uv1,
                                               Vec<T, 2> uv2, Vec<T, 2> uv3) {
    Assert(!(uv1 == Vec<T, 2>{} && uv2 == Vec<T, 2>{} && uv3 == Vec<T, 2>{}),
           "uvs are (0, 0) will cause numerical error");
    Vec<T, 3> e1{p2 - p1}, e2{p3 - p1};
    Vec<T, 2> dUV1{uv2 - uv1}, dUV2{uv3 - uv1};
    float denoInv = 1.0 / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

    // clang-format off
    auto t = nickel::cgmath::Vec<T, 3>{
                   dUV2.y * e1.x - dUV1.y * e2.x,
                   dUV2.y * e1.y - dUV1.y * e2.y,
                   dUV2.y * e1.z - dUV1.y * e2.z} * denoInv;
    auto b = nickel::cgmath::Vec<T, 3>{
                   -dUV2.x * e1.x + dUV1.x * e2.x,
                   -dUV2.x * e1.y + dUV1.x * e2.y,
                   -dUV2.x * e1.z + dUV1.x * e2.z} * denoInv;
    // clang-format on

    return {nickel::cgmath::Normalize(t), nickel::cgmath::Normalize(b)};
}

template <typename T>
Vec<T, 3> GetNormalMapTangent(Vec<T, 3> p1, Vec<T, 3> p2,
                                               Vec<T, 3> p3, Vec<T, 2> uv1,
                                               Vec<T, 2> uv2, Vec<T, 2> uv3) {
    Assert(!(uv1 == Vec<T, 2>{} && uv2 == Vec<T, 2>{} && uv3 == Vec<T, 2>{}),
           "uvs are (0, 0) will cause numerical error");
    Vec<T, 3> e1{p2 - p1}, e2{p3 - p1};
    Vec<T, 2> dUV1{uv2 - uv1}, dUV2{uv3 - uv1};
    float denoInv = 1.0 / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

    return nickel::cgmath::Vec<T, 3>{dUV2.y * e1.x - dUV1.y * e2.x,
                                     dUV2.y * e1.y - dUV1.y * e2.y,
                                     dUV2.y * e1.z - dUV1.y * e2.z} *
           denoInv;
}

}  // namespace cgmath

}  // namespace nickel