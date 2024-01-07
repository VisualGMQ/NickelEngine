#pragma once

#include <cstdint>

namespace nickel::rhi {

enum class DescriptorType {
    Sampler,
    CombinedImageSampler,
    SampledImage,
    StorageImage,
    UniformTexelBuffer,
    StorageTexelBuffer,
    UniformBuffer,
    StorageBuffer,
    UniformBufferDynamic,
    StorageBufferDynamic,
    InputAttachment,
    InlineUniformBlock,
    AccelerationStructureKHR,
    // AccelerationStructureNV
    SampleWeightImageQCOM,
    BlockMatchImageQCOM,
    // MutableEXT
    // InlineUniformBlockEXT
    MutableVALVE,
};

using ShaderStageFlags = uint32_t;

enum class ShaderStageFlagBits : ShaderStageFlags {
    All,
    Vertex,
    TessellationControl,
    TessellationEvaluation,
    Geometry,
    Fragment,
    Compute,
    AllGraphics,
    // RaygenKHR,
    // AnyHitKHR,
    // ClosestHitKHR,
    // MissKHR,
    // IntersectionKHR,
    // CallableKHR,
    // TaskEXT,
    // MeshEXT,
    // SubpassShadingHUAWEI,
    // AnyHitNV             ,
    // CallableNV           ,
    // ClosestHitNV         ,
    // IntersectionNV       ,
    // MeshNV               ,
    // MissNV               ,
    // RaygenNV             ,
    // TaskNV               ,
};

enum class FrontFace {
    CW,
    CCW,
};

enum class PolygonMode {
    Point,
    Line,
    Fill,
};

enum class CullMode {
    None,
    Back,
    Front,
    Both,
};

enum class SampleCountFlag {
    e1 = 1,
    e2 = 2,
    e4 = 4,
    e8 = 8,
    e16 = 16,
    e32 = 32,
    e64 = 64
};

enum class CompareOp {
    Never,
    Less,
    LessEqual,
    Equal,
    GreaterEqual,
    Greater,
    Always,
};

enum class StencilOpEnum {
    Keep,
    Zero,
    Replace,
    Increament,
    IncreamentWrap,
    Decreament,
    DecreamentWrap,
    Invert,
};

enum class LogicOp {
    Clear,
    And,
    AndReverse,
    AndInverted,
    Copy,
    NoOp,
    Xor,
    Or,
    Nor,
    Equivalent,
    Invert,
    OrReverse,
    CopyInverted,
    OrInverted,
    Nand,
    Set
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
    Src1Color,
    OneMinusSrc1Color,
    Src1Alpha,
    OneMinusSrc1Alpha,
};

enum class BlendOp {
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max,
};

using ColorComponentFlags = uint32_t;

enum class ColorComponentFlagBits: ColorComponentFlags {
    R = 0x01,
    G = 0x02,
    B = 0x04,
    A = 0x08,
};

enum class PrimitiveType {
    Points,
    LineStrip,
    Lines,
    LineStripAdjacency,
    LinesAdjacency,
    TriangleStrip,
    TriangleFan,
    Triangles,
    TriangleStripAdjacency,
    TrianglesAdjacency,
    Patches,
};

enum class ImageLayout {
    Undefined,
    General,
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
    ReadOnlyOptimal,
    AttachmentOptimal,
    PresentSrcKHR,
};

enum class Format {
    Undefined,
    R4G4UnormPack8,
    R4G4B4A4UnormPack16,
    B4G4R4A4UnormPack16,
    R5G6B5UnormPack16,
    B5G6R5UnormPack16,
    R5G5B5A1UnormPack16,
    B5G5R5A1UnormPack16,
    A1R5G5B5UnormPack16,
    R8Unorm,
    R8Snorm,
    R8Uscaled,
    R8Sscaled,
    R8Uint,
    R8Sint,
    R8Srgb,
    R8G8Unorm,
    R8G8Snorm,
    R8G8Uscaled,
    R8G8Sscaled,
    R8G8Uint,
    R8G8Sint,
    R8G8Srgb,
    R8G8B8Unorm,
    R8G8B8Snorm,
    R8G8B8Uscaled,
    R8G8B8Sscaled,
    R8G8B8Uint,
    R8G8B8Sint,
    R8G8B8Srgb,
    B8G8R8Unorm,
    B8G8R8Snorm,
    B8G8R8Uscaled,
    B8G8R8Sscaled,
    B8G8R8Uint,
    B8G8R8Sint,
    B8G8R8Srgb,
    R8G8B8A8Unorm,
    R8G8B8A8Snorm,
    R8G8B8A8Uscaled,
    R8G8B8A8Sscaled,
    R8G8B8A8Uint,
    R8G8B8A8Sint,
    R8G8B8A8Srgb,
    B8G8R8A8Unorm,
    B8G8R8A8Snorm,
    B8G8R8A8Uscaled,
    B8G8R8A8Sscaled,
    B8G8R8A8Uint,
    B8G8R8A8Sint,
    B8G8R8A8Srgb,
    A8B8G8R8UnormPack32,
    A8B8G8R8SnormPack32,
    A8B8G8R8UscaledPack32,
    A8B8G8R8SscaledPack32,
    A8B8G8R8UintPack32,
    A8B8G8R8SintPack32,
    A8B8G8R8SrgbPack32,
    A2R10G10B10UnormPack32,
    A2R10G10B10SnormPack32,
    A2R10G10B10UscaledPack32,
    A2R10G10B10SscaledPack32,
    A2R10G10B10UintPack32,
    A2R10G10B10SintPack32,
    A2B10G10R10UnormPack32,
    A2B10G10R10SnormPack32,
    A2B10G10R10UscaledPack32,
    A2B10G10R10SscaledPack32,
    A2B10G10R10UintPack32,
    A2B10G10R10SintPack32,
    R16Unorm,
    R16Snorm,
    R16Uscaled,
    R16Sscaled,
    R16Uint,
    R16Sint,
    R16Sfloat,
    R16G16Unorm,
    R16G16Snorm,
    R16G16Uscaled,
    R16G16Sscaled,
    R16G16Uint,
    R16G16Sint,
    R16G16Sfloat,
    R16G16B16Unorm,
    R16G16B16Snorm,
    R16G16B16Uscaled,
    R16G16B16Sscaled,
    R16G16B16Uint,
    R16G16B16Sint,
    R16G16B16Sfloat,
    R16G16B16A16Unorm,
    R16G16B16A16Snorm,
    R16G16B16A16Uscaled,
    R16G16B16A16Sscaled,
    R16G16B16A16Uint,
    R16G16B16A16Sint,
    R16G16B16A16Sfloat,
    R32Uint,
    R32Sint,
    R32Sfloat,
    R32G32Uint,
    R32G32Sint,
    R32G32Sfloat,
    R32G32B32Uint,
    R32G32B32Sint,
    R32G32B32Sfloat,
    R32G32B32A32Uint,
    R32G32B32A32Sint,
    R32G32B32A32Sfloat,
    R64Uint,
    R64Sint,
    R64Sfloat,
    R64G64Uint,
    R64G64Sint,
    R64G64Sfloat,
    R64G64B64Uint,
    R64G64B64Sint,
    R64G64B64Sfloat,
    R64G64B64A64Uint,
    R64G64B64A64Sint,
    R64G64B64A64Sfloat,
    B10G11R11UfloatPack32,
    E5B9G9R9UfloatPack32,
    D16Unorm,
    X8D24UnormPack32,
    D32Sfloat,
    S8Uint,
    D16UnormS8Uint,
    D24UnormS8Uint,
    D32SfloatS8Uint,
    Bc1RgbUnormBlock,
    Bc1RgbSrgbBlock,
    Bc1RgbaUnormBlock,
    Bc1RgbaSrgbBlock,
    Bc2UnormBlock,
    Bc2SrgbBlock,
    Bc3UnormBlock,
    Bc3SrgbBlock,
    Bc4UnormBlock,
    Bc4SnormBlock,
    Bc5UnormBlock,
    Bc5SnormBlock,
    Bc6HUfloatBlock,
    Bc6HSfloatBlock,
    Bc7UnormBlock,
    Bc7SrgbBlock,
    Etc2R8G8B8UnormBlock,
    Etc2R8G8B8SrgbBlock,
    Etc2R8G8B8A1UnormBlock,
    Etc2R8G8B8A1SrgbBlock,
    Etc2R8G8B8A8UnormBlock,
    Etc2R8G8B8A8SrgbBlock,
    EacR11UnormBlock,
    EacR11SnormBlock,
    EacR11G11UnormBlock,
    EacR11G11SnormBlock,
    Astc4x4UnormBlock,
    Astc4x4SrgbBlock,
    Astc5x4UnormBlock,
    Astc5x4SrgbBlock,
    Astc5x5UnormBlock,
    Astc5x5SrgbBlock,
    Astc6x5UnormBlock,
    Astc6x5SrgbBlock,
    Astc6x6UnormBlock,
    Astc6x6SrgbBlock,
    Astc8x5UnormBlock,
    Astc8x5SrgbBlock,
    Astc8x6UnormBlock,
    Astc8x6SrgbBlock,
    Astc8x8UnormBlock,
    Astc8x8SrgbBlock,
    Astc10x5UnormBlock,
    Astc10x5SrgbBlock,
    Astc10x6UnormBlock,
    Astc10x6SrgbBlock,
    Astc10x8UnormBlock,
    Astc10x8SrgbBlock,
    Astc10x10UnormBlock,
    Astc10x10SrgbBlock,
    Astc12x10UnormBlock,
    Astc12x10SrgbBlock,
    Astc12x12UnormBlock,
    Astc12x12SrgbBlock,
    G8B8G8R8422Unorm,
    B8G8R8G8422Unorm,
    G8B8R83Plane420Unorm,
    G8B8R82Plane420Unorm,
    G8B8R83Plane422Unorm,
    G8B8R82Plane422Unorm,
    G8B8R83Plane444Unorm,
    R10X6UnormPack16,
    R10X6G10X6Unorm2Pack16,
    R10X6G10X6B10X6A10X6Unorm4Pack16,
    G10X6B10X6G10X6R10X6422Unorm4Pack16,
    B10X6G10X6R10X6G10X6422Unorm4Pack16,
    G10X6B10X6R10X63Plane420Unorm3Pack16,
    G10X6B10X6R10X62Plane420Unorm3Pack16,
    G10X6B10X6R10X63Plane422Unorm3Pack16,
    G10X6B10X6R10X62Plane422Unorm3Pack16,
    G10X6B10X6R10X63Plane444Unorm3Pack16,
    R12X4UnormPack16,
    R12X4G12X4Unorm2Pack16,
    R12X4G12X4B12X4A12X4Unorm4Pack16,
    G12X4B12X4G12X4R12X4422Unorm4Pack16,
    B12X4G12X4R12X4G12X4422Unorm4Pack16,
    G12X4B12X4R12X43Plane420Unorm3Pack16,
    G12X4B12X4R12X42Plane420Unorm3Pack16,
    G12X4B12X4R12X43Plane422Unorm3Pack16,
    G12X4B12X4R12X42Plane422Unorm3Pack16,
    G12X4B12X4R12X43Plane444Unorm3Pack16,
    G16B16G16R16422Unorm,
    B16G16R16G16422Unorm,
    G16B16R163Plane420Unorm,
    G16B16R162Plane420Unorm,
    G16B16R163Plane422Unorm,
    G16B16R162Plane422Unorm,
    G16B16R163Plane444Unorm,
    G8B8R82Plane444Unorm,
    G10X6B10X6R10X62Plane444Unorm3Pack16,
    G12X4B12X4R12X42Plane444Unorm3Pack16,
    G16B16R162Plane444Unorm,
    A4R4G4B4UnormPack16,
    A4B4G4R4UnormPack16,
    Astc4x4SfloatBlock,
    Astc5x4SfloatBlock,
    Astc5x5SfloatBlock,
    Astc6x5SfloatBlock,
    Astc6x6SfloatBlock,
    Astc8x5SfloatBlock,
    Astc8x6SfloatBlock,
    Astc8x8SfloatBlock,
    Astc10x5SfloatBlock,
    Astc10x6SfloatBlock,
    Astc10x8SfloatBlock,
    Astc10x10SfloatBlock,
    Astc12x10SfloatBlock,
    Astc12x12SfloatBlock,
    Pvrtc12BppUnormBlockIMG,
    Pvrtc14BppUnormBlockIMG,
    Pvrtc22BppUnormBlockIMG,
    Pvrtc24BppUnormBlockIMG,
    Pvrtc12BppSrgbBlockIMG,
    Pvrtc14BppSrgbBlockIMG,
    Pvrtc22BppSrgbBlockIMG,
    Pvrtc24BppSrgbBlockIMG,
    R16G16S105NV,
    A4B4G4R4UnormPack16EXT,
    A4R4G4B4UnormPack16EXT,
    Astc10x10SfloatBlockEXT,
    Astc10x5SfloatBlockEXT,
    Astc10x6SfloatBlockEXT,
    Astc10x8SfloatBlockEXT,
    Astc12x10SfloatBlockEXT,
    Astc12x12SfloatBlockEXT,
    Astc4x4SfloatBlockEXT,
    Astc5x4SfloatBlockEXT,
    Astc5x5SfloatBlockEXT,
    Astc6x5SfloatBlockEXT,
    Astc6x6SfloatBlockEXT,
    Astc8x5SfloatBlockEXT,
    Astc8x6SfloatBlockEXT,
    Astc8x8SfloatBlockEXT,
    B10X6G10X6R10X6G10X6422Unorm4Pack16KHR,
    B12X4G12X4R12X4G12X4422Unorm4Pack16KHR,
    B16G16R16G16422UnormKHR,
    B8G8R8G8422UnormKHR,
    G10X6B10X6G10X6R10X6422Unorm4Pack16KHR,
    G10X6B10X6R10X62Plane420Unorm3Pack16KHR,
    G10X6B10X6R10X62Plane422Unorm3Pack16KHR,
    G10X6B10X6R10X62Plane444Unorm3Pack16EXT,
    G10X6B10X6R10X63Plane420Unorm3Pack16KHR,
    G10X6B10X6R10X63Plane422Unorm3Pack16KHR,
    G10X6B10X6R10X63Plane444Unorm3Pack16KHR,
    G12X4B12X4G12X4R12X4422Unorm4Pack16KHR,
    G12X4B12X4R12X42Plane420Unorm3Pack16KHR,
    G12X4B12X4R12X42Plane422Unorm3Pack16KHR,
    G12X4B12X4R12X42Plane444Unorm3Pack16EXT,
    G12X4B12X4R12X43Plane420Unorm3Pack16KHR,
    G12X4B12X4R12X43Plane422Unorm3Pack16KHR,
    G12X4B12X4R12X43Plane444Unorm3Pack16KHR,
    G16B16G16R16422UnormKHR,
    G16B16R162Plane420UnormKHR,
    G16B16R162Plane422UnormKHR,
    G16B16R162Plane444UnormEXT,
    G16B16R163Plane420UnormKHR,
    G16B16R163Plane422UnormKHR,
    G16B16R163Plane444UnormKHR,
    G8B8G8R8422UnormKHR,
    G8B8R82Plane420UnormKHR,
    G8B8R82Plane422UnormKHR,
    G8B8R82Plane444UnormEXT,
    G8B8R83Plane420UnormKHR,
    G8B8R83Plane422UnormKHR,
    G8B8R83Plane444UnormKHR,
    R10X6G10X6B10X6A10X6Unorm4Pack16KHR,
    R10X6G10X6Unorm2Pack16KHR,
    R10X6UnormPack16KHR,
    R12X4G12X4B12X4A12X4Unorm4Pack16KHR,
    R12X4G12X4Unorm2Pack16KHR,
    R12X4UnormPack16KHR,
};

enum class PipelineBindPoint {
    Graphics,
    Compute,
    // eRayTracingKHR
    // eSubpassShadingHUAWEI
    // eRayTracingNV
};

using PipelineStageFlags = uint32_t;

enum class PipelineStageFlagBits : PipelineStageFlags {
    TopOfPipe,
    DrawIndirect,
    VertexInput,
    VertexShader,
    TessellationControlShader,
    TessellationEvaluationShader,
    GeometryShader,
    FragmentShader,
    EarlyFragmentTests,
    LateFragmentTests,
    ColorAttachmentOutput,
    ComputeShader,
    Transfer,
    BottomOfPipe,
    Host,
    AllGraphics,
    AllCommands,
    None,

    /*
    TransformFeedbackEXT,
    ConditionalRenderingEXT,
    RayTracingShaderKHR,
    AccelerationStructureBuildKHR,
    FragmentDensityProcessEXT,
    FragmentShadingRateAttachmentKHR,
    CommandPreprocessNV,
    TaskShaderEXT,
    MeshShaderEXT,
    AccelerationStructureBuildNV,
    MeshShaderNV,
    NoneKHR,
    RayTracingShaderNV,
    ShadingRateImageNV,
    TaskShaderNV,
    */
};

using AccessFlags = uint32_t;

enum class AccessFlagBits : AccessFlags {
    IndirectCommandRead,
    IndexRead,
    VertexAttributeRead,
    UniformRead,
    InputAttachmentRead,
    ShaderRead,
    ShaderWrite,
    ColorAttachmentRead,
    ColorAttachmentWrite,
    DepthStencilAttachmentRead,
    DepthStencilAttachmentWrite,
    TransferRead,
    TransferWrite,
    HostRead,
    HostWrite,
    MemoryRead,
    MemoryWrite,
    None,
    /*
    TransformFeedbackWriteEXT,
    TransformFeedbackCounterReadEXT,
    TransformFeedbackCounterWriteEXT,
    ConditionalRenderingReadEXT,
    ColorAttachmentReadNoncoherentEXT,
    AccelerationStructureReadKHR,
    AccelerationStructureWriteKHR,
    FragmentDensityMapReadEXT,
    FragmentShadingRateAttachmentReadKHR,
    CommandPreprocessReadNV,
    CommandPreprocessWriteNV,
    AccelerationStructureReadNV,
    AccelerationStructureWriteNV,
    NoneKHR,
    ShadingRateImageReadNV,
    */
};

enum class AttachmentLoadOp {
    Load,
    Clear,
    DontCare,
    // NoneEXT
};

enum class AttachmentStoreOp {
    Store,
    DontCare,
    None,
    /*
    NoneEXT
    NoneKHR
    NoneQCOM
    */
};

using BufferUsageFlags = uint32_t;

enum class BufferUsageFlagBits : BufferUsageFlags {
    TransferSrc,
    TransferDst,
    UniformTexelBuffer,
    StorageTexelBuffer,
    UniformBuffer,
    StorageBuffer,
    IndexBuffer,
    VertexBuffer,
    IndirectBuffer,
    ShaderDeviceAddress,
};

enum class SharingMode {
    Exclusive,
    Concurrent,
};

using MemoryPropertyFlags = uint32_t;

enum class MemoryPropertyFlagBits : MemoryPropertyFlags {
    DeviceLocal,
    HostVisible,
    HostCoherent,
    HostCached,
    LazilyAllocated,
    Protected,
    /*
    DeviceCoherentAMD
    DeviceUncachedAMD
    RdmaCapableNV
    */
};

enum class ImageViewType {
    e1D,
    e2D,
    e3D,
    Cube,
    e1DArray,
    e2DArray,
    CubeArray,
};

enum class ComponentSwizzle {
    Identity,
    Zero,
    One,
    R,
    G,
    B,
    A,
};

using ImageAspectFlags = uint32_t;

enum class ImageAspectFlagBits {
    Color,
    Depth,
    Stencil,
    Metadata,
    Plane0,
    Plane1,
    Plane2,
    None,
    /*
    MemoryPlane0EXT
    MemoryPlane1EXT
    MemoryPlane2EXT
    MemoryPlane3EXT
    NoneKHR
    Plane0KHR
    Plane1KHR
    Plane2KHR
    */
};

enum class ImageType {
    e1D,
    e2D,
    e3D,
};

using ImageUsageFlags = uint32_t;

enum class ImageUsageFlagBits : ImageUsageFlags {
    TransferSrc,
    TransferDst,
    Sampled,
    Storage,
    ColorAttachment,
    DepthStencilAttachment,
    TransientAttachment,
    InputAttachment,
};

enum class ImageTiling {
    Optimal,
    Linear,
};

enum class CommandPoolCreateFlag {
    Transient,
    ResetCommandBuffer,
    Protected,
};

enum class CommandBufferLevel {
    Primary,
    Secondary,
};

enum class CommandBufferUsage {
    OneTimeSubmit,
    RenderPassContinue,
    SimultaneousUse,
};

}  // namespace nickel::rhi