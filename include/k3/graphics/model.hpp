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

            struct Builder {

                std::vector<Vertex> verticies{};

                std::vector<uint32_t> indices{};

            };

            KeModel() = default;

            ~KeModel() {if(m_initFlag) shutdown();}

            void init(std::shared_ptr<KeDevice> device, const KeModel::Builder &builder);

            void shutdown();

            void bind(VkCommandBuffer commandBuffer);

            void draw(VkCommandBuffer commandBuffer);

        private:

            void createVertexBuffers(const std::vector<Vertex> &verticies);

            void createIndexBuffers(const std::vector<uint32_t> &indices);

            bool m_initFlag = false;

            std::shared_ptr<KeDevice> m_device;

            VkBuffer m_vertexBuffer;

            VkDeviceMemory m_vertexBufferMemory;

            uint32_t m_vertexCount;

            bool m_hasIndexBuffer = false;

            VkBuffer m_indexBuffer;

            VkDeviceMemory m_indexBufferMemory;

            uint32_t m_indexCount;

    };

}