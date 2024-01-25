#pragma once

#include "pch.hpp"

namespace nickel::vulkan {

#define VK_CALL(return_value, expr)                                           \
    do {                                                                      \
        if (auto result = (expr); result.result != vk::Result::eSuccess) {    \
            LOGE(nickel::log_tag::Vulkan, "call ", #expr,                     \
                 " failed: ", nickel::vulkan::VkError2String(result.result)); \
        } else {                                                              \
            return_value = result.value;                                      \
        }                                                                     \
    } while (0)

#define VK_CALL_NO_VALUE(expr)                                                \
    do {                                                                      \
        if (auto result = (expr); result != vk::Result::eSuccess) {           \
            LOGE(nickel::log_tag::Vulkan, "call ", #expr,                     \
                 " failed: ", nickel::vulkan::VkError2String(result)); \
        }                                                                     \
    } while (0)

#define RETURN_ENUM_NAME(e) \
    case e:                 \
        return nameof::nameof_enum(e);

inline std::string_view VkError2String(vk::Result err) {
    switch (err) {
        RETURN_ENUM_NAME(vk::Result::eSuccess);
        RETURN_ENUM_NAME(vk::Result::eNotReady);
        RETURN_ENUM_NAME(vk::Result::eTimeout);
        RETURN_ENUM_NAME(vk::Result::eEventSet);
        RETURN_ENUM_NAME(vk::Result::eEventReset);
        RETURN_ENUM_NAME(vk::Result::eIncomplete);
        RETURN_ENUM_NAME(vk::Result::eErrorOutOfHostMemory);
        RETURN_ENUM_NAME(vk::Result::eErrorOutOfDeviceMemory);
        RETURN_ENUM_NAME(vk::Result::eErrorInitializationFailed);
        RETURN_ENUM_NAME(vk::Result::eErrorDeviceLost);
        RETURN_ENUM_NAME(vk::Result::eErrorMemoryMapFailed);
        RETURN_ENUM_NAME(vk::Result::eErrorLayerNotPresent);
        RETURN_ENUM_NAME(vk::Result::eErrorExtensionNotPresent);
        RETURN_ENUM_NAME(vk::Result::eErrorFeatureNotPresent);
        RETURN_ENUM_NAME(vk::Result::eErrorIncompatibleDriver);
        RETURN_ENUM_NAME(vk::Result::eErrorTooManyObjects);
        RETURN_ENUM_NAME(vk::Result::eErrorFormatNotSupported);
        RETURN_ENUM_NAME(vk::Result::eErrorFragmentedPool);
        RETURN_ENUM_NAME(vk::Result::eErrorUnknown);
        RETURN_ENUM_NAME(vk::Result::eErrorOutOfPoolMemory);
        RETURN_ENUM_NAME(vk::Result::eErrorInvalidExternalHandle);
        RETURN_ENUM_NAME(vk::Result::eErrorFragmentation);
        RETURN_ENUM_NAME(vk::Result::eErrorInvalidOpaqueCaptureAddress);
        RETURN_ENUM_NAME(vk::Result::ePipelineCompileRequired);
        RETURN_ENUM_NAME(vk::Result::eErrorSurfaceLostKHR);
        RETURN_ENUM_NAME(vk::Result::eErrorNativeWindowInUseKHR);
        RETURN_ENUM_NAME(vk::Result::eSuboptimalKHR);
        RETURN_ENUM_NAME(vk::Result::eErrorOutOfDateKHR);
        RETURN_ENUM_NAME(vk::Result::eErrorIncompatibleDisplayKHR);
        RETURN_ENUM_NAME(vk::Result::eErrorValidationFailedEXT);
        RETURN_ENUM_NAME(vk::Result::eErrorInvalidShaderNV);
        RETURN_ENUM_NAME(vk::Result::eErrorImageUsageNotSupportedKHR);
        RETURN_ENUM_NAME(vk::Result::eErrorVideoPictureLayoutNotSupportedKHR);
        RETURN_ENUM_NAME(
            vk::Result::eErrorVideoProfileOperationNotSupportedKHR);
        RETURN_ENUM_NAME(vk::Result::eErrorVideoProfileFormatNotSupportedKHR);
        RETURN_ENUM_NAME(vk::Result::eErrorVideoProfileCodecNotSupportedKHR);
        RETURN_ENUM_NAME(vk::Result::eErrorVideoStdVersionNotSupportedKHR);
        RETURN_ENUM_NAME(vk::Result::eErrorIncompatibleShaderBinaryEXT);
        RETURN_ENUM_NAME(
            vk::Result::eErrorInvalidDrmFormatModifierPlaneLayoutEXT);
        RETURN_ENUM_NAME(vk::Result::eErrorNotPermittedEXT);
        RETURN_ENUM_NAME(vk::Result::eThreadIdleKHR);
        RETURN_ENUM_NAME(vk::Result::eThreadDoneKHR);
        RETURN_ENUM_NAME(vk::Result::eOperationDeferredKHR);
        RETURN_ENUM_NAME(vk::Result::eOperationNotDeferredKHR);
        RETURN_ENUM_NAME(vk::Result::eErrorCompressionExhaustedEXT);
    }
}

template <typename T, typename U>
void RemoveUnexistsElems(std::vector<T>& requires,
                         const std::vector<U>& supports,
                         std::function<bool(const T&, const U&)> isEqual) {
    for (int i = requires.size() - 1; i >= 0; i--) {
        bool found = false;
        auto& require =
        requires[
            i];
        for (auto& support : supports) {
            if (isEqual(require, support)) {
                found = true;
            }
        }

        if (!found) {
            requires.erase(requires.begin() + i);
        }
    }
}

}  // namespace nickel::vulkan