find_package(Vulkan QUIET)

if (Vulkan_FOUND)
    message("found vulkan:")
    message("\tinclude dir: " ${Vulkan_INCLUDE_DIRS})
    message("\tlibraries: " ${Vulkan_LIBRARIES})
    add_library(Vulkan INTERFACE IMPORTED GLOBAL)
    target_link_libraries(Vulkan INTERFACE Vulkan::Vulkan)
endif()