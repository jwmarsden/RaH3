#pragma once

#include "k3/logging/log.hpp"

#include "utils.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "vulkan/vulkan.h"

#include <vector>
#include <unordered_map>

namespace k3::graphics {

    struct K3Vertex {

        glm::vec3 position{};

        glm::vec3 color{};

        glm::vec3 normal{};

        glm::vec2 uv{};

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

        bool operator==(const K3Vertex other) const {
            return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
        }

    };

    struct K3Builder {

        std::vector<K3Vertex> vertices{};

        std::vector<uint32_t> indices{};

        void loadModel(const std::string &filePath);

    };

}