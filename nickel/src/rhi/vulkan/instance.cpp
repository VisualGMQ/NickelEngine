#include "rhi/vulkan/instance.hpp"
#include "rhi/vulkan/util.hpp"

namespace nickel::rhi::vulkan {

Instance::Instance(Window& window) : window_(&window) {
    vk::InstanceCreateInfo createInfo;
    vk::ApplicationInfo appInfo;
    appInfo.setApiVersion(VK_API_VERSION_1_3);
    appInfo.setPEngineName("NickelEngine");
    createInfo.setPApplicationInfo(&appInfo);

    unsigned int count;
    SDL_Window* sdlWindow = (SDL_Window*)(window.Raw());
    SDL_Vulkan_GetInstanceExtensions(sdlWindow, &count, nullptr);
    std::vector<const char*> extensions(count);
    SDL_Vulkan_GetInstanceExtensions(sdlWindow, &count, extensions.data());

    for (auto& ext : extensions) {
        LOGW(nickel::log_tag::Vulkan, ext);
    }
    createInfo.setPEnabledExtensionNames(extensions);

    auto supportLayers = vk::enumerateInstanceLayerProperties();
    std::vector<const char*> requireLayers = {"VK_LAYER_KHRONOS_validation"};
    using LiteralString = const char*;
    RemoveUnexistsElems<const char*, vk::LayerProperties>(
        requireLayers, supportLayers,
        [](const LiteralString& require, const vk::LayerProperties& prop) {
            return std::strcmp(prop.layerName.data(), require) == 0;
        });

    createInfo.setPEnabledLayerNames(requireLayers);
    instance_ = vk::createInstance(createInfo);
}

}  // namespace nickel::rhi::vulkan