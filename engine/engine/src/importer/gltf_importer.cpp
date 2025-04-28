#include "nickel/importer/gltf_importer.hpp"

#include "nickel/common/common.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/importer/internal/gltf_importer_impl.hpp"
#include "tiny_gltf.h"

namespace nickel {

GLTFImportData ImportGLTF(const Path& filename) {
    auto content = ReadWholeFile(filename);
    NICKEL_RETURN_VALUE_IF_FALSE_LOGW({}, !content.empty(), "read {} failed",
                                      filename);

    tinygltf::TinyGLTF tiny_gltf_loader;
    std::string err, warn;
    tinygltf::Model gltf_model;
    if (!tiny_gltf_loader.LoadASCIIFromString(
            &gltf_model, &err, &warn, content.data(), content.size(),
            filename.ParentPath().ToString())) {
        LOGE("load model from {} failed: \n\terr: {}\n\twarn: {}", filename,
             err, warn);
        return {};
    }

    if (gltf_model.nodes.empty()) {
        LOGE("load model from {} failed: no nodes", filename);
        return {};
    }

    GLTFLoader loader(gltf_model);
    return loader.Load(filename, Context::GetInst().GetGPUAdapter(),
                       *Context::GetInst().GetGLTFManager().GetImpl());
}

bool ImportGLTF2Engine(const Path& filename, const GLTFLoadConfig& config) {
    auto& ctx = Context::GetInst();
    GLTFImportData load_data = ImportGLTF(filename);
    bool success = true;
    success = success && ctx.GetGLTFManager().Load(load_data, config);
    success = success && ctx.GetSkeletonManager().Load(load_data, config);
    return success;
}

}  // namespace nickel