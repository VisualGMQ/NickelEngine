// Copyright 2023 VisualGMQ

#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <utility>
#include <vector>

#include "core/assert.hpp"

namespace nickel {

namespace cgmath {

// some forward declare for Vec

#ifndef CGMATH_NUMERIC_TYPE
#define CGMATH_NUMERIC_TYPE float
#endif

constexpr CGMATH_NUMERIC_TYPE PI =
    static_cast<CGMATH_NUMERIC_TYPE>(3.14159265358979);

template <typename T>
inline T Rad2Deg(T radians) {
    return radians * 180.0 / PI;
}

template <typename T>
inline T Deg2Rad(T degrees) {
    return static_cast<T>(degrees * PI / 180.0);
}

template <typename T>
using MustArithmetic = std::enable_if_t<std::is_arithmetic_v<T>>;

template <typename T, unsigned int N>
class Vec;
template <typename T, unsigned int Col, unsigned int Row, typename>
class Mat;

using Vec2 = Vec<CGMATH_NUMERIC_TYPE, 2>;
using Vec3 = Vec<CGMATH_NUMERIC_TYPE, 3>;
using Vec4 = Vec<CGMATH_NUMERIC_TYPE, 4>;
using Color = Vec4;

template <typename T, unsigned int N>
std::ostream& operator<<(std::ostream& o, const Vec<T, N>& v) {
    o << "Vec" << N << "(";
    for (int i = 0; i < N; i++) {
        o << v.data[i] << " ";
    }
    o << ")";
    return o;
}

template <typename T, unsigned int N>
Vec<T, N> operator+(const Vec<T, N>&, const Vec<T, N>&);
template <typename T, unsigned int N>
Vec<T, N> operator-(const Vec<T, N>&, const Vec<T, N>&);
template <typename T, typename U, unsigned int N, typename = MustArithmetic<U>>
Vec<T, N> operator*(T, const Vec<T, N>&);
template <typename T, typename U, unsigned int N, typename = MustArithmetic<U>>
Vec<T, N> operator*(const Vec<T, N>&, T);
template <typename T, typename U, unsigned int N, typename = MustArithmetic<U>>
Vec<T, N> operator/(const Vec<T, N>&, T);
template <typename T, unsigned int N>
Vec<T, N> operator*(const Vec<T, N>&, const Vec<T, N>&);
template <typename T, unsigned int N>
Vec<T, N> operator/(const Vec<T, N>&, const Vec<T, N>&);
template <typename T, unsigned int N>
bool operator!=(const Vec<T, N>& v1, const Vec<T, N>& v2);
template <typename T, unsigned int N>
bool operator==(const Vec<T, N>& v1, const Vec<T, N>& v2);
template <typename T, unsigned int N>
T LengthSquare(const Vec<T, N>& v);
template <typename T, unsigned int N>
T Length(const Vec<T, N>& v);
template <typename T, unsigned int N>
T Project(const Vec<T, N>& src, const Vec<T, N>& des);
template <typename T>
T Cross(const Vec<T, 2>& v1, const Vec<T, 2>& v2);
template <typename T>
Vec<T, 3> Cross(const Vec<T, 3>& v1, const Vec<T, 3>& v2);
template <typename T, unsigned int N>
Vec<T, N> Normalize(const Vec<T, N>& v);

// Vec function implementations

template <typename T, unsigned int N, unsigned int NewN>
auto VecCvt(const Vec<T, N>& v) {
    Vec<T, NewN> result;
    unsigned int idx = 0;
    unsigned int min = std::min(N, NewN);

    while (idx < min) {
        result.data[idx] = v.data[idx];
        idx++;
    }

    while (idx < NewN) {
        result.data[idx] = T{};
    }
    return result;
}

template <typename T, unsigned int N>
Vec<T, N> operator+(const Vec<T, N>& v1, const Vec<T, N>& v2) {
    Vec<T, N> result;
    for (unsigned int i = 0; i < N; i++) {
        result.data[i] = v1.data[i] + v2.data[i];
    }
    return result;
}

template <typename T, unsigned int N>
Vec<T, N> operator-(const Vec<T, N>& v1, const Vec<T, N>& v2) {
    Vec<T, N> result;
    for (unsigned int i = 0; i < N; i++) {
        result.data[i] = v1.data[i] - v2.data[i];
    }
    return result;
}

template <typename T, typename U, unsigned int N, typename = MustArithmetic<U>>
Vec<T, N> operator*(U value, const Vec<T, N>& v) {
    Vec<T, N> result;
    for (unsigned int i = 0; i < N; i++) {
        result.data[i] = v.data[i] * value;
    }
    return result;
}

template <typename T, typename U, unsigned int N, typename = MustArithmetic<U>>
Vec<T, N> operator*(const Vec<T, N>& v, U value) {
    return value * v;
}

template <typename T, typename U, unsigned int N, typename = MustArithmetic<U>>
Vec<T, N> operator/(const Vec<T, N>& v, U value) {
    Vec<T, N> result;
    for (unsigned int i = 0; i < N; i++) {
        result.data[i] = v.data[i] / value;
    }
    return result;
}

template <typename T, unsigned int N>
Vec<T, N> operator*(const Vec<T, N>& v1, const Vec<T, N>& v2) {
    Vec<T, N> result;
    for (unsigned int i = 0; i < N; i++) {
        result.data[i] = v1.data[i] * v2.data[i];
    }
    return result;
}

template <typename T, unsigned int N>
Vec<T, N> operator/(const Vec<T, N>& v1, const Vec<T, N>& v2) {
    Vec<T, N> result;
    for (unsigned int i = 0; i < N; i++) {
        result.data[i] = v1.data[i] / v2.data[i];
    }
    return result;
}

template <typename T, unsigned int N>
T Dot(const Vec<T, N>& v1, const Vec<T, N>& v2) {
    T sum{};
    for (unsigned int i = 0; i < N; i++) {
        sum += v1.data[i] * v2.data[i];
    }
    return sum;
}

template <typename T, unsigned int N>
bool operator==(const Vec<T, N>& v1, const Vec<T, N>& v2) {
    for (unsigned int i = 0; i < N; i++) {
        if (v1.data[i] != v2.data[i]) {
            return false;
        }
    }
    return true;
}

template <typename T, unsigned int N>
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

template <typename T, unsigned int N>
T LengthSquare(const Vec<T, N>& v) {
    return Dot(v, v);
}

template <typename T, unsigned int N>
T Length(const Vec<T, N>& v) {
    return std::sqrt(LengthSquare(v));
}

template <typename T, unsigned int N>
T Project(const Vec<T, N>& src, const Vec<T, N>& des) {
    return Dot(src, des) / Length(des);
}

template <typename T, unsigned int N>
Vec<T, N> Normalize(const Vec<T, N>& v) {
    return v / Length(v);
}

// basic vector class

template <typename T, unsigned int N>
class Vec {
public:
    T data[N];

    Vec() { memset(data, 0, sizeof(data)); }

    Vec(const std::initializer_list<T>& initList) {
        auto it = initList.begin();
        unsigned int idx = 0;
        while (it != initList.end() && idx < N) {
            data[idx] = *it;
            it++;
            idx++;
        }
        while (idx < N) {
            data[idx++] = 0;
        }
    }

    auto Dot(const Vec<T, N>& o) const { return Dot(*this, o); }

    auto operator+=(const Vec<T, N>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator-=(const Vec<T, N>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator*=(const Vec<T, N>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator/=(const Vec<T, N>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator*=(T value) {
        *this = *this * value;
        return *this;
    }

    auto operator/=(T value) {
        *this = *this / value;
        return *this;
    }

    auto LengthSquare() const { return LengthSquare(*this); }

    auto Length() const { return Length(*this); }

    void Normalize() const { *this = Normalize(*this); }

    Vec& operator=(const Vec& o) {
        for (unsigned int i = 0; i < N; i++) {
            this->data[i] = o.data[i];
        }
        return *this;
    }
};

template <typename T>
class Vec<T, 2> final {
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

    Vec(T x, T y) : x(x), y(y) {}

    Vec(const Vec&) = default;
    Vec& operator=(const Vec&) = default;

    auto Dot(const Vec<T, 2>& o) const { return cgmath::Dot(*this, o); }

    void Set(T x, T y) {
        this->x = x;
        this->y = y;
    }

    auto operator+=(const Vec<T, 2>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator-=(const Vec<T, 2>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator*=(const Vec<T, 2>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator/=(const Vec<T, 2>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator*=(T value) {
        *this = *this * value;
        return *this;
    }

    auto operator/=(T value) {
        *this = *this / value;
        return *this;
    }

    auto LengthSquare() const { return cgmath::LengthSquare(*this); }

    auto Length() const { return cgmath::Length(*this); }

    void Normalize() { *this = cgmath::Normalize(*this); }

    auto Cross(const Vec<T, 2>& o) const { return cgmath::Cross(*this, o); }
};

template <typename T>
class Vec<T, 3> final {
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

    explicit Vec(T x) : x(x), y{}, z{} {}

    Vec(T x, T y) : x(x), y(y), z{} {}

    Vec(T x, T y, T z) : x(x), y(y), z(z) {}

    Vec(const Vec&) = default;
    Vec& operator=(const Vec&) = default;

    auto Dot(const Vec<T, 3>& o) const { return cgmath::Dot(*this, o); }

    void Set(T x, T y, T z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    auto operator+=(const Vec<T, 3>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator-=(const Vec<T, 3>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator*=(const Vec<T, 3>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator/=(const Vec<T, 3>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator*=(T value) {
        *this = *this * value;
        return *this;
    }

    auto operator/=(T value) {
        *this = *this / value;
        return *this;
    }

    auto LengthSquare() const { return cgmath::LengthSquare(*this); }

    auto Length() const { return cgmath::Length(*this); }

    void Normalize() { *this = cgmath::Normalize(*this); }

    auto Cross(const Vec<T, 3>& o) { return cgmath::Cross(*this, o); }
};

template <typename T>
class Vec<T, 4> final {
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

    Vec(T x, T y) : x(x), y(y), z{}, w{} {}

    Vec(T x, T y, T z) : x(x), y(y), z(z), w{} {}

    Vec(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

    Vec(const Vec&) = default;
    Vec& operator=(const Vec&) = default;

    auto Dot(const Vec<T, 4>& o) const { return cgmath::Dot(*this, o); }

    void Set(T x, T y, T z, T w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    auto operator+=(const Vec<T, 4>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator-=(const Vec<T, 4>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator*=(const Vec<T, 4>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator/=(const Vec<T, 4>& o) {
        *this = *this + o;
        return *this;
    }

    auto operator*=(T value) {
        *this = *this * value;
        return *this;
    }

    auto operator/=(T value) {
        *this = *this / value;
        return *this;
    }

    auto LengthSquare() const { return LengthSquare(*this); }

    auto Length() const { return Length(*this); }

    void Normalize() const { *this = Normalize(*this); }
};

// basic Mat class

template <typename T, unsigned int Col, unsigned int Row,
          typename = std::enable_if_t<std::is_arithmetic_v<T>>>
class Mat {
public:
    T data[Col * Row];

    static Mat Zeros() {
        Mat mat;
        memset(mat.data, 0, sizeof(mat.data));
        return mat;
    }

    static Mat FromCol(const std::initializer_list<Vec<T, Row>>& initVecs) {
        Mat mat;
        auto it = initVecs.begin();
        unsigned int x = 0;
        while (it != initVecs.end()) {
            for (unsigned int y = 0; y < Row; y++) {
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
        unsigned int y = 0;
        while (it != initVecs.end()) {
            for (unsigned int x = 0; x < Col; x++) {
                Set(x, y, it->data[x]);
            }
            it++;
            y++;
        }
        return mat;
    }

    static Mat FromCol(const std::initializer_list<T>& initList) {
        auto it = initList.begin();
        unsigned int idx = 0;
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
        unsigned int idx = 0;
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

private:
    Mat() = default;
};

template <typename T, unsigned int Common, unsigned int Mat1Row,
          unsigned int Mat2Col>
auto operator*(const Mat<T, Common, Mat1Row>& m1,
               const Mat<T, Mat2Col, Common>& m2) {
    auto result = Mat<T, Mat2Col, Mat1Row>::Zeros();
    for (unsigned int i = 0; i < Mat1Row; i++) {
        for (unsigned int j = 0; j < Mat2Col; j++) {
            T sum{};
            for (unsigned int k = 0; k < Common; k++) {
                sum += m1.Get(k, i) * m2.Get(j, k);
            }
            result.Set(j, i, sum);
        }
    }
    return result;
}

template <typename T, typename U, unsigned int Col, unsigned int Row>
auto operator*(const Mat<T, Col, Row>& m, U value) {
    auto result = Mat<T, Col, Row>::Zeros();
    for (unsigned int i = 0; i < Col * Row; i++) {
        result.data[i] = m.data[i] * value;
    }
    return result;
}

template <typename T, typename U, unsigned int Col, unsigned int Row>
auto operator*(U value, const Mat<T, Col, Row>& m) {
    return m * value;
}

template <typename T, typename U, unsigned int Col, unsigned int Row>
auto operator/(const Mat<T, Col, Row>& m, T value) {
    Mat<T, Col, Row> result;
    for (unsigned int i = 0; i < Col * Row; i++) {
        result.data[i] = m.data[i] / value;
    }
    return result;
}

template <typename T, unsigned int Col, unsigned int Row>
auto operator+(const Mat<T, Col, Row>& m1, const Mat<T, Col, Row>& m2) {
    auto result = Mat<T, Col, Row>::Zeros();
    for (unsigned int i = 0; i < Col * Row; i++) {
        result.data[i] = m1.data[i] + m2.data[i];
    }
    return result;
}

template <typename T, unsigned int Col, unsigned int Row>
auto operator-(const Mat<T, Col, Row>& m1, const Mat<T, Col, Row>& m2) {
    auto result = Mat<T, Col, Row>::Zeros();
    for (unsigned int i = 0; i < Col * Row; i++) {
        result.data[i] = m1.data[i] - m2.data[i];
    }
    return result;
}

template <typename T, unsigned int Col, unsigned int Row>
auto operator*(const Mat<T, Col, Row>& m, const Vec<T, Col>& v) {
    Vec<T, Row> result;
    for (unsigned int y = 0; y < Row; y++) {
        T sum{};
        for (unsigned int x = 0; x < Col; x++) {
            sum += m.Get(x, y) * v.data[x];
        }
        result.data[y] = sum;
    }
    return result;
}

template <typename T, unsigned int Col, unsigned int Row>
auto MulEach(const Mat<T, Col, Row>& m1, const Mat<T, Col, Row>& m2) {
    auto result = Mat<T, Col, Row>::Zeros();
    for (unsigned int i = 0; i < Col * Row; i++) {
        result.data[i] = m1.data[i] * m2.data[i];
    }
    return result;
}

template <typename T, unsigned int Col, unsigned int Row>
auto DivEach(const Mat<T, Col, Row>& m1, const Mat<T, Col, Row>& m2) {
    auto result = Mat<T, Col, Row>::Zeros();
    for (unsigned int i = 0; i < Col * Row; i++) {
        result.data[i] = m1.data[i] / m2.data[i];
    }
    return result;
}

template <typename T, unsigned int Col, unsigned int Row>
auto Transpose(const Mat<T, Col, Row>& m) {
    auto result = Mat<T, Col, Row>::Zeros();
    for (unsigned int x = 0; x < Col; x++) {
        for (unsigned int y = 0; y < Row; y++) {
            result.Set(x, y, m.Get(y, x));
        }
    }
    return result;
}

using Mat22 = Mat<CGMATH_NUMERIC_TYPE, 2, 2>;
using Mat33 = Mat<CGMATH_NUMERIC_TYPE, 3, 3>;
using Mat44 = Mat<CGMATH_NUMERIC_TYPE, 4, 4>;

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
        -sin, -sin,  cos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    });
    // clang-format on
}

inline Mat44 CreateXYZRotation(const cgmath::Vec3& r) {
    return CreateXRotation(r.x) * CreateYRotation(r.y) * CreateZRotation(r.z);
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
    union {
        struct {
            Vec2 position, size;
        };

        struct {
            float x, y, w, h;
        };
    };

    Rect(): position{}, size{} {}

    Rect(const Vec2& position, const Vec2& size)
        : position(position), size(size) {}

    Rect(float x, float y, float w, float h) : position{x, y}, size{w, h} {}

    Rect(const Rect&) = default;
    Rect& operator=(const Rect&) = default;
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

}  // namespace cgmath

}  // namespace nickel