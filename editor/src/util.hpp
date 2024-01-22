#pragma once

#include "nickel.hpp"

/**
 * @brief a help function to scale a point by a anchor
 * the p will move offset, and scale accroding to anchor
 */
inline nickel::cgmath::Vec2 ScaleByAnchor(const nickel::cgmath::Vec2& p,
                                          float scale,
                                          const nickel::cgmath::Vec2& halfSize,
                                          const nickel::cgmath::Vec2& offset) {
    return (p + offset - halfSize) * scale + halfSize;
}

/**
 * @brief a help function to scale a point by a anchor
 * the p will move offset, and scale accroding to anchor
 */
inline nickel::cgmath::Mat44 ScaleByAnchorAsMat(
    const nickel::cgmath::Vec2& p, float scale,
    const nickel::cgmath::Vec2& halfSize, const nickel::cgmath::Vec2& offset) {
    return nickel::cgmath::CreateTranslation(
               nickel::cgmath::Vec3{halfSize.x, halfSize.y, 0}) *
           nickel::cgmath::CreateScale({scale, scale, scale}) *
           nickel::cgmath::CreateTranslation(nickel::cgmath::Vec3{
               p.x + offset.x - halfSize.x, p.y + offset.y - halfSize.y, 0});
}

bool ChDir(const std::filesystem::path&);

#define FS_LOG_ERR(err, ...)                                        \
    do {                                                            \
        if (err) {                                                  \
            LOGE(nickel::log_tag::Filesystem, err.message(), " : ", \
                 ##__VA_ARGS__);                                    \
        }                                                           \
    } while (0)

#define FS_CALL(expr, err)                                    \
    do {                                                      \
        (expr);                                               \
        if (err) {                                            \
            LOGE(nickel::log_tag::Filesystem, err.message()); \
        }                                                     \
    } while (0)