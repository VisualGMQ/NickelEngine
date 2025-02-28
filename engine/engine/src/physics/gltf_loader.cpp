#include "nickel/graphics/internal/gltf_loader.hpp"

#include "nickel/graphics/internal/context_impl.hpp"
#include "nickel/graphics/internal/gltf_manager_impl.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"
#include "nickel/graphics/internal/mesh_impl.hpp"
#include "nickel/graphics/internal/texture_impl.hpp"

namespace nickel::graphics {

GLTFLoader::GLTFLoader(const tinygltf::Model& model) : m_gltf_model{model} {}

GLTFLoadData GLTFLoader::Load(const Path& filename, const Adapter& adapter,
                              GLTFManagerImpl& gltf_manager) {
    return loadGLTF(filename, adapter, gltf_manager,
                    nickel::Context::GetInst()
                        .GetGraphicsContext()
                        .GetImpl()
                        ->GetGLTFRenderPass(),
                    nickel::Context::GetInst().GetTextureManager());
}

GLTFLoadData GLTFLoader::loadGLTF(const Path& filename, const Adapter& adapter,
                                  GLTFManagerImpl& gltf_manager,
                                  GLTFRenderPass& render_pass,
                                  TextureManager& texture_mgr) {
    Device device = adapter.GetDevice();

    Path root_dir = filename.ParentPath();
    GLTFLoadData load_data;
    load_data.m_resource =
        gltf_manager.m_model_resource_allocator.Allocate(&gltf_manager);
    GLTFModelResourceImpl* resource = load_data.m_resource.GetImpl();
    resource->m_gpu_resource.textures.reserve(m_gltf_model.images.size());
    resource->m_gpu_resource.materials.reserve(m_gltf_model.materials.size());

    // load images
    for (int i = 0; i < m_gltf_model.images.size(); i++) {
        auto& image = m_gltf_model.images[i];
        resource->m_gpu_resource.textures.push_back(texture_mgr.Load(
            root_dir / Path{ParseURI2Path(image.uri)}, Format::R8G8B8A8_UNORM));
    }

    // load samplers
    for (auto& sampler : m_gltf_model.samplers) {
        resource->m_gpu_resource.samplers.emplace_back(
            createSampler(device, sampler));
    }

    auto& common_res = nickel::Context::GetInst()
                           .GetGraphicsContext()
                           .GetImpl()
                           ->GetCommonResource();

    std::vector<Material3D::Descriptor> mtl_desces;

    // load material
    for (auto& mtl : m_gltf_model.materials) {
        // Material3D material;
        auto align = adapter.GetLimits().min_uniform_buffer_offset_alignment;
        Material3D::Descriptor desc;

        uint32_t pbr_parameter_offset =
            std::ceil(resource->m_cpu_data.pbr_parameters.size() /
                      (float)align) *
            align;
        uint32_t pbr_parameter_size = sizeof(PBRParameters);
        resource->m_cpu_data.pbr_parameters.resize(pbr_parameter_offset +
                                                   pbr_parameter_size);
        desc.pbrParameters.m_count = 1;
        desc.pbrParameters.m_offset = pbr_parameter_offset;
        desc.pbrParameters.m_size = pbr_parameter_size;

        PBRParameters pbrParam;

        auto& colorFactor = mtl.pbrMetallicRoughness.baseColorFactor;
        pbrParam.m_base_color.r = colorFactor[0];
        pbrParam.m_base_color.g = colorFactor[1];
        pbrParam.m_base_color.b = colorFactor[2];
        pbrParam.m_base_color.a = colorFactor[3];

        pbrParam.m_metallic = mtl.pbrMetallicRoughness.metallicFactor;
        pbrParam.m_roughness = mtl.pbrMetallicRoughness.roughnessFactor;

        memcpy(
            resource->m_cpu_data.pbr_parameters.data() + pbr_parameter_offset,
            &pbrParam, sizeof(PBRParameters));

        if (auto idx = mtl.pbrMetallicRoughness.baseColorTexture.index;
            idx != -1) {
            desc.basicTexture = parseTextureInfo(idx, *resource);
        } else {
            desc.basicTexture.sampler = common_res.m_default_sampler;
            desc.basicTexture.image = common_res.m_white_image;
        }

        if (auto idx = mtl.pbrMetallicRoughness.metallicRoughnessTexture.index;
            idx != -1) {
            desc.metalicRoughnessTexture = parseTextureInfo(idx, *resource);
        } else {
            desc.metalicRoughnessTexture.sampler = common_res.m_default_sampler;
            desc.metalicRoughnessTexture.image = common_res.m_black_image;
        }

        if (auto idx = mtl.normalTexture.index; idx != -1) {
            desc.normalTexture = parseTextureInfo(idx, *resource);
        } else {
            desc.normalTexture.sampler = common_res.m_default_sampler;
            desc.normalTexture.image = common_res.m_default_normal_image;
        }

        if (auto idx = mtl.occlusionTexture.index; idx != -1) {
            desc.occlusionTexture = parseTextureInfo(idx, *resource);
        } else {
            desc.occlusionTexture.sampler = common_res.m_default_sampler;
            desc.occlusionTexture.image = common_res.m_white_image;
        }

        mtl_desces.push_back(std::move(desc));
    }

    resource->m_gpu_resource.pbr_parameter_buffer =
        copyBuffer2GPU(device, std::span{resource->m_cpu_data.pbr_parameters},
                       Flags{BufferUsage::Uniform} | BufferUsage::CopyDst);

    for (auto& desc : mtl_desces) {
        desc.pbr_param_buffer = resource->m_gpu_resource.pbr_parameter_buffer;
        resource->m_gpu_resource.materials.push_back(
            Material3D{gltf_manager.m_mtl_allocator.Allocate(
                gltf_manager, desc, render_pass.GetBindGroupLayout())});
    }

    load_data.m_meshes.reserve(m_gltf_model.meshes.size());
    for (auto& m : m_gltf_model.meshes) {
        Mesh mesh = createMesh(device, m, &gltf_manager, *resource);
        load_data.m_meshes.push_back(mesh);
    }
    return load_data;
}

Material3D::TextureInfo GLTFLoader::parseTextureInfo(
    int idx, GLTFModelResourceImpl& model) {
    Material3D::TextureInfo texture_info;
    auto& info = m_gltf_model.textures[idx];
    if (info.source != -1) {
        texture_info.image =
            model.m_gpu_resource.textures[info.source].GetImpl()->m_view;
        if (info.sampler != -1) {
            texture_info.sampler = model.m_gpu_resource.samplers[info.sampler];
        } else {
            texture_info.sampler = nickel::Context::GetInst()
                                       .GetGraphicsContext()
                                       .GetImpl()
                                       ->GetCommonResource()
                                       .m_default_sampler;
        }
    }
    return texture_info;
}

Sampler GLTFLoader::createSampler(Device device,
                                  const tinygltf::Sampler& gltfSampler) {
    Sampler::Descriptor desc;
    desc.m_min_filter = GLTFFilter2RHI(gltfSampler.minFilter);
    desc.m_mag_filter = GLTFFilter2RHI(gltfSampler.magFilter);
    desc.m_address_mode_u = GLTFWrapper2RHI(gltfSampler.wrapS);
    desc.m_address_mode_v = GLTFWrapper2RHI(gltfSampler.wrapT);
    return device.CreateSampler(desc);
}

Mesh GLTFLoader::createMesh(Device device, const tinygltf::Mesh& gltf_mesh,
                            GLTFManagerImpl* mgr,
                            GLTFModelResourceImpl& model) {
    MeshImpl* newNode = mgr->m_mesh_allocator.Allocate(mgr);
    newNode->m_name = gltf_mesh.name;

    std::vector<unsigned char> data_buffer;
    for (uint32_t i = 0; i < gltf_mesh.primitives.size(); i++) {
        auto prim =
            recordPrimInfo(data_buffer, model.m_cpu_data.data_buffers.size(),
                           gltf_mesh.primitives[i]);
        newNode->m_primitives.emplace_back(prim);
    }
    model.m_cpu_data.data_buffers.push_back(data_buffer);
    model.m_gpu_resource.dataBuffers.emplace_back(
        copyBuffer2GPU(device, std::span{model.m_cpu_data.data_buffers.back()},
                       Flags{BufferUsage::Index} | BufferUsage::CopyDst |
                           BufferUsage::Vertex));

    return newNode;
}

Primitive GLTFLoader::recordPrimInfo(std::vector<unsigned char>& data_buffer,
                                     uint32_t buffer_idx,
                                     const tinygltf::Primitive& prim) {
    Primitive primitive;

    auto& attrs = prim.attributes;
    if (auto it = attrs.find("POSITION"); it != attrs.end()) {
        auto& accessor = m_gltf_model.accessors[it->second];
        primitive.m_pos_buf_view =
            RecordBufferView<float>(m_gltf_model, accessor, data_buffer,
                                    buffer_idx, TINYGLTF_TYPE_VEC3);
    }

    if (prim.indices != -1) {
        auto& accessor = m_gltf_model.accessors[prim.indices];
        if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            primitive.m_index_type = IndexType::Uint16;
            primitive.m_indices_buf_view =
                RecordBufferView<uint16_t>(m_gltf_model, accessor, data_buffer,
                                           buffer_idx, TINYGLTF_TYPE_SCALAR);
        } else if (accessor.componentType ==
                   TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
            primitive.m_index_type = IndexType::Uint32;
            primitive.m_indices_buf_view =
                RecordBufferView<uint32_t>(m_gltf_model, accessor, data_buffer,
                                           buffer_idx, TINYGLTF_TYPE_SCALAR);
        } else {
            NICKEL_CANT_REACH();
        }
    }

    bool has_uv = true;
    if (auto it = attrs.find("TEXCOORD_0"); it != attrs.end()) {
        auto& accessor = m_gltf_model.accessors[it->second];
        primitive.m_uv_buf_view =
            RecordBufferView<float>(m_gltf_model, accessor, data_buffer,
                                    buffer_idx, TINYGLTF_TYPE_VEC2);
    } else {
        // some trivial data
        primitive.m_uv_buf_view = primitive.m_pos_buf_view;
        has_uv = false;
    }

    const BufferView& position_buffer_view = primitive.m_pos_buf_view;

    if (auto it = attrs.find("NORMAL"); it != attrs.end()) {
        auto& accessor = m_gltf_model.accessors[it->second];

        primitive.m_norm_buf_view =
            RecordBufferView<float>(m_gltf_model, accessor, data_buffer,
                                    buffer_idx, TINYGLTF_TYPE_VEC3);
    } else {
        size_t old_size = data_buffer.size();
        uint32_t pos_count = position_buffer_view.m_count;
        size_t size = pos_count * sizeof(Vec3);
        data_buffer.resize(data_buffer.size() + size, 0);
        Vec3* norm_ptr = (Vec3*)(data_buffer.data() + old_size);

        BufferView view;
        view.m_count = pos_count;
        view.m_size = size;
        view.m_offset = old_size;
        view.m_buffer = buffer_idx;
        primitive.m_norm_buf_view = view;

        auto posPtr =
            (const Vec3*)(data_buffer.data() + position_buffer_view.m_offset);
        if (primitive.m_indices_buf_view) {
            const BufferView& indices_buffer_view =
                primitive.m_indices_buf_view;
            auto indices_ptr =
                data_buffer.data() + indices_buffer_view.m_offset;

            for (int i = 0; i < indices_buffer_view.m_count / 3; i++) {
                auto idx1 = indices_ptr[i * 3];
                auto idx2 = indices_ptr[i * 3 + 1];
                auto idx3 = indices_ptr[i * 3 + 2];

                auto pos1 = posPtr[idx1];
                auto pos2 = posPtr[idx2];
                auto pos3 = posPtr[idx3];
                auto normal = Cross(Normalize(pos2 - pos1), pos3 - pos1);
                norm_ptr[idx1] = normal;
                norm_ptr[idx2] = normal;
                norm_ptr[idx3] = normal;
            }
        } else {
            for (int i = 0; i < position_buffer_view.m_count / 3; i++) {
                auto pos1 = posPtr[i * 3];
                auto pos2 = posPtr[i * 3 + 1];
                auto pos3 = posPtr[i * 3 + 2];
                auto normal = Cross(Normalize(pos2 - pos1), pos3 - pos1);
                norm_ptr[i * 3] = normal;
                norm_ptr[i * 3 + 1] = normal;
                norm_ptr[i * 3 + 2] = normal;
            }
        }
    }

    if (auto it = attrs.find("TANGENT"); it != attrs.end()) {
        auto& accessor = m_gltf_model.accessors[it->second];

        primitive.m_tan_buf_view =
            RecordBufferView<float>(m_gltf_model, accessor, data_buffer,
                                    buffer_idx, TINYGLTF_TYPE_VEC4);
    } else {
        size_t old_size = data_buffer.size();
        uint32_t pos_count = position_buffer_view.m_count;
        size_t size = pos_count * sizeof(Vec4);
        data_buffer.resize(data_buffer.size() + size, 0);

        BufferView view;
        view.m_count = pos_count;
        view.m_size = size;
        view.m_offset = old_size;
        view.m_buffer = buffer_idx;
        primitive.m_tan_buf_view = view;

        const BufferView& uv_buffer_view = primitive.m_uv_buf_view;

        auto posPtr =
            (const Vec3*)(data_buffer.data() + position_buffer_view.m_offset);
        auto uvPtr =
            (const Vec2*)(data_buffer.data() + uv_buffer_view.m_offset);
        auto tanPtr = (Vec4*)(data_buffer.data() + view.m_offset);
        if (primitive.m_indices_buf_view) {
            const BufferView& indices_buffer_view =
                primitive.m_indices_buf_view;
            auto indicesPtr = (const uint16_t*)(data_buffer.data() +
                                                indices_buffer_view.m_offset);

            NICKEL_ASSERT(indices_buffer_view.m_count % 3 == 0,
                          "indices can't be triangle list");
            for (int i = 0; i < indices_buffer_view.m_count / 3; i++) {
                uint32_t idx1, idx2, idx3;
                switch (primitive.m_index_type) {
                    case IndexType::Uint16:
                        idx1 = *(indicesPtr + i * 3);
                        idx2 = *(indicesPtr + i * 3 + 1);
                        idx3 = *(indicesPtr + i * 3 + 2);
                        break;
                    case IndexType::Uint32:
                        idx1 = *(indicesPtr + 2 * (i * 3));
                        idx2 = *(indicesPtr + 2 * (i * 3 + 1));
                        idx3 = *(indicesPtr + 2 * (i * 3 + 2));
                        break;
                }

                auto uv1 = *(uvPtr + idx1);
                auto uv2 = *(uvPtr + idx2);
                auto uv3 = *(uvPtr + idx3);

                auto pos1 = *(posPtr + idx1);
                auto pos2 = *(posPtr + idx2);
                auto pos3 = *(posPtr + idx3);

                Vec4 tangent;

                if (!has_uv ||
                    (uv1 == Vec2{} && uv2 == Vec2{} && uv3 == Vec2{})) {
                    auto t = Normalize(pos2 - pos1);
                    tangent.x = t.x;
                    tangent.y = t.y;
                    tangent.z = t.z;
                    tangent.w = 1;
                } else {
                    auto [tan, _] =
                        GetNormalMapTB(pos1, pos2, pos3, uv1, uv2, uv3);

                    tangent = Vec4{tan.x, tan.y, tan.z, 1};
                }
                *(tanPtr + idx1) = tangent;
                *(tanPtr + idx2) = tangent;
                *(tanPtr + idx3) = tangent;
            }
        } else {
            // TODO:
        }
    }

    if (prim.material != -1) {
        primitive.m_material = prim.material;
    }

    return primitive;
}

}  // namespace nickel::graphics