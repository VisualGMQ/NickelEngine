#pragma once
#include "common/cgmath.hpp"
#include "graphics/rhi/adapter.hpp"

struct Camera {
public:
    friend class SphericalCoordCameraProxy;

    Camera(nickel::rhi::APIPreference api, const nickel::cgmath::Vec2& size)
        : proj_{nickel::cgmath::CreatePersp(nickel::cgmath::Deg2Rad(45.0f),
                                            size.w / size.h, 0.1, 10000,
                                            api == nickel::rhi::APIPreference::GL)} {}

    auto& View() const { return view_; }

    auto& Proj() const { return proj_; }

    auto& Position() const { return pos_; }

    void Move(const nickel::cgmath::Vec3& pos) { pos_ = pos; }

private:
    nickel::cgmath::Mat44 proj_;
    nickel::cgmath::Mat44 view_ = nickel::cgmath::Mat44::Identity();
    nickel::cgmath::Vec3 pos_;
};

struct SphericalCoordCameraProxy final {
public:
    float radius;
    float phi;
    float theta;

    SphericalCoordCameraProxy(Camera& camera,
                              const nickel::cgmath::Vec3& center)
        : camera_{camera}, center_{center} {
        auto dir = camera_.pos_ - center_;
        auto zAxis = nickel::cgmath::Vec3{0, 1, 0};
        radius = dir.Length();
        dir.Normalize();
        Assert(radius != 0, "your camera too close to origin (distance = 0)");

        auto xozVec = nickel::cgmath::Normalize(dir - (dir.Dot(zAxis) * zAxis));

        theta = std::acos(dir.Dot(nickel::cgmath::Vec3{0, 1, 0}));
        phi = nickel::cgmath::GetRadianIn180Signed(
            nickel::cgmath::Vec2{1, 0},
            nickel::cgmath::Vec2{xozVec.x, xozVec.z});
    }

    void Update2Camera() {
        auto xoyAxis = nickel::cgmath::Vec2{std::cos(phi), std::sin(phi)} *
                       std::sin(theta) * radius;
        auto y = std::cos(theta) * radius;

        camera_.pos_.Set(xoyAxis.x, y, xoyAxis.y);
        camera_.view_ =
            nickel::cgmath::LookAt(center_, camera_.pos_, {0, 1, 0});
    }

private:
    Camera& camera_;
    nickel::cgmath::Vec3 center_;
};
