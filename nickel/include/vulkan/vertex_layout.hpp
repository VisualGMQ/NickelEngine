#pragma once

#include "stdpch.hpp"
#include "common/log_tag.hpp"

namespace nickel::vulkan {

enum class ShaderDataType { Float, Vec2, Vec3, Vec4, Mat3, Mat4 };

struct Attribute final {
    ShaderDataType type;
    int location;
    size_t offset;

    Attribute(ShaderDataType type, int location, size_t offset)
        : type(type), location(location), offset(offset) {}

    Attribute(ShaderDataType type, int location)
        : type(type), location(location), offset(0) {}
};

inline uint8_t GetAttributeTypeCount(ShaderDataType type) {
    switch (type) {
        case ShaderDataType::Float:
            return 1;
        case ShaderDataType::Vec2:
            return 2;
        case ShaderDataType::Vec3:
            return 3;
        case ShaderDataType::Vec4:
            return 4;
        case ShaderDataType::Mat3:
            return 3 * 3;
        case ShaderDataType::Mat4:
            return 4 * 4;
    }
    LOGW(log_tag::GL, "Unknown attribute type");
    return 0;
}

inline uint8_t GetAttributeTypeSize(ShaderDataType type) {
    uint8_t typeCount = GetAttributeTypeCount(type);
    switch (type) {
        case ShaderDataType::Vec2:
        case ShaderDataType::Vec3:
        case ShaderDataType::Vec4:
        case ShaderDataType::Float:
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4:
            return typeCount * 4;
    }
    LOGW(log_tag::GL, "Unknown attribute type");
    return 0;
}

class VertexLayout final {
public:
    static auto CreateFromTypes(
        const std::initializer_list<ShaderDataType>& types) {
        std::vector<Attribute> attrs;
        int i = 0;
        size_t offset = 0;
        for (auto type : types) {
            size_t size = GetAttributeTypeSize(type);
            attrs.push_back(Attribute(type, i++, offset));
            offset += size;
        }

        return VertexLayout(std::move(attrs), offset);
    }

    static auto CreateFromUnoffsetAttrs(
        const std::initializer_list<Attribute>& attrs) {
        std::vector<Attribute> attributes;
        int i = 0;
        size_t offset = 0;
        for (auto attr : attrs) {
            size_t size = GetAttributeTypeSize(attr.type);
            attributes.push_back(Attribute(attr.type, i++, offset));
            offset += size;
        }

        return VertexLayout(std::move(attrs), offset);
    }

    static auto CreateFromAttrs(const std::initializer_list<Attribute>& attrs) {
        size_t stride = 0;
        for (auto& attr : attrs) {
            stride += GetAttributeTypeSize(attr.type);
        }
        return VertexLayout(attrs, stride);
    }

    VertexLayout() = default;

    VertexLayout(std::vector<Attribute>&& attrs, size_t stride)
        : attributes_(std::move(attrs)), stride_(stride) {}

    VertexLayout(const std::vector<Attribute>& attrs, size_t stride)
        : attributes_(attrs), stride_(stride) {}

    auto& Attributes() const { return attributes_; }

    auto Stride() const { return stride_; }

private:
    std::vector<Attribute> attributes_;
    size_t stride_ = 0;
};

inline vk::Format CvtAttribFormat(ShaderDataType type) {
    switch (type) {
        case ShaderDataType::Float:
            return vk::Format::eR32Sfloat;
        case ShaderDataType::Vec2:
            return vk::Format::eR32G32Sfloat;
        case ShaderDataType::Mat3:
        case ShaderDataType::Vec3:
            return vk::Format::eR32G32B32Sfloat;
        case ShaderDataType::Vec4:
        case ShaderDataType::Mat4:
            return vk::Format::eR32G32B32A32Sfloat;
    }
}

inline std::vector<vk::VertexInputAttributeDescription>
CvtLayout2AttribDescription(const VertexLayout& layout) {
    std::vector<vk::VertexInputAttributeDescription> descs;

    auto& attrs = layout.Attributes();
    for (int i = 0; i < attrs.size(); i++) {
        auto& attr = attrs[i];
        vk::VertexInputAttributeDescription desc;
        desc.setBinding(0)
            .setFormat(CvtAttribFormat(attr.type))
            .setLocation(attr.location)
            .setOffset(attr.offset);

        descs.push_back(desc);
    }

    return descs;
}

inline vk::VertexInputBindingDescription
CvtLayout2BindingDescription(const VertexLayout& layout) {
    auto& attrs = layout.Attributes();

    vk::VertexInputBindingDescription desc;
    desc.setInputRate(vk::VertexInputRate::eVertex)
        .setBinding(0)
        .setStride(layout.Stride());
    return desc;
}

}  // namespace nickel::vulkan