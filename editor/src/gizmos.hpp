#pragma once

#include "nickel.hpp"

class Gizmos {
public:
    virtual ~Gizmos() = default;

    virtual void Update(gecs::registry) = 0;
};

class SRTGizmos: public Gizmos {
public:
    enum class Mode {
        Translate,
        Scale,
        Rotate,
    };

    // call this every frame in GameWindow
    void SetMouseRelativePos(const nickel::cgmath::Vec2& pos) { mousePos_ = pos;}
    void SetFBOScale(float scale) { scale_ = scale; }
    void SetEventHandleState(bool b) { shouldHandleEvent_ = b; }

    void SetMode(Mode mode) { mode_ = mode; }
    void Update(gecs::registry) override;

private:
    enum DragType {
        None = 0x00,
        xAxis = 0x01,
        yAxis = 0x02,
        Both = 0x03,
    } dragType_ = None;

    Mode mode_ = Mode::Translate;

    bool shouldHandleEvent_ = false;
    std::optional<nickel::cgmath::Vec2> baseAxis_; // for rotation
    float oldRotation_{};
    nickel::cgmath::Vec2 mousePos_;
    float scale_ = 1.0;

    static constexpr float AxisLen = 100;
    static constexpr float ArrowLen = 10;
    static constexpr float ArrowHeight = 20;
    static constexpr float normalAlpha = 0.5;
    static constexpr float hovingAlpha = 1;
    static constexpr float RotateAxisLen = 100;

    nickel::geom2d::Segment<float> getXAxis(const nickel::cgmath::Vec2& start);
    nickel::geom2d::Segment<float> getYAxis(const nickel::cgmath::Vec2& start);

    bool isHovingOnAxis(const nickel::geom2d::Segment<float>& axis, const nickel::cgmath::Vec2& mousePos);

    void drawTranslateAxis(const nickel::geom2d::Segment<float>& seg,
                  nickel::cgmath::Color color,
                  nickel::Renderer2D& renderer, bool isHoving);

    void drawScaleAxis(const nickel::geom2d::Segment<float>& seg,
                  nickel::cgmath::Color color,
                  nickel::Renderer2D& renderer, bool isHoving);

    void drawRotateLine(const nickel::cgmath::Vec2& center,
                        const nickel::cgmath::Vec2& dir, float len,
                        nickel::cgmath::Color color,
                        nickel::Renderer2D& renderer);

    void drawRect(const nickel::cgmath::Rect& rect,
                  nickel::cgmath::Color color,
                  nickel::Renderer2D& renderer, bool isHoving);

    std::array<nickel::Vertex, 3> getXAxisTriangleVertices(
        float l, float h, const nickel::cgmath::Color& color) {
        float xOffset = h / 3.0;
        float halfL = l * 0.5;
        return {
            nickel::Vertex{nickel::cgmath::Vec3{-xOffset, halfL, 0}, {}, color                           },
            nickel::Vertex{
                           nickel::cgmath::Vec3{-xOffset, -halfL, 0}, {}, color},
            nickel::Vertex{  nickel::cgmath::Vec3{2 * xOffset, 0, 0}, {}, color}
        };
    }
};