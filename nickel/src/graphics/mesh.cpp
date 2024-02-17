#include "graphics/mesh.hpp"
#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace nickel {

Mesh parseMesh(const tinygltf::Model& gltfModel,
               const tinygltf::Mesh& gltfMesh) {
    Mesh mesh;

    for (auto& prim : gltfMesh.primitives) {
        Primitive primitive;
        auto& attrs = prim.attributes;
        if (auto it = attrs.find("POSITION"); it != attrs.end()) {
            primitive.posBufIdx = it->second;
            auto& accessor = gltfModel.accessors[it->second];
            if (accessor.type != TINYGLTF_TYPE_VEC3 ||
                accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
                TODO("convert position to vec3");
            }
        }

        if (auto it = attrs.find("NORMAL"); it != attrs.end()) {
            primitive.normBufIdx = it->second;
            auto& accessor = gltfModel.accessors[it->second];
            if (accessor.type != TINYGLTF_TYPE_VEC3 ||
                accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
                TODO("convert normal to vec3");
            }
        }

        if (auto it = attrs.find("TANGENT"); it != attrs.end()) {
            primitive.tangentIdx = it->second;
            auto& accessor = gltfModel.accessors[it->second];
            if (accessor.type != TINYGLTF_TYPE_VEC3 ||
                accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
                TODO("convert normal to vec3");
            }
        }

        if (auto it = attrs.find("TEXCOORD_0"); it != attrs.end()) {
            primitive.uvBufIdx = it->second;
            auto& accessor = gltfModel.accessors[it->second];
            if (accessor.type != TINYGLTF_TYPE_SCALAR ||
                accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
                TODO("convert texcoord to float");
            }
        }

        if (auto it = attrs.find("COLOR_0"); it != attrs.end()) {
            primitive.colorIdx = it->second;
            auto& accessor = gltfModel.accessors[it->second];
            if (accessor.type != TINYGLTF_TYPE_VEC4 ||
                accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
                TODO("convert color to float");
            }
        }

        if (prim.material != -1) {
            primitive.materialIdx = prim.material;
        }

        if (prim.indices != -1) {
            primitive.indicesIdx = prim.indices;
        }

        mesh.primitives.emplace_back(std::move(primitive));
    }

    return mesh;
}

TextureHandle parseImage(const Model& model,
                         const std::filesystem::path& rootDir,
                         const tinygltf::Image& image,
                         TextureManager& textureMgr) {
    if (!image.uri.empty()) {
        auto path = rootDir / image.uri;
        TextureHandle handle;
        if (!textureMgr.Has(path)) {
            return textureMgr.Load(rootDir / image.uri);
        } else {
            return textureMgr.GetHandle(path);
        }
    } else {
        auto& view = model.bufferViews[image.bufferView];
        auto& buffer = model.buffers[view.bufIdx];
        int w, h;
        void* data =
            stbi_load_from_memory(buffer.data() + view.offset, view.size, &w,
                                  &h, nullptr, STBI_rgb_alpha);
        if (!data) {
            LOGW(log_tag::Asset, "load image from gltf failed");
            return {};
        }

        return textureMgr.Create("", data, w, h);
    }
}

Model LoadFromFile(const std::filesystem::path& filepath,
                   TextureManager& textureMgr) {
    tinygltf::TinyGLTF loader;
    tinygltf::Model gltfModel;
    std::string err, warn;
    if (!loader.LoadASCIIFromFile(&gltfModel, &err, &warn,
                                  filepath.filename().string())) {
        LOGW(log_tag::Asset, "load model from ", filepath,
             " failed:\n\terr:", err, "\n\twarn:", warn);
        return {};
    }

    auto rootDir = filepath.parent_path();

    Model model;
    for (auto& buffer : gltfModel.buffers) {
        model.buffers.emplace_back(buffer.data);
    }

    for (auto& view : gltfModel.bufferViews) {
        BufferView v;
        v.bufIdx = view.buffer;
        v.offset = view.byteOffset;
        v.size = view.byteLength;
        model.bufferViews.emplace_back(v);
    }

    for (auto& accessor : gltfModel.accessors) {
        BufferAccessor a;
        a.viewIdx = accessor.bufferView;
        a.offset = accessor.byteOffset;
        a.count = accessor.count;
        model.accessors.emplace_back(a);
    }

    for (auto& image : gltfModel.images) {
        model.textures.emplace_back(
            parseImage(model, rootDir, image, textureMgr));
    }

    for (auto& gltfMesh : gltfModel.meshes) {
        auto mesh = parseMesh(gltfModel, gltfMesh);
        mesh.model = &model;
        model.meshes.emplace_back(std::move(mesh));
    }

    for (auto& material : gltfModel.materials) {
        Material mat;
        mat.textureIdx = material.pbrMetallicRoughness.baseColorTexture.index;
        mat.uvIdx = material.pbrMetallicRoughness.baseColorTexture.texCoord;
    }

    return model;
}

}  // namespace nickel