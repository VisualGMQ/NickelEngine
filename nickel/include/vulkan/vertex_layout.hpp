#pragma once

#include "pch.hpp"

namespace nickel::vulkan {

struct Attribute final {
    enum class Type { Float, Vec2, Vec3, Vec4 } type;
    int location;
    size_t offset;

    Attribute(Type type, int location, size_t offset)
        : type(type), location(location), offset(offset) {}

    Attribute(Type type, int location)
        : type(type), location(location), offset(0) {}
};

inline uint8_t GetAttributeTypeCount(Attribute::Type type) {
    switch (type) {
        case Attribute::Type::Float:
            return 1;
        case Attribute::Type::Vec2:
            return 2;
        case Attribute::Type::Vec3:
            return 3;
        case Attribute::Type::Vec4:
            return 4;
    }
    LOGW(log_tag::GL, "Unknown attribute type");
    return 0;
}

inline uint8_t GetAttributeTypeSize(Attribute::Type type) {
    uint8_t typeCount = GetAttributeTypeCount(type);
    switch (type) {
        case Attribute::Type::Vec2:
        case Attribute::Type::Vec3:
        case Attribute::Type::Vec4:
        case Attribute::Type::Float:
            return typeCount * 4;
            break;
    }
    LOGW(log_tag::GL, "Unknown attribute type");
    return 0;
}

class VertexLayout final {
public:
    static auto CreateFromTypes(
        const std::initializer_list<Attribute::Type>& types) {
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


inline vk::Format CvtAttribFormat(Attribute::Type type) {
    switch (type) {
        case Attribute::Type::Float:
            return vk::Format::eR32Sfloat;
        case Attribute::Type::Vec2:
            return vk::Format::eR32G32Sfloat;
        case Attribute::Type::Vec3:
            return vk::Format::eR32G32B32Sfloat;
        case Attribute::Type::Vec4:
            return vk::Format::eR32G32B32A32Sfloat;
    }
}

inline std::vector<vk::VertexInputAttributeDescription> CvtLayout2AttribDescription(
    const VertexLayout& layout) {
    std::vector<vk::VertexInputAttributeDescription> descs;

    auto& attrs = layout.Attributes();
    for (int i = 0; i < attrs.size(); i++) {
        auto& attr = attrs[i];
        vk::VertexInputAttributeDescription desc;
        desc.setBinding(attr.location)
            .setFormat(CvtAttribFormat(attr.type))
            .setBinding(0);

        descs.push_back(desc);
    }

    return descs;
}

inline std::vector<vk::VertexInputBindingDescription> CvtLayout2BindingDescription(
    const VertexLayout& layout) {
    auto& attrs = layout.Attributes();

    std::vector<vk::VertexInputBindingDescription> descs;
    for (auto& attr : attrs) {
        vk::VertexInputBindingDescription desc;
        desc.setInputRate(vk::VertexInputRate::eVertex)
            .setBinding(0)
            .setStride(layout.Stride());
        descs.push_back(desc);
    }
    return descs;
}

}  // namespace nickel::vulkan