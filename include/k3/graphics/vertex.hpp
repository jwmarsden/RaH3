#pragma once

#include "log.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "vulkan/vulkan.h"

#include <vector>

namespace k3::graphics {

    struct K3Vertex {

        glm::vec3 position{};

        glm::vec3 color{};

        glm::vec3 normal{};

        glm::vec2 uv{};

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

    };

    struct K3Builder {

        std::vector<K3Vertex> vertices{};

        std::vector<uint32_t> indices{};

        void loadModel(const std::string &filePath, bool flipY=false);

    };

}