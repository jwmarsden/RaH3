#pragma once

#include "log.h"

#include "camera.hpp"

#include <vulkan/vulkan.h>

namespace k3::graphics {

    struct K3FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        k3::graphics::K3Camera &camera;
    };

}

