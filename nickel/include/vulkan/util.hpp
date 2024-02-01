#pragma once

#include "stdpch.hpp"
#include "vulkan/pch.hpp"

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
        case vk::Result::eSuccess: return "Success";
        case vk::Result::eNotReady: return "NotReady";
        case vk::Result::eTimeout: return "Timeout";
        case vk::Result::eEventSet: return "EventSet";
        case vk::Result::eEventReset: return "EventReset";
        case vk::Result::eIncomplete: return "Incomplete";
        case vk::Result::eErrorOutOfHostMemory: return "ErrorOutOfHostMemory";
        case vk::Result::eErrorOutOfDeviceMemory: return "ErrorOutOfDeviceMemory";
        case vk::Result::eErrorInitializationFailed: return "ErrorInitializationFailed";
        case vk::Result::eErrorDeviceLost: return "ErrorDeviceLost";
        case vk::Result::eErrorMemoryMapFailed: return "ErrorMemoryMapFailed";
        case vk::Result::eErrorLayerNotPresent: return "ErrorLayerNotPresent";
        case vk::Result::eErrorExtensionNotPresent: return "ErrorExtensionNotPresent";
        case vk::Result::eErrorFeatureNotPresent: return "ErrorFeatureNotPresent";
        case vk::Result::eErrorIncompatibleDriver: return "ErrorIncompatibleDriver";
        case vk::Result::eErrorTooManyObjects: return "ErrorTooManyObjects";
        case vk::Result::eErrorFormatNotSupported: return "ErrorFormatNotSupported";
        case vk::Result::eErrorFragmentedPool: return "ErrorFragmentedPool";
        case vk::Result::eErrorUnknown: return "ErrorUnknown";
        case vk::Result::eErrorOutOfPoolMemory: return "ErrorOutOfPoolMemory";
        case vk::Result::eErrorInvalidExternalHandle: return "ErrorInvalidExternalHandle";
        case vk::Result::eErrorFragmentation: return "ErrorFragmentation";
        case vk::Result::eErrorInvalidOpaqueCaptureAddress: return "ErrorInvalidOpaqueCaptureAddress";
        case vk::Result::ePipelineCompileRequired: return "PipelineCompileRequired";
        case vk::Result::eErrorSurfaceLostKHR: return "ErrorSurfaceLostKHR";
        case vk::Result::eErrorNativeWindowInUseKHR: return "ErrorNativeWindowInUseKHR";
        case vk::Result::eSuboptimalKHR: return "SuboptimalKHR";
        case vk::Result::eErrorOutOfDateKHR: return "ErrorOutOfDateKHR";
        case vk::Result::eErrorIncompatibleDisplayKHR: return "ErrorIncompatibleDisplayKHR";
        case vk::Result::eErrorValidationFailedEXT: return "ErrorValidationFailedEXT";
        case vk::Result::eErrorInvalidShaderNV: return "ErrorInvalidShaderNV";
        case vk::Result::eErrorImageUsageNotSupportedKHR: return "ErrorImageUsageNotSupportedKHR";
        case vk::Result::eErrorVideoPictureLayoutNotSupportedKHR: return "ErrorVideoPictureLayoutNotSupportedKHR";
        case  vk::Result::eErrorVideoProfileOperationNotSupportedKHR: return "ErrorVideoProfileOperationNotSupportedKHR";
        case vk::Result::eErrorVideoProfileFormatNotSupportedKHR: return "ErrorVideoProfileFormatNotSupportedKHR";
        case vk::Result::eErrorVideoProfileCodecNotSupportedKHR: return "ErrorVideoProfileCodecNotSupportedKHR";
        case vk::Result::eErrorVideoStdVersionNotSupportedKHR: return "ErrorVideoStdVersionNotSupportedKHR";
        case vk::Result::eErrorIncompatibleShaderBinaryEXT: return "ErrorIncompatibleShaderBinaryEXT";
        case  vk::Result::eErrorInvalidDrmFormatModifierPlaneLayoutEXT: return "ErrorInvalidDrmFormatModifierPlaneLayoutEXT";
        case vk::Result::eErrorNotPermittedEXT: return "ErrorNotPermittedEXT";
        case vk::Result::eThreadIdleKHR: return "ThreadIdleKHR";
        case vk::Result::eThreadDoneKHR: return "ThreadDoneKHR";
        case vk::Result::eOperationDeferredKHR: return "OperationDeferredKHR";
        case vk::Result::eOperationNotDeferredKHR: return "OperationNotDeferredKHR";
        case vk::Result::eErrorCompressionExhaustedEXT: return "ErrorCompressionExhaustedEXT";
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