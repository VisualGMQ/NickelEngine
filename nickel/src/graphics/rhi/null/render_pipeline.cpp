#include "graphics/rhi/null/render_pipeline.hpp"
#include "graphics/rhi/pipeline_layout.hpp"
#include "graphics/rhi/null/device.hpp"

namespace nickel::rhi::null {

PipelineLayout RenderPipelineImpl::GetLayout() const {
    DeviceImpl impl;
    return PipelineLayout{APIPreference::Null, impl, {}};
}

}