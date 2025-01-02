#pragma once

#include "nickel/common/assert.hpp"
#include "nickel/common/math/constants.hpp"
#include "nickel/common/math/view.hpp"

namespace nickel {

template <typename T, bool IsConst>
void MatrixAdd(const MatrixView<T, IsConst> m1,
               const MatrixView<T, IsConst> m2,
               MatrixView<T, false> out_m) noexcept {
    for (int c = 0; c < m1.ColNum(); c++) {
        for (int r = 0; r < m2.RowNum(); r++) {
            out_m[c][r] = m1[c][r] + m2[c][r];
        }
    }
}

template <typename T, bool IsConst>
void MatrixMinus(const MatrixView<T, IsConst> m1,
                 const MatrixView<T, IsConst> m2,
                 MatrixView<T, false> out_m) noexcept {
    for (int c = 0; c < m1.ColNum(); c++) {
        for (int r = 0; r < m2.RowNum(); r++) {
            out_m[c][r] = m1[c][r] - m2[c][r];
        }
    }
}

template <typename T, bool IsConst>
void MatrixMul(const MatrixView<T, IsConst> m, float value,
               MatrixView<T, IsConst> m_out) noexcept {
    for (int c = 0; c < m.ColNum(); c++) {
        for (int r = 0; r < m.RowNum(); r++) {
            m_out[c][r] = m[c][r] * value;
        }
    }
}

template <typename T, bool IsConst>
void MatrixNeg(const MatrixView<T, IsConst> m,
               MatrixView<T, false> m_out) noexcept {
    for (int c = 0; c < m.ColNum(); c++) {
        for (int r = 0; r < m.RowNum(); r++) {
            m_out[c][r] = -m[c][r];
        }
    }
}

template <typename T, bool IsConst>
void MatrixDiv(MatrixView<T, IsConst> m, float value,
               MatrixView<T, false>& out_m) noexcept {
    for (int c = 0; c < m.ColNum(); c++) {
        for (int r = 0; r < m.RowNum(); r++) {
            out_m[c][r] = m[c][r] / value;
        }
    }
}

template <typename T, bool IsConst>
std::ostream& operator<<(std::ostream& o, MatrixView<T, IsConst> view) {
    o << "[";
    for (int r = 0; r < view.RowNum(); r++) {
        for (int c = 0; c < view.ColNum(); c++) {
            o << view[c][r] << " ";
        }
        o << std::endl;
    }
    o << "]";
    return o;
}

//
// @brief A_m_n x B_n_r
//
template <typename T, bool IsConst>
void MatrixMul(const MatrixView<T, IsConst> m1, const MatrixView<T, IsConst> m2,
               MatrixView<T, false> out_m) {
    auto M = m1.RowNum();
    auto N = m1.ColNum();
    auto R = m2.ColNum();
    for (int m = 0; m < M; m++) {
        for (int r = 0; r < R; r++) {
            float sum = 0;
            for (int n = 0; n < N; n++) {
                sum += m1[n][m] * m2[r][n];
            }
            out_m[r][m] = sum;
        }
    }
}

template <typename T, bool IsConst>
void MatrixMulEach(const MatrixView<T, IsConst> m1,
                   const MatrixView<T, IsConst> m2,
                   MatrixView<T, false> out_m) {
    for (int c = 0; c < m1.ColNum(); c++) {
        for (int r = 0; r < m1.RowNum(); r++) {
            out_m[c][r] = m1[c][r] * m2[c][r];
        }
    }
}

template <typename T, bool IsConst>
void MatrixDivEach(const MatrixView<T, IsConst> m1,
                   const MatrixView<T, IsConst> m2,
                   MatrixView<T, false> out_m) {
    for (int c = 0; c < m1.ColNum(); c++) {
        for (int r = 0; r < m1.RowNum(); r++) {
            out_m[c][r] = m1[c][r] / m2[c][r];
        }
    }
}

template <typename T, bool IsConst>
bool MatrixEq(const MatrixView<T, IsConst> m1,
              const MatrixView<T, IsConst> m2) noexcept {
    for (size_t c = 0; c < m1.ColNum(); c++) {
        for (size_t r = 0; r < m1.RowNum(); r++) {
            if (m1[c][r] != m2[c][r]) {
                return false;
            }
        }
    }
    return true;
}

template <typename T, bool IsConst>
void Transpose(const MatrixView<T, IsConst> m, MatrixView<T, false> out_m) {
    for (int c = 0; c < m.ColNum(); c++) {
        for (int r = 0; r < m.RowNum(); r++) {
            out_m[r][c] = m[c][r];
        }
    }
}

template <typename T, bool IsConst>
float Det(const MatrixView<T, IsConst> m) {
    NICKEL_ASSERT(m.IsSquare());
    auto len = m.RowNum();
    if (len == 1) {
        return m[0][0];
    }
    if (len == 2) {
        return m[0][0] * m[1][1] - m[0][1] * m[1][0];
    }
    if (len == 3) {
        return m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] +
               m[1][0] * m[2][1] * m[0][2] -
               (m[2][0] * m[1][1] * m[0][2] + m[1][0] * m[0][1] * m[2][2] +
                m[0][0] * m[2][1] * m[1][2]);
    }

    float result = 0;
    for (int c = 0; c < len; c++) {
        Matrix<T> temp(len - 1, len - 1);
        for (int idx = 0; idx < len; idx++) {
            if (idx == c) continue;
            for (int r = 1; r < len; r++) {
                temp[idx < c ? idx : idx - 1][r - 1] = m[idx][r];
            }
        }
        result += ((c % 2 == 0) ? 1 : -1) * m[c][0] * Det(MatrixView{temp});
    }
    return result;
}

template <typename T, bool IsConst>
auto Cofactor(const MatrixView<T, IsConst> m, size_t col, size_t row,
              MatrixView<T, false> out_m) {
    for (int c = 0; c < m.ColNum(); c++) {
        if (c == col) continue;
        for (int r = 0; r < m.RowNum(); r++) {
            if (r == row) continue;
            out_m[c < col ? c : c - 1][r < row ? r : r - 1] = m[c][r];
        }
    }
    return out_m;
}

template <typename T, bool IsConst>
auto Adjoint(const MatrixView<T, IsConst> m, MatrixView<T, false> out_m) {
    for (int c = 0; c < m.ColNum(); c++) {
        for (int r = 0; r < m.RowNum(); r++) {
            out_m[c][r] = ((c + r) % 2 == 0 ? 1 : -1) * Det(Cofactor(m, c, r));
        }
    }
}

/**
 * @brief please make sure m has inverse matrix
 */
template <typename T, bool IsConst>
void Inverse(const MatrixView<T, IsConst> m, MatrixView<T, false> out_m) {
    Adjoint(m, out_m);
    out_m / Det(m);
}

enum class ReducedRowEchelonFormPolicy {
    DontStop,
    StopWhenAllZeroInLine,
};

template <typename T>
void RowEchelonForm(const MatrixView<T, false> m) {
    size_t minNum = std::min(m.RowNum(), m.ColNum());
    for (int i = 0; i < minNum; i++) {
        // rearrange
        size_t swapRow = i;
        for (int r = i + 1; r < m.RowNum(); r++) {
            if (std::abs(m[i][i]) < std::abs(m[i][r])) {
                swapRow = r;
            }
        }

        for (int c = 0; c < m.ColNum(); c++) {
            std::swap(m[c][i], m[c][swapRow]);
        }

        // gaussian elimination
        for (int r = i + 1; r < m.RowNum(); r++) {
            float value = m[i][r] / m[i][i];
            for (int k = i; k < m.ColNum(); k++) {
                m[k][r] -= value * m[k][i];
            }
        }
    }
}

/**
 * @brief require m is RowEchelonForm
 * @return bool  whether the algorithm is completed
 */
template <typename T>
bool ReducedRowEchelonForm(const MatrixView<T, false> m,
                           ReducedRowEchelonFormPolicy policy) {
    for (int r = m.RowNum() - 1; r >= 0; r--) {
        int lastNozeroIdx = 0;
        while (lastNozeroIdx < m.ColNum() && m[lastNozeroIdx][r] == 0) {
            lastNozeroIdx++;
        }
        if (lastNozeroIdx == m.ColNum()) {
            if (policy == ReducedRowEchelonFormPolicy::StopWhenAllZeroInLine) {
                return false;
            }
            continue;
        }

        for (int nr = r - 1; nr >= 0; nr--) {
            float value = m[lastNozeroIdx][nr] / m[lastNozeroIdx][r];
            for (int c = lastNozeroIdx; c < m.ColNum(); c++) {
                m[c][nr] -= value * m[c][r];
            }
        }
        float fstNozeroElem = m[lastNozeroIdx][r];
        for (int c = lastNozeroIdx; c < m.ColNum(); c++) {
            m[c][r] /= fstNozeroElem;
        }
    }
    return true;
}

/**
 * @brief solve linear equations by Gaussian-Jordan-Elimination inplace
 * @note m is Augmented matrix
 */
template <typename T>
bool Solve(MatrixView<T, false> m) {
    if (m.ColNum() != m.RowNum() + 1) {
        return false;
    }

    RowEchelonForm(m);
    if (!ReducedRowEchelonForm(
            m, ReducedRowEchelonFormPolicy::StopWhenAllZeroInLine)) {
        return false;
    }

    return true;
}

// SVector functions

template <typename T, size_t Len>
T Dot(const SVector<T, Len>& v1, const SVector<T, Len>& v2) {
    T sum = {};
    for (int i = 0; i < Len; i++) {
        sum += v1[i] * v2[i];
    }
    return sum;
}

template <typename T, size_t Len>
T LengthSqrd(const SVector<T, Len>& v) {
    return Dot(v, v);
}

template <typename T, size_t Len>
T Length(const SVector<T, Len>& v) {
    return std::sqrt(LengthSqrd(v));
}

template <typename T>
T Cross(const SVector<T, 2>& v1, const SVector<T, 2>& v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

template <typename T>
SVector<T, 3> Cross(const SVector<T, 3>& v1, const SVector<T, 3>& v2) {
    SVector<T, 3> result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    return result;
}

/**
 * @brief triple cross product: v1 x v2 x v3
 */
template <typename T>
SVector<T, 3> TripleCross(const SVector<T, 3>& v1, const SVector<T, 3>& v2,
                          const SVector<T, 3>& v3) {
    return Cross(Cross(v1, v2), v3);
}

/**
 * @brief mixed product: (v1 x v2) * v3
 */
template <typename T>
SVector<T, 3> MixedProduct(const SVector<T, 3>& v1, const SVector<T, 3>& v2,
                           const SVector<T, 3>& v3) {
    return Dot(Cross(v1, v2), v3);
}

template <typename T, size_t N>
T Project(const SVector<T, N>& src, const SVector<T, N>& des) {
    return Dot(src, des) / Length(des);
}

template <typename T, size_t N>
SVector<T, N> Normalize(const SVector<T, N>& v) {
    return v / Length(v);
}

template <typename T>
SVector<T, 2> PerpendicVector(const SVector<T, 2>& v) {
    return {-v.y, v.x};
}

// create functions

template <typename T>
SMatrix<T, 4, 4> CreatePersp(T fov, T aspect, T n, T f) {
    T focal = 1.0 / std::tan(fov * 0.5);

    // clang-format off
    return SMatrix<T, 4, 4>::FromRow(
        focal / aspect,       0,                   0,                       0,
                     0,  -focal,                   0,                       0,
                     0,       0, (2.f * n) / (f - n), (2.f * n * f) / (f - n),
                     0,       0,                  -1,                       0
    );
    // clang-format on
}

template <typename T>
SMatrix<T, 4, 4> CreateOrtho(T left, T right, T top, T bottom, T n, T f) {
    // clang-format off
    return SMatrix<T, 4, 4>::FromRow(
        2.0f / (right - left),                  0.0f,               0.0f, (left + right) / (left - right),
                         0.0f,-2.0f / (top - bottom),               0.0f, (bottom + top) / (bottom - top),
                         0.0f,                  0.0f,     1.0f / (n - f),                      f/ (f - n),
                         0.0f,                  0.0f,               0.0f,                           1.0f
    );
    // clang-format on
}

template <typename T>
SMatrix<T, 4, 4> CreateTranslation(const SVector<T, 3>& position) {
    // clang-format off
    return SMatrix<T, 4, 4>::FromRow(
        1.0f, 0.0f, 0.0f, position.x,
        0.0f, 1.0f, 0.0f, position.y,
        0.0f, 0.0f, 1.0f, position.z,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on
}

template <typename T>
SMatrix<T, 4, 4> LookAt(const SVector<T, 3>& target,
                        const SVector<T, 3>& srcPos, const SVector<T, 3>& up) {
    NICKEL_ASSERT(up.LengthSqrd() == 1, "look at param up must be normalized");

    auto zAxis = Normalize(srcPos - target);
    auto xAxis = Normalize(up.Cross(zAxis));
    auto yAxis = zAxis.Cross(xAxis);

    // clang-format off
    return SMatrix<T, 4, 4>::FromRow(
        xAxis.x, xAxis.y, xAxis.z, -xAxis.Dot(srcPos),
        yAxis.x, yAxis.y, yAxis.z, -yAxis.Dot(srcPos),
        zAxis.x, zAxis.y, zAxis.z, -zAxis.Dot(srcPos),
             0,      0,       0,                    1
    );
    // clang-format on
}

template <typename T>
SMatrix<T, 4, 4> CreateXRotation(float radians) {
    float cos = std::cos(radians);
    float sin = std::sin(radians);
    // clang-format off
    return SMatrix<T, 4, 4>::FromRow(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f,  cos, -sin, 0.0f,
        0.0f,  sin,  cos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on
}

template <typename T>
SMatrix<T, 4, 4> CreateYRotation(float radians) {
    float cos = std::cos(radians);
    float sin = std::sin(radians);
    // clang-format off
    return SMatrix<T, 4, 4>::FromRow(
         cos, 0.0f,  sin, 0.0f,
        0.0f, 1.0f,  0.0, 0.0f,
        -sin, 0.0f,  cos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on
}

template <typename T>
SMatrix<T, 4, 4> CreateZRotation(float radians) {
    float cos = std::cos(radians);
    float sin = std::sin(radians);
    // clang-format off
    return SMatrix<T, 4, 4>::FromRow(
         cos, -sin, 0.0f, 0.0f,
         sin,  cos, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on
}

template <typename T>
SMatrix<T, 4, 4> CreateXYZRotation(const SVector<T, 3>& r) {
    return CreateXRotation(r.x) * CreateYRotation(r.y) * CreateZRotation(r.z);
}

template <typename T>
SMatrix<T, 2, 2> CreateRotation2D(float radians) {
    auto cos = std::cos(radians);
    auto sin = std::sin(radians);
    // clang-format off
    return SMatrix<T, 2, 2>::FromRow(
        cos, -sin,
        sin, cos
    );
    // clang-format on
}

template <typename T>
SMatrix<T, 4, 4> CreateScale(const SVector<T, 3>& scale) {
    // clang-format off
    return SMatrix<T, 4, 4>::FromRow(
        scale.x,     0.0,    0.0f, 0.0f,
           0.0f, scale.y,    0.0f, 0.0f,
           0.0f,    0.0f, scale.z, 0.0f,
           0.0f,    0.0f,    0.0f, 1.0f
    );
    // clang-format on
}

template <typename T>
struct TRect {
    SVector<T, 2> position, size;

    TRect() = default;

    TRect(const SVector<T, 2>& position, const SVector<T, 2>& size)
        : TRect(position.x, position.y, size.w, size.h) {}

    TRect(float x, float y, float w, float h) : position{x, y}, size{w, h} {}

    TRect(const TRect&) = default;
    TRect& operator=(const TRect&) = default;

    static TRect FromCenter(const SVector<T, 2>& center,
                            const SVector<T, 2>& halfSize) {
        return TRect{center - halfSize, halfSize * 2.0};
    }

    bool IsPtIn(const SVector<T, 2>& v) const {
        return v.x > position.x && v.x < position.x + size.w &&
               v.y > position.y && v.y < position.y + size.h;
    }

    /**
     * @brief return two rects intersect
     * @note if rects are not intersect, will return a invalid rect
     */
    TRect Intersect(const TRect& o) const {
        auto minX = std::max(position.x, o.position.x);
        auto minY = std::max(position.y, o.position.y);
        auto maxX = std::min(position.x + size.w, o.position.x + o.size.w);
        auto maxY = std::min(position.y + size.h, o.position.y + o.size.h);
        return TRect(minX, minY, maxX - minX, maxY - minY);
    }

    bool IsIntersect(const TRect& o) const {
        return !(position.x > o.position.x + o.size.w ||
                 position.x + size.w < o.position.x ||
                 position.y > o.position.y + o.size.h ||
                 position.y + size.h < o.position.y);
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
    return !(min1 >= max2 || min2 >= max1);
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
T GetRadianIn2PI(const SVector<T, 2>& v1, const SVector<T, 2>& v2) {
    auto cos = Dot(v1, v2);
    auto sin = Cross(v1, v2);

    return Wrap<float>(std::acos(cos) * Sign(sin), 0, 2 * GenericPI<T>);
}

/**
 * @brief Get degree between v1 & v2 in [0, 2 * PI)
 * @note v1 & v2 are both normalized
 */
template <typename T>
T GetRadianIn360(const SVector<T, 3>& v1, const SVector<T, 3>& v2,
                 const SVector<T, 3>& rightAxis) {
    auto cos = Dot(v1, v2);
    auto sin = Dot(Cross(v1, v2), rightAxis);

    if (Sign(sin) >= 0) {
        return std::acos(cos);
    }
    return 2 * GenericPI<T> - std::acos(cos);
}

/**
 * @brief Get degree between v1 & v2 in [-PI, PI)
 * @note v1 & v2 are both normalized
 */
template <typename T>
T GetRadianInPISigned(const SVector<T, 2>& v1, const SVector<T, 2>& v2) {
    auto cos = Dot(v1, v2);
    auto sin = Cross(v1, v2);
    return std::acos(cos) * Sign(sin);
}

template <typename T>
std::pair<SVector<T, 3>, SVector<T, 3>> GetNormalMapTB(
    SVector<T, 3> p1, SVector<T, 3> p2, SVector<T, 3> p3, SVector<T, 2> uv1,
    SVector<T, 2> uv2, SVector<T, 2> uv3) {
    NICKEL_ASSERT(!(uv1 == SVector<T, 2>{} && uv2 == SVector<T, 2>{} &&
                    uv3 == SVector<T, 2>{}),
                  "uvs are (0, 0) will cause numerical error");
    SVector<T, 3> e1{p2 - p1}, e2{p3 - p1};
    SVector<T, 2> dUV1{uv2 - uv1}, dUV2{uv3 - uv1};
    float denoInv = 1.0 / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

    // clang-format off
    auto t = SVector<T, 3>{
                   dUV2.y * e1.x - dUV1.y * e2.x,
                   dUV2.y * e1.y - dUV1.y * e2.y,
                   dUV2.y * e1.z - dUV1.y * e2.z} * denoInv;
    auto b = SVector<T, 3>{
                   -dUV2.x * e1.x + dUV1.x * e2.x,
                   -dUV2.x * e1.y + dUV1.x * e2.y,
                   -dUV2.x * e1.z + dUV1.x * e2.z} * denoInv;
    // clang-format on

    return {Normalize(t), Normalize(b)};
}

template <typename T>
SVector<T, 3> GetNormalMapTangent(SVector<T, 3> p1, SVector<T, 3> p2,
                                  SVector<T, 3> p3, SVector<T, 2> uv1,
                                  SVector<T, 2> uv2, SVector<T, 2> uv3) {
    Assert(!(uv1 == SVector<T, 2>{} && uv2 == SVector<T, 2>{} &&
             uv3 == SVector<T, 2>{}),
           "uvs are (0, 0) will cause numerical error");
    SVector<T, 3> e1{p2 - p1}, e2{p3 - p1};
    SVector<T, 2> dUV1{uv2 - uv1}, dUV2{uv3 - uv1};
    float denoInv = 1.0 / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

    return SVector<T, 3>{dUV2.y * e1.x - dUV1.y * e2.x,
                         dUV2.y * e1.y - dUV1.y * e2.y,
                         dUV2.y * e1.z - dUV1.y * e2.z} *
           denoInv;
}

template <typename T>
T Clamp(T value, T min, T max) {
    return value < min ? min : value > max ? max : value;
}

}  // namespace nickel
