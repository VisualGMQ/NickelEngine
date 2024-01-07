#pragma once

#include "rhi/enums.hpp"

namespace nickel::rhi {

class ImageView {
public:
    virtual ~ImageView() = default;
};

class Image {
public:
    virtual ~Image() = default;
};

}