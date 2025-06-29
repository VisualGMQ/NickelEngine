#pragma once
#include "nickel/common/refl_macro.hpp"

namespace nickel::graphics {


enum  class NICKEL_REFL_ATTR(refl, script) ImageAspect {
    None = 0x00,
    Color = 0x01,
    Depth = 0x02,
    Stencil = 0x04,
};

enum class NICKEL_REFL_ATTR(refl, script) CompareOp {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
};

enum class NICKEL_REFL_ATTR(refl, script) ImageType {
    Dim1,
    Dim2,
    Dim3,
};

enum class NICKEL_REFL_ATTR(refl) BufferUsage {
    CopySrc = 0x0004,
    CopyDst = 0x0008,
    Index = 0x0010,
    Vertex = 0x0020,
    Uniform = 0x0040,
    Storage = 0x0080,
    Indirect = 0x0100,
};

enum class NICKEL_REFL_ATTR(refl) MemoryType {
    CPULocal,
    Coherence,
    GPULocal,
};

enum class NICKEL_REFL_ATTR(refl) SamplerAddressMode {
    ClampToEdge = 0,
    Repeat = 1,
    MirrorRepeat = 2,
};

enum class NICKEL_REFL_ATTR(refl) Filter {
    Nearest = 0,
    Linear = 1,
};

enum class NICKEL_REFL_ATTR(refl) ImageViewType {
    Dim1,
    Dim2,
    Dim3,
    Dim2Array,
    Cube,
    CubeArray,
};

enum class NICKEL_REFL_ATTR(refl) ImageUsage {
    CopySrc = 0x01,
    CopyDst = 0x02,
    Sampled = 0x04,
    StorageBinding = 0x08,
    ColorAttachment = 0x10,
    DepthStencilAttachment = 0x20,
};

enum class NICKEL_REFL_ATTR(refl) SampleCount {
    Count1,
    Count2,
    Count4,
    Count8,
    Count16,
    Count32,
    Count64,
};

// don't change value! copied from volk.hpp directly
enum class NICKEL_REFL_ATTR(refl) Format {
    UNDEFINED = 0,

    R4G4_UNORM_PACK8 = 1,
    R4G4B4A4_UNORM_PACK16 = 2,
    B4G4R4A4_UNORM_PACK16 = 3,
    R5G6B5_UNORM_PACK16 = 4,
    B5G6R5_UNORM_PACK16 = 5,
    R5G5B5A1_UNORM_PACK16 = 6,
    B5G5R5A1_UNORM_PACK16 = 7,
    A1R5G5B5_UNORM_PACK16 = 8,
    R8_UNORM = 9,
    R8_SNORM = 10,
    R8_USCALED = 11,
    R8_SSCALED = 12,
    R8_UINT = 13,
    R8_SINT = 14,
    R8_SRGB = 15,
    R8G8_UNORM = 16,
    R8G8_SNORM = 17,
    R8G8_USCALED = 18,
    R8G8_SSCALED = 19,
    R8G8_UINT = 20,
    R8G8_SINT = 21,
    R8G8_SRGB = 22,
    R8G8B8_UNORM = 23,
    R8G8B8_SNORM = 24,
    R8G8B8_USCALED = 25,
    R8G8B8_SSCALED = 26,
    R8G8B8_UINT = 27,
    R8G8B8_SINT = 28,
    R8G8B8_SRGB = 29,
    B8G8R8_UNORM = 30,
    B8G8R8_SNORM = 31,
    B8G8R8_USCALED = 32,
    B8G8R8_SSCALED = 33,
    B8G8R8_UINT = 34,
    B8G8R8_SINT = 35,
    B8G8R8_SRGB = 36,
    R8G8B8A8_UNORM = 37,
    R8G8B8A8_SNORM = 38,
    R8G8B8A8_USCALED = 39,
    R8G8B8A8_SSCALED = 40,
    R8G8B8A8_UINT = 41,
    R8G8B8A8_SINT = 42,
    R8G8B8A8_SRGB = 43,
    B8G8R8A8_UNORM = 44,
    B8G8R8A8_SNORM = 45,
    B8G8R8A8_USCALED = 46,
    B8G8R8A8_SSCALED = 47,
    B8G8R8A8_UINT = 48,
    B8G8R8A8_SINT = 49,
    B8G8R8A8_SRGB = 50,
    A8B8G8R8_UNORM_PACK32 = 51,
    A8B8G8R8_SNORM_PACK32 = 52,
    A8B8G8R8_USCALED_PACK32 = 53,
    A8B8G8R8_SSCALED_PACK32 = 54,
    A8B8G8R8_UINT_PACK32 = 55,
    A8B8G8R8_SINT_PACK32 = 56,
    A8B8G8R8_SRGB_PACK32 = 57,
    A2R10G10B10_UNORM_PACK32 = 58,
    A2R10G10B10_SNORM_PACK32 = 59,
    A2R10G10B10_USCALED_PACK32 = 60,
    A2R10G10B10_SSCALED_PACK32 = 61,
    A2R10G10B10_UINT_PACK32 = 62,
    A2R10G10B10_SINT_PACK32 = 63,
    A2B10G10R10_UNORM_PACK32 = 64,
    A2B10G10R10_SNORM_PACK32 = 65,
    A2B10G10R10_USCALED_PACK32 = 66,
    A2B10G10R10_SSCALED_PACK32 = 67,
    A2B10G10R10_UINT_PACK32 = 68,
    A2B10G10R10_SINT_PACK32 = 69,
    R16_UNORM = 70,
    R16_SNORM = 71,
    R16_USCALED = 72,
    R16_SSCALED = 73,
    R16_UINT = 74,
    R16_SINT = 75,
    R16_SFLOAT = 76,
    R16G16_UNORM = 77,
    R16G16_SNORM = 78,
    R16G16_USCALED = 79,
    R16G16_SSCALED = 80,
    R16G16_UINT = 81,
    R16G16_SINT = 82,
    R16G16_SFLOAT = 83,
    R16G16B16_UNORM = 84,
    R16G16B16_SNORM = 85,
    R16G16B16_USCALED = 86,
    R16G16B16_SSCALED = 87,
    R16G16B16_UINT = 88,
    R16G16B16_SINT = 89,
    R16G16B16_SFLOAT = 90,
    R16G16B16A16_UNORM = 91,
    R16G16B16A16_SNORM = 92,
    R16G16B16A16_USCALED = 93,
    R16G16B16A16_SSCALED = 94,
    R16G16B16A16_UINT = 95,
    R16G16B16A16_SINT = 96,
    R16G16B16A16_SFLOAT = 97,
    R32_UINT = 98,
    R32_SINT = 99,
    R32_SFLOAT = 100,
    R32G32_UINT = 101,
    R32G32_SINT = 102,
    R32G32_SFLOAT = 103,
    R32G32B32_UINT = 104,
    R32G32B32_SINT = 105,
    R32G32B32_SFLOAT = 106,
    R32G32B32A32_UINT = 107,
    R32G32B32A32_SINT = 108,
    R32G32B32A32_SFLOAT = 109,
    R64_UINT = 110,
    R64_SINT = 111,
    R64_SFLOAT = 112,
    R64G64_UINT = 113,
    R64G64_SINT = 114,
    R64G64_SFLOAT = 115,
    R64G64B64_UINT = 116,
    R64G64B64_SINT = 117,
    R64G64B64_SFLOAT = 118,
    R64G64B64A64_UINT = 119,
    R64G64B64A64_SINT = 120,
    R64G64B64A64_SFLOAT = 121,
    B10G11R11_UFLOAT_PACK32 = 122,
    E5B9G9R9_UFLOAT_PACK32 = 123,
    D16_UNORM = 124,
    X8_D24_UNORM_PACK32 = 125,
    D32_SFLOAT = 126,
    S8_UINT = 127,
    D16_UNORM_S8_UINT = 128,
    D24_UNORM_S8_UINT = 129,
    D32_SFLOAT_S8_UINT = 130,
    BC1_RGB_UNORM_BLOCK = 131,
    BC1_RGB_SRGB_BLOCK = 132,
    BC1_RGBA_UNORM_BLOCK = 133,
    BC1_RGBA_SRGB_BLOCK = 134,
    BC2_UNORM_BLOCK = 135,
    BC2_SRGB_BLOCK = 136,
    BC3_UNORM_BLOCK = 137,
    BC3_SRGB_BLOCK = 138,
    BC4_UNORM_BLOCK = 139,
    BC4_SNORM_BLOCK = 140,
    BC5_UNORM_BLOCK = 141,
    BC5_SNORM_BLOCK = 142,
    BC6H_UFLOAT_BLOCK = 143,
    BC6H_SFLOAT_BLOCK = 144,
    BC7_UNORM_BLOCK = 145,
    BC7_SRGB_BLOCK = 146,
    ETC2_R8G8B8_UNORM_BLOCK = 147,
    ETC2_R8G8B8_SRGB_BLOCK = 148,
    ETC2_R8G8B8A1_UNORM_BLOCK = 149,
    ETC2_R8G8B8A1_SRGB_BLOCK = 150,
    ETC2_R8G8B8A8_UNORM_BLOCK = 151,
    ETC2_R8G8B8A8_SRGB_BLOCK = 152,
    EAC_R11_UNORM_BLOCK = 153,
    EAC_R11_SNORM_BLOCK = 154,
    EAC_R11G11_UNORM_BLOCK = 155,
    EAC_R11G11_SNORM_BLOCK = 156,
    ASTC_4x4_UNORM_BLOCK = 157,
    ASTC_4x4_SRGB_BLOCK = 158,
    ASTC_5x4_UNORM_BLOCK = 159,
    ASTC_5x4_SRGB_BLOCK = 160,
    ASTC_5x5_UNORM_BLOCK = 161,
    ASTC_5x5_SRGB_BLOCK = 162,
    ASTC_6x5_UNORM_BLOCK = 163,
    ASTC_6x5_SRGB_BLOCK = 164,
    ASTC_6x6_UNORM_BLOCK = 165,
    ASTC_6x6_SRGB_BLOCK = 166,
    ASTC_8x5_UNORM_BLOCK = 167,
    ASTC_8x5_SRGB_BLOCK = 168,
    ASTC_8x6_UNORM_BLOCK = 169,
    ASTC_8x6_SRGB_BLOCK = 170,
    ASTC_8x8_UNORM_BLOCK = 171,
    ASTC_8x8_SRGB_BLOCK = 172,
    ASTC_10x5_UNORM_BLOCK = 173,
    ASTC_10x5_SRGB_BLOCK = 174,
    ASTC_10x6_UNORM_BLOCK = 175,
    ASTC_10x6_SRGB_BLOCK = 176,
    ASTC_10x8_UNORM_BLOCK = 177,
    ASTC_10x8_SRGB_BLOCK = 178,
    ASTC_10x10_UNORM_BLOCK = 179,
    ASTC_10x10_SRGB_BLOCK = 180,
    ASTC_12x10_UNORM_BLOCK = 181,
    ASTC_12x10_SRGB_BLOCK = 182,
    ASTC_12x12_UNORM_BLOCK = 183,
    ASTC_12x12_SRGB_BLOCK = 184,
    G8B8G8R8_422_UNORM = 1000156000,
    B8G8R8G8_422_UNORM = 1000156001,
    G8_B8_R8_3PLANE_420_UNORM = 1000156002,
    G8_B8R8_2PLANE_420_UNORM = 1000156003,
    G8_B8_R8_3PLANE_422_UNORM = 1000156004,
    G8_B8R8_2PLANE_422_UNORM = 1000156005,
    G8_B8_R8_3PLANE_444_UNORM = 1000156006,
    R10X6_UNORM_PACK16 = 1000156007,
    R10X6G10X6_UNORM_2PACK16 = 1000156008,
    R10X6G10X6B10X6A10X6_UNORM_4PACK16 = 1000156009,
    G10X6B10X6G10X6R10X6_422_UNORM_4PACK16 = 1000156010,
    B10X6G10X6R10X6G10X6_422_UNORM_4PACK16 = 1000156011,
    G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 = 1000156012,
    G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 = 1000156013,
    G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 = 1000156014,
    G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 = 1000156015,
    G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 = 1000156016,
    R12X4_UNORM_PACK16 = 1000156017,
    R12X4G12X4_UNORM_2PACK16 = 1000156018,
    R12X4G12X4B12X4A12X4_UNORM_4PACK16 = 1000156019,
    G12X4B12X4G12X4R12X4_422_UNORM_4PACK16 = 1000156020,
    B12X4G12X4R12X4G12X4_422_UNORM_4PACK16 = 1000156021,
    G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 = 1000156022,
    G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 = 1000156023,
    G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 = 1000156024,
    G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 = 1000156025,
    G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 = 1000156026,
    G16B16G16R16_422_UNORM = 1000156027,
    B16G16R16G16_422_UNORM = 1000156028,
    G16_B16_R16_3PLANE_420_UNORM = 1000156029,
    G16_B16R16_2PLANE_420_UNORM = 1000156030,
    G16_B16_R16_3PLANE_422_UNORM = 1000156031,
    G16_B16R16_2PLANE_422_UNORM = 1000156032,
    G16_B16_R16_3PLANE_444_UNORM = 1000156033,
    G8_B8R8_2PLANE_444_UNORM = 1000330000,
    G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16 = 1000330001,
    G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16 = 1000330002,
    G16_B16R16_2PLANE_444_UNORM = 1000330003,
    A4R4G4B4_UNORM_PACK16 = 1000340000,
    A4B4G4R4_UNORM_PACK16 = 1000340001,
    ASTC_4x4_SFLOAT_BLOCK = 1000066000,
    ASTC_5x4_SFLOAT_BLOCK = 1000066001,
    ASTC_5x5_SFLOAT_BLOCK = 1000066002,
    ASTC_6x5_SFLOAT_BLOCK = 1000066003,
    ASTC_6x6_SFLOAT_BLOCK = 1000066004,
    ASTC_8x5_SFLOAT_BLOCK = 1000066005,
    ASTC_8x6_SFLOAT_BLOCK = 1000066006,
    ASTC_8x8_SFLOAT_BLOCK = 1000066007,
    ASTC_10x5_SFLOAT_BLOCK = 1000066008,
    ASTC_10x6_SFLOAT_BLOCK = 1000066009,
    ASTC_10x8_SFLOAT_BLOCK = 1000066010,
    ASTC_10x10_SFLOAT_BLOCK = 1000066011,
    ASTC_12x10_SFLOAT_BLOCK = 1000066012,
    ASTC_12x12_SFLOAT_BLOCK = 1000066013,
    PVRTC1_2BPP_UNORM_BLOCK_IMG = 1000054000,
    PVRTC1_4BPP_UNORM_BLOCK_IMG = 1000054001,
    PVRTC2_2BPP_UNORM_BLOCK_IMG = 1000054002,
    PVRTC2_4BPP_UNORM_BLOCK_IMG = 1000054003,
    PVRTC1_2BPP_SRGB_BLOCK_IMG = 1000054004,
    PVRTC1_4BPP_SRGB_BLOCK_IMG = 1000054005,
    PVRTC2_2BPP_SRGB_BLOCK_IMG = 1000054006,
    PVRTC2_4BPP_SRGB_BLOCK_IMG = 1000054007,
};

enum class NICKEL_REFL_ATTR(refl) DepthCompare {
    DecrementClamp,
    DecrementWrap,
    Invert,
    IncrementClamp,
    IncrementWrap,
    Keep,
    Replace,
    Zero,
};

enum class NICKEL_REFL_ATTR(refl) StencilOp {
    Keep,
    Zero,
    Replace,
    IncrementAndClamp,
    DecrementAndClamp,
    Invert,
    IncrementAndWrap,
    DecrementAndWrap,
};

enum class NICKEL_REFL_ATTR(refl) CullMode {
    None = 0,
    Back = 0x01,
    Front = 0x02,
};

enum class NICKEL_REFL_ATTR(refl) FrontFace {
    CCW,
    CW,
};

enum class NICKEL_REFL_ATTR(refl) StripIndexFormat {
    Uint16,
    Uint32,
};

enum class NICKEL_REFL_ATTR(refl) Topology {
    LineList,
    LineStrip,
    PointList,
    TriangleList,
    TriangleStrip,
    TriangleFan,
};

enum class NICKEL_REFL_ATTR(refl) FlagsConstant {
    Red = 0x01,
    Green = 0x02,
    Blue = 0x04,
    Alpha = 0x08,
    All = Red | Green | Blue | Alpha,
};

enum class NICKEL_REFL_ATTR(refl) BlendFactor {
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

enum class NICKEL_REFL_ATTR(refl) BlendOp {
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max,
};

enum class NICKEL_REFL_ATTR(refl) PolygonMode {
    Line,
    Fill,
    Point,
};

enum class NICKEL_REFL_ATTR(refl) AttachmentLoadOp {
    Clear,
    Load,
    DontCare,
};

enum class NICKEL_REFL_ATTR(refl) AttachmentStoreOp {
    Store,
    DontCare,
};

enum class NICKEL_REFL_ATTR(refl) IndexType {
    Uint16,
    Uint32,
};

enum class NICKEL_REFL_ATTR(refl) ColorWriteMask {
    Red = 0x01,
    Green = 0x02,
    Blue = 0x04,
    Alpha = 0x08,
    All = 0x01 | 0x02 | 0x04 | 0x08,
};

enum class NICKEL_REFL_ATTR(refl) VertexFormat {
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
};

enum class NICKEL_REFL_ATTR(refl) ShaderStage {
    Vertex = 0x01,
    TessellationControl = 0x02,
    TessellationEvaluation = 0x04,
    Geometry = 0x08,
    Fragment = 0x10,
    Compute = 0x20,
    AllGraphics = 0x1F,
    All = 0x7FFFFFFF,
};

enum class NICKEL_REFL_ATTR(refl) BorderColor {
    FloatTransparentBlack,
    IntTransparentBlack,
    FloatOpaqueBlack,
    IntOpaqueBlack,
    FloatOpaqueWhite,
    IntOpaqueWhite,
};

enum class NICKEL_REFL_ATTR(refl) SamplerMipmapMode {
    Nearest,
    Linear,
};

enum class NICKEL_REFL_ATTR(refl) BindGroupEntryType {
    Sampler,
    CombinedImageSampler,
    SampledImage,
    StorageImage,
    UniformTexelBuffer,
    StorageTexelBuffer,
    UniformBuffer,
    StorageBuffer,
    UniformBufferDynamic,
    StoragesBufferDynamic,
    InputAttachment,
    InlineUniformBlock,
};

enum class NICKEL_REFL_ATTR(refl) PipelineStage {
    None = 0,
    TopOfPipe = 0x00000001,
    DrawIndirect = 0x00000002,
    VertexInput = 0x00000004,
    VertexShader = 0x00000008,
    TessellationControlShader = 0x00000010,
    TessellationEvaluationShader = 0x00000020,
    GeometryShader = 0x00000040,
    FragmentShader = 0x00000080,
    EarlyFragmentTests = 0x00000100,
    LateFragmentTests = 0x00000200,
    ColorAttachmentOutput = 0x00000400,
    ComputeShader = 0x00000800,
    Transfer = 0x00001000,
    BottomOfPipe = 0x00002000,
    Host = 0x00004000,
    AllGraphics = 0x00008000,
    AllCommands = 0x00010000,
};

enum class NICKEL_REFL_ATTR(refl) Access {
    IndirectCommandRead = 0x00000001,
    IndexRead = 0x00000002,
    VertexAttributeRead = 0x00000004,
    UniformRead = 0x00000008,
    InputAttachmentRead = 0x00000010,
    ShaderRead = 0x00000020,
    ShaderWrite = 0x00000040,
    ColorAttachmentRead = 0x00000080,
    ColorAttachmentWrite = 0x00000100,
    DepthStencilAttachmentRead = 0x00000200,
    DepthStencilAttachmentWrite = 0x00000400,
    TransferRead = 0x00000800,
    TransferWrite = 0x00001000,
    HostRead = 0x00002000,
    HostWrite = 0x00004000,
    MemoryRead = 0x00008000,
    MemoryWrite = 0x00010000,
    None = 0,
};

enum class NICKEL_REFL_ATTR(refl) Dependency {
    ByRegionBit = 0x00000001,
    DeviceGroupBit = 0x00000004,
    ViewLocalBit = 0x00000002,
};

enum class NICKEL_REFL_ATTR(refl) ImageLayout {
    Undefined = 0,
    General,
    PresentSrcKHR,
    ColorAttachmentOptimal,
    DepthStencilAttachmentOptimal,
    DepthStencilReadOnlyOptimal,
    ShaderReadOnlyOptimal,
    TransferSrcOptimal,
    TransferDstOptimal,
    Preinitialized,
    DepthReadOnlyStencilAttachmentOptimal,
    DepthAttachmentStencilReadOnlyOptimal,
    DepthAttachmentOptimal,
    DepthReadOnlyOptimal,
    StencilAttachmentOptimal,
    StencilReadOnlyOptimal,
    ReadOnly_optimal,
    AttachmentOptimal,
};

enum class NICKEL_REFL_ATTR(refl) PipelineBindPoint { Graphics = 0, Compute };

enum class NICKEL_REFL_ATTR(refl) ImageTiling { Optimal = 0, Linear };

enum class NICKEL_REFL_ATTR(refl) SharingMode { Exclusive = 0, Concurrent };

enum class NICKEL_REFL_ATTR(refl) ComponentMapping {
    SwizzleIdentity = 0,
    SwizzleZero,
    SwizzleOne,
    SwizzleR,
    SwizzleG,
    SwizzleB,
    SwizzleA,
};

enum class NICKEL_REFL_ATTR(refl) ColorComponent {
    R = 0x00000001,
    G = 0x00000002,
    B = 0x00000004,
    A = 0x00000008,
    All = R | G | B | A,
};

enum class NICKEL_REFL_ATTR(refl) ImageColorSpace {
    SrgbNonlinearKHR = 0,
    DisplayP3NonlinearEXT,
    ExtendedSrgbLinearEXT,
    DisplayP3LinearEXT,
    DciP3NonlinearEXT,
    Bt709LinearEXT,
    Bt709NonlinearEXT,
    Bt2020LinearEXT,
    Hdr10St2084EXT,
    DolbyvisionEXT,
    Hdr10HlgEXT,
    AdobergbLinearEXT,
    AdobergbNonlinearEXT,
    PassThroughEXT,
    ExtendedSrgbNonlinearEXT,
    DisplayNativeAMD,
};

enum class NICKEL_REFL_ATTR(refl) SubpassContent {
    Inline,
    SecondaryCommandBuffer,
};

}  // namespace nickel::graphics