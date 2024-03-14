#pragma once

#include <cstdint>
#include <type_traits>

namespace nickel::rhi {

enum class APIPreference {
    Undefine,
    Null,
    GL,
    Vulkan,
};

enum class BufferUsage {
    MapRead = 0x0001,
    MapWrite = 0x0002,
    CopySrc = 0x0004,
    CopyDst = 0x0008,
    Index = 0x0010,
    Vertex = 0x0020,
    Uniform = 0x0040,
    Storage = 0x0080,
    Indirect = 0x0100,
    QueryResolve = 0x0200,
};

enum class SamplerAddressMode {
    ClampToEdge,
    Repeat,
    MirrorRepeat,
};

enum class Filter {
    Nearest,
    Linear,
};

enum class CompareOp {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
};

enum class TextureType {
    Dim1,
    Dim2,
    Dim3,
};

enum class TextureViewType {
    Dim1,
    Dim2,
    Dim3,
    Dim2Array,
    Cube,
    CubeArray,
};

enum class TextureUsage {
    CopySrc = 0x01,
    CopyDst = 0x02,
    TextureBinding = 0x04,
    StorageBinding = 0x08,
    RenderAttachment = 0x10,
};

enum class SampleCount {
    Count1,
    Count2,
    Count4,
    Count8,
    Count16,
    Count32,
    Count64,
};

enum class TextureFormat {
    Undefined,
    Presentation,

    // 8_bit formats
    R8_UNORM,
    R8_SNORM,
    R8_UINT,
    R8_SINT,

    // 16_bit formats
    R16_UINT,
    R16_SINT,
    R16_FLOAT,
    RG8_UNORM,
    RG8_SNORM,
    RG8_UINT,
    RG8_SINT,

    // 32_bit formats
    R32_UINT,
    R32_SINT,
    R32_FLOAT,
    RG16_UINT,
    RG16_SINT,
    RG16_FLOAT,
    RGBA8_UNORM,
    RGBA8_UNORM_SRGB,
    RGBA8_SNORM,
    RGBA8_UINT,
    RGBA8_SINT,
    BGRA8_UNORM,
    BGRA8_UNORM_SRGB,
    // Packed 32_bit formats
    RGB9E5_UFLOAT,
    RGB10A2_UINT,
    RGB10A2_UNORM,
    RG11B10_UFLOAT,

    // 64_bit formats
    RG32_UINT,
    RG32_SINT,
    RG32_FLOAT,
    RGBA16_UINT,
    RGBA16_SINT,
    RGBA16_FLOAT,

    // 128_bit formats
    RGBA32_UINT,
    RGBA32_SINT,
    RGBA32_FLOAT,

    // Depth/stencil formats
    STENCIL8,
    DEPTH16_UNORM,
    DEPTH24_PLUS,
    DEPTH24_PLUS_STENCIL8,
    DEPTH32_FLOAT,

    // "depth32float_stencil8" feature
    DEPTH32_FLOAT_STENCIL8,

    // BC compressed formats usable if "texture_compression_bc" is both
    // supported by the device/user agent and enabled in requestDevice.
    BC1_RGBA_UNORM,
    BC1_RGBA_UNORM_SRGB,
    BC2_RGBA_UNORM,
    BC2_RGBA_UNORM_SRGB,
    BC3_RGBA_UNORM,
    BC3_RGBA_UNORM_SRGB,
    BC4_R_UNORM,
    BC4_R_SNORM,
    BC5_RG_UNORM,
    BC5_RG_SNORM,
    BC6H_RGB_UFLOAT,
    BC6H_RGB_FLOAT,
    BC7_RGBA_UNORM,
    BC7_RGBA_UNORM_SRGB,

    // ETC2 compressed formats usable if "texture_compression_etc2" is both
    // supported by the device/user agent and enabled in requestDevice.
    ETC2_RGB8_UNORM,
    ETC2_RGB8_UNORM_SRGB,
    ETC2_RGB8A1_UNORM,
    ETC2_RGB8A1_UNORM_SRGB,
    ETC2_RGBA8_UNORM,
    ETC2_RGBA8_UNORM_SRGB,
    EAC_R11_UNORM,
    EAC_R11_SNORM,
    EAC_RG11_UNORM,
    EAC_RG11_SNORM,

    // ASTC compressed formats usable if "texture_compression_astc" is both
    // supported by the device/user agent and enabled in requestDevice.
    ASTC_4X4_UNORM,
    ASTC_4X4_UNORM_SRGB,
    ASTC_5X4_UNORM,
    ASTC_5X4_UNORM_SRGB,
    ASTC_5X5_UNORM,
    ASTC_5X5_UNORM_SRGB,
    ASTC_6X5_UNORM,
    ASTC_6X5_UNORM_SRGB,
    ASTC_6X6_UNORM,
    ASTC_6X6_UNORM_SRGB,
    ASTC_8X5_UNORM,
    ASTC_8X5_UNORM_SRGB,
    ASTC_8X6_UNORM,
    ASTC_8X6_UNORM_SRGB,
    ASTC_8X8_UNORM,
    ASTC_8X8_UNORM_SRGB,
    ASTC_10X5_UNORM,
    ASTC_10X5_UNORM_SRGB,
    ASTC_10X6_UNORM,
    ASTC_10X6_UNORM_SRGB,
    ASTC_10X8_UNORM,
    ASTC_10X8_UNORM_SRGB,
    ASTC_10X10_UNORM,
    ASTC_10X10_UNORM_SRGB,
    ASTC_12X10_UNORM,
    ASTC_12X10_UNORM_SRGB,
    ASTC_12X12_UNORM,
    ASTC_12X12_UNORM_SRGB,
};

struct Extent2D {
    uint32_t width, height;

    bool operator==(const Extent2D& o) const {
        return o.width == width && o.height == height;
    }

    bool operator!=(const Extent2D& o) const { return !(*this == o); }
};

struct Extent3D {
    uint32_t width, height, depthOrArrayLayers;

    bool operator==(const Extent3D& e) const {
        return width == e.width && height == e.height &&
               depthOrArrayLayers == e.depthOrArrayLayers;
    }

    bool operator!=(const Extent3D& e) const { return !(*this == e); }
};

struct Offset3D {
    uint32_t x{}, y{}, z{};
};

enum class TextureAspect {
    All,
    DepthOnly,
    StencilOnly,
};

enum class ShaderStage {
    Vertex = 0x01,
    Compute = 0x02,
    Fragment = 0x04,
    Geometry = 0x08,
};

enum class DepthCompare {
    DecrementClamp,
    DecrementWrap,
    Invert,
    IncrementClamp,
    IncrementWrap,
    Keep,
    Replace,
    Zero,
};

enum class StencilOp {
    Keep,
    Zero,
    Replace,
    IncrementAndClamp,
    DecrementAndClamp,
    Invert,
    IncrementAndWrap,
    DecrementAndWrap,
};

enum class CullMode {
    Back,
    Front,
    None,
};

enum class FrontFace {
    CCW,
    CW,
};

enum class StripIndexFormat {
    Uint16,
    Uint32,
};

enum class Topology {
    LineList,
    LineStrip,
    PointList,
    TriangleList,
    TriangleStrip,
    TriangleFan,
};

enum class FlagsConstant {
    Red = 0x01,
    Green = 0x02,
    Blue = 0x04,
    Alpha = 0x08,
    All = Red | Green | Blue | Alpha,
};

enum class BlendFactor {
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    ConstantColor,
    OneMinusConstantColor,
    ConstantAlpha,
    OneMinusConstantAlpha,
    SrcAlphaSaturate,
};

enum class BlendOp {
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max,
};

struct Offset final {
    int32_t x, y;

    bool operator==(const Offset& lhs) const {
        return x == lhs.x && y == lhs.y;
    }

    bool operator!=(const Offset& lhs) const { return !(*this == lhs); }
};

struct Rect2D final {
    Offset offset;
    Extent2D extent;

    bool operator==(const Rect2D& o) const {
        return o.offset == offset && o.extent == extent;
    }

    bool operator!=(const Rect2D& o) const { return !(*this == o); }
};

enum class PolygonMode {
    Line,
    Fill,
    Point,
};

enum class AttachmentLoadOp {
    Clear,
    Load,
};

enum class AttachmentStoreOp {
    Store,
    Discard,
};

enum class IndexType {
    Uint16,
    Uint32,
};

enum class ColorWriteMask {
    Red = 0x01,
    Green = 0x02,
    Blue = 0x04,
    Alpha = 0x08,
    All = 0x01 | 0x02 | 0x04 | 0x08,
};

enum class VertexFormat {
    Uint8x2,
    Uint8x4,
    Sint8x2,
    Sint8x4,
    Unorm8x2,
    Unorm8x4,
    Snorm8x2,
    Snorm8x4,
    Uint16x2,
    Uint16x4,
    Sint16x2,
    Sint16x4,
    Unorm16x2,
    Unorm16x4,
    Snorm16x2,
    Snorm16x4,
    Float16x2,
    Float16x4,
    Float32,
    Float32x2,
    Float32x3,
    Float32x4,
    Uint32,
    Uint32x2,
    Uint32x3,
    Uint32x4,
    Sint32,
    Sint32x2,
    Sint32x3,
    Sint32x4,
    Unorm10_10_10_2,
};

template <typename T>
struct Flags final {
public:
    static_assert(std::is_enum_v<T>);

    using underlying_type = std::underlying_type_t<T>;

    constexpr Flags(): data_{0} {}
    constexpr Flags(T mask) : data_{static_cast<underlying_type>(mask)} {}
    constexpr Flags(const Flags&) = default;
    explicit constexpr Flags(underlying_type mask) : data_{mask} {}

    constexpr Flags operator|(T mask) const {
        return Flags(static_cast<underlying_type>(mask) | data_);
    }

    constexpr Flags operator|(Flags mask) const {
        return Flags(static_cast<underlying_type>(mask.data_) | data_);
    }

    constexpr Flags operator&(T mask) const {
        return Flags(static_cast<underlying_type>(mask) & data_);
    }

    constexpr Flags operator&(Flags mask) const {
        return Flags(static_cast<underlying_type>(mask.data_) & data_);
    }

    constexpr Flags& operator|=(T mask) {
        data_ |= static_cast<underlying_type>(mask);
        return *this;
    }

    constexpr Flags& operator|=(Flags mask) {
        data_ |= static_cast<underlying_type>(mask.data_);
        return *this;
    }

    constexpr Flags& operator&=(T mask) {
        data_ &= static_cast<underlying_type>(mask);
        return *this;
    }

    constexpr Flags& operator&=(Flags mask) {
        data_ &= static_cast<underlying_type>(mask.data_);
        return *this;
    }

    constexpr bool operator==(T mask) const {
        return data_ == static_cast<underlying_type>(mask);
    }

    constexpr bool operator==(Flags mask) const {
        return data_ == mask.data_;
    }

    constexpr bool operator!=(T mask) const {
        return !(*this == mask);
    }

    constexpr bool operator!=(Flags mask) const {
        return !(*this != mask);
    }

    constexpr Flags& operator=(T mask) {
        data_ = static_cast<underlying_type>(mask);
        return *this;
    }

    constexpr Flags& operator=(Flags mask) {
        data_ = mask.data_;
        return *this;
    }

    constexpr Flags& operator=(underlying_type mask) {
        data_ = mask;
        return *this;
    }

    constexpr operator T() const { return static_cast<T>(data_); }
    constexpr operator underlying_type() const { return data_; }

private:
    underlying_type data_;
};

}  // namespace nickel::rhi