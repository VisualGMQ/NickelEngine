#include "renderer/camera.hpp"
#include "refl/camera.hpp"

namespace nickel {

Camera2D::Camera2D(float left, float right, float top, float bottom, float near,
                   float far) {
    SetProject(left, right, top, bottom, near, far);
}

void Camera2D::SetProject(float left, float right, float top, float bottom,
                          float near, float far) {
    project_ = cgmath::CreateOrtho(left, right, top, bottom, near, far);
    cube_.center.Set((left + right) / 2.0f, (top + bottom) / 2.0f,
                     (near + far) / 2.0f);
    cube_.halfLen.Set(std::abs((right - left) / 2.0f),
                       std::abs((top - bottom) / 2.0f),
                       std::abs((near - far) / 2.0f));
}

std::optional<Camera2D> Camera2D::FromConfigFile(std::string_view filename) {
    auto parseResult = toml::parse_file(filename);
    if (parseResult.failed()) {
        LOGE(log_tag::Config, "2D camera file ", filename, " parse failed:\n", parseResult.error());
        return std::nullopt;
    } else {
        auto& tbl = parseResult.table();
        if (auto configTbl = tbl["camera"]; configTbl.is_table()) {
            if (auto camera = *configTbl.as_table(); camera.is_string() && camera.as_string()->get() == "2D") {
                return FromConfig(camera);
            } else {
                LOGE(log_tag::Config, "camera type is not 2D");
            }
        } else {
            LOGE(log_tag::Config, "config file don't has `camera` node");
        }
        return std::nullopt;
    }
}

Camera2D Camera2D::FromConfig(const toml::table& tbl) {
    Assert(tbl.is_table(), "camera config must be a table");
    Assert(tbl["type"].is_string() && tbl["type"].as_string()->get() == "2D",
           "camera is not 2D type");
    Camera2D::ConfigData config;
    mirrow::serd::srefl::deserialize<Camera2D::ConfigData>(tbl, config);
    return Camera2D(config.left, config.right, config.top, config.bottom,
                    config.near, config.far);
}

}  // namespace nicke 