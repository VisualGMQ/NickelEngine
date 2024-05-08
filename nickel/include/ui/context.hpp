#pragma once

#include "graphics/camera.hpp"
#include "graphics/font.hpp"
#include "ui/event.hpp"
#include "graphics/context.hpp"

namespace nickel::ui {

struct UIVertex final {
    nickel::cgmath::Vec3 position;
    nickel::cgmath::Vec2 uv;
    nickel::cgmath::Vec4 color;
};

struct TextureClip final {
    TextureHandle handle;
    cgmath::Vec2 uv0{0, 0}, uv1{1, 1};
};

struct RenderUIContext final {
    struct BatchBreakInfo final {
        uint32_t start;
        uint32_t count{};
        rhi::RenderPipeline pipeline;
        TextureHandle texture;
    };

    rhi::RenderPipeline fillPipeline;
    rhi::RenderPipeline linePipeline;
    rhi::PipelineLayout pipelineLayout;
    rhi::BindGroupLayout bindGroupLayout;
    rhi::ShaderModule vertexShader;
    rhi::ShaderModule fragmentShader;
    rhi::Buffer vertexBuffer;
    rhi::Buffer indexBuffer;
    std::vector<BatchBreakInfo> batchBreakInfos;
    std::unordered_map<TextureHandle, rhi::BindGroup, typename TextureHandle::Hash,
                       typename TextureHandle::Eq>
        bindGroups;
    rhi::BindGroup defaultBindGroup;
    Camera camera;

    struct FontCharSet{
        uint32_t code;
        rhi::Texture texture;
        rhi::TextureView view;
    };
    using FontCharMap = std::unordered_map<uint32_t, FontCharSet>;

    std::unordered_map<FontHandle, FontCharMap, typename FontHandle::Hash,
                       typename FontHandle::Eq>
        fontChars;  // OPTIMIZE: render chars to one Texture when engine init

    std::vector<UIVertex> vertices;

    static constexpr uint32_t MaxRectSize = 4096;

    RenderUIContext(rhi::APIPreference, rhi::Device, RenderContext&,
                    const cgmath::Vec2& windowSize);
    ~RenderUIContext();

    void AddElement(const cgmath::Rect&, const cgmath::Color&,
                    const TextureClip&, rhi::RenderPipeline);

    rhi::BindGroup FindBindGroup(TextureHandle handle);
    rhi::TextureView GetFontChar(FontHandle, uint32_t code);

    void RecreatePipeline(rhi::APIPreference api);

private:
    rhi::Device device_;

    rhi::PipelineLayout createPipelineLayout();
    void initPipelineShader(rhi::APIPreference);
    rhi::BindGroupLayout createBindGroupLayout(RenderContext& ctx);
    void initPipelines(rhi::APIPreference);
    rhi::Buffer createIndexBuffer();
    rhi::Buffer createVertexBuffer();
    void initDefaultBindGroup();
};

struct UIContext final {
    RenderUIContext renderCtx;
    EventRecorder eventRecorder;

    UIContext(rhi::APIPreference api, rhi::Device device, RenderContext& ctx,
              const cgmath::Vec2& windowSize);
    UIContext(const UIContext&) = delete;
    UIContext& operator=(const UIContext&) = delete;
};


}  // namespace nickel::ui