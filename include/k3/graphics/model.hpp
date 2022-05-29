#pragma once

#include "log.h"

#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace k3::graphics {

    class KeModel {

        public:

            struct Vertex {

                glm::vec3 position;

                glm::vec3 color;

                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            };

            KeModel() = default;

            ~KeModel() {if(m_initFlag) shutdown();}

            void init(std::shared_ptr<KeDevice> device, std::vector<Vertex> verticies);

            void shutdown();

            void bind(VkCommandBuffer commandBuffer);

            void draw(VkCommandBuffer commandBuffer);

        private:

            void createVertexBuffers(const std::vector<Vertex> verticies);

            bool m_initFlag = false;

            std::vector<Vertex> m_vertices;

            std::shared_ptr<KeDevice> m_device;

            VkBuffer m_vertexBuffer;

            VkDeviceMemory m_vertexBufferMemory;

            uint32_t m_vertexCount;

    };

}