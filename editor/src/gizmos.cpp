#include "gizmos.hpp"
#include "context.hpp"

void SRTGizmos::Update(gecs::registry reg) {
    auto ctx = reg.res<EditorContext>();
    auto renderer2D = reg.res_mut<nickel::Renderer2D>();
    auto camera = reg.res_mut<nickel::Camera>();
    auto mouse = reg.res<nickel::Mouse>();

    auto entity = ctx->entityListWindow.GetSelected();
    if (!reg.alive(entity) || !reg.has<nickel::Transform>(entity)) {
        return;
    }

    auto& transform = reg.get_mut<nickel::Transform>(entity);

    auto mousePos = mouse->Position() - offset_;

    renderer2D->BeginRenderTexture(camera.get());

    auto xAxis = getXAxis(transform.translation);
    auto yAxis = getYAxis(transform.translation);
    const nickel::cgmath::Vec2 rectSize{25, 25};
    auto rect = nickel::cgmath::Rect{
        transform.translation - rectSize * 0.5, {25, 25}
    };

    bool xHoving = isHovingOnAxis(xAxis, mousePos);
    bool yHoving = isHovingOnAxis(yAxis, mousePos);
    bool isInRect = rect.IsPtIn(mousePos);

    nickel::cgmath::Color xAxisColor{1, 0, 0, normalAlpha};
    nickel::cgmath::Color yAxisColor{0, 1, 0, normalAlpha};
    nickel::cgmath::Color rotAxisColor{1, 1, 0, normalAlpha};

    bool xAxisHighlight = xHoving || isInRect || dragType_ & DragType::xAxis;
    bool yAxisHighlight = yHoving || isInRect || dragType_ & DragType::yAxis;

    switch (mode_) {
        case Mode::Translate:
            drawTranslateAxis(xAxis, xAxisColor, renderer2D.get(),
                              xAxisHighlight);
            drawTranslateAxis(yAxis, yAxisColor, renderer2D.get(),
                              yAxisHighlight);
            break;
        case Mode::Scale:
            drawScaleAxis(xAxis, xAxisColor, renderer2D.get(), xAxisHighlight);
            drawScaleAxis(yAxis, yAxisColor, renderer2D.get(), yAxisHighlight);
            break;
        case Mode::Rotate:
            if (baseAxis_) {
                drawRotateLine(transform.translation, baseAxis_.value(),
                               RotateAxisLen, rotAxisColor, renderer2D.get());
            }
            break;
    }

    drawRect(rect, {1, 1, 0, normalAlpha}, renderer2D.get(),
             isInRect || dragType_ == DragType::Both);

    if (mouse->LeftBtn().IsPressed()) {
        if (xHoving) {
            dragType_ = DragType::xAxis;
        }

        if (yHoving) {
            dragType_ = DragType::yAxis;
        }

        if (isInRect) {
            dragType_ = DragType::Both;
        }

        if (mode_ == Mode::Rotate) {
            baseAxis_ = nickel::cgmath::Normalize(mousePos - transform.translation);
            oldRotation_ = transform.rotation;
        }
    }

    if (mouse->LeftBtn().IsReleased()) {
        dragType_ = DragType::None;
        baseAxis_.reset();
    }

    // handle drag
    constexpr float ScaleFactor = 0.01;
    switch (mode_) {
        case Mode::Translate:
            if (dragType_ & DragType::xAxis) {
                transform.translation.x += mouse->Offset().x;
            }
            if (dragType_ & DragType::yAxis) {
                transform.translation.y += mouse->Offset().y;
            }
            break;
        case Mode::Scale:
            if (dragType_ == DragType::Both) {
                auto scale =
                    std::max(mouse->Offset().x, mouse->Offset().y) * ScaleFactor;
                transform.scale.x += scale;
                transform.scale.y += scale;
            } else if (dragType_ & DragType::xAxis) {
                transform.scale.x += mouse->Offset().x * ScaleFactor;
            } else if (dragType_ & DragType::yAxis) {
                transform.scale.y += mouse->Offset().y * ScaleFactor;
            }
            break;
        case Mode::Rotate:
            if (baseAxis_) {
                auto axis = nickel::cgmath::Normalize(mousePos - transform.translation);
                drawRotateLine(transform.translation, axis,
                               RotateAxisLen, rotAxisColor, renderer2D.get());
                auto sign = nickel::cgmath::Sign(nickel::cgmath::Cross(baseAxis_.value(), axis));
                auto deg = sign * nickel::cgmath::Rad2Deg(std::acos(nickel::cgmath::Dot(baseAxis_.value(), axis)));
                transform.rotation = std::clamp(oldRotation_ + deg, 0.0f, 360.0f);
            }
            break;
    }

    renderer2D->EndRender();
}

bool SRTGizmos::isHovingOnAxis(const nickel::geom2d::Segment<float>& axis,
                               const nickel::cgmath::Vec2& mousePos) {
    constexpr float tol = 5;
    return (nickel::geom::SegNearestPt(axis, mousePos) - mousePos)
               .LengthSqrd() <= tol * tol;
}

nickel::geom2d::Segment<float> SRTGizmos::getXAxis(
    const nickel::cgmath::Vec2& start) {
    auto end = start + nickel::cgmath::Vec2{AxisLen, 0};
    return nickel::geom2d::Segment<float>::FromPts(start, end);
}

nickel::geom2d::Segment<float> SRTGizmos::getYAxis(
    const nickel::cgmath::Vec2& start) {
    auto end = start + nickel::cgmath::Vec2{0, AxisLen};
    return nickel::geom2d::Segment<float>::FromPts(start, end);
}

void SRTGizmos::drawTranslateAxis(const nickel::geom2d::Segment<float>& seg,
                                  nickel::cgmath::Color color,
                                  nickel::Renderer2D& renderer, bool isHoving) {
    if (isHoving) {
        color.a = hovingAlpha;
    }

    auto triangleVertices =
        getXAxisTriangleVertices(ArrowLen, ArrowHeight, color);
    auto end = seg.p + seg.dir * seg.len;
    renderer.DrawLine(seg.p, end, color);
    renderer.DrawTriangles(
        triangleVertices, std::array<uint32_t, 0>{},
        nickel::cgmath::CreateTranslation(nickel::cgmath::Vec3{end}) *
            nickel::cgmath::CreateXYZRotation(
                {0, 0, std::asin(nickel::cgmath::Cross({1, 0}, seg.dir))}));
}

void SRTGizmos::drawScaleAxis(const nickel::geom2d::Segment<float>& seg,
                              nickel::cgmath::Color color,
                              nickel::Renderer2D& renderer, bool isHoving) {
    if (isHoving) {
        color.a = hovingAlpha;
    }
    auto end = seg.p + seg.dir * seg.len;
    renderer.DrawLine(seg.p, end, color);
    constexpr float RectLen = 15;
    nickel::cgmath::Rect rect{end.x - RectLen * 0.5f,
                              end.y - RectLen * 0.5f, RectLen, RectLen};
    renderer.FillRect(rect, color);
}

void SRTGizmos::drawRect(const nickel::cgmath::Rect& rect,
                         nickel::cgmath::Color color,
                         nickel::Renderer2D& renderer, bool isHoving) {
    if (isHoving) {
        color.a = hovingAlpha;
    }
    renderer.FillRect(rect, color);
}

void SRTGizmos::drawRotateLine(const nickel::cgmath::Vec2& center,
                               const nickel::cgmath::Vec2& dir, float len,
                               nickel::cgmath::Color color,
                               nickel::Renderer2D& renderer) {
    renderer.DrawLine(center, center + dir * len, color);
}