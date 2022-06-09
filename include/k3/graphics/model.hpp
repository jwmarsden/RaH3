#pragma once

#include "log.h"

#include "vertex.hpp"
#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace k3::graphics {

    class KeModel {

        public:

            KeModel(std::shared_ptr<KeDevice> device, const K3Builder &builder);

            ~KeModel();

            static std::unique_ptr<KeModel> createModelFromFile(std::shared_ptr<KeDevice> device, const std::string &filePath, bool flipY=false);

            void bind(VkCommandBuffer commandBuffer);

            void draw(VkCommandBuffer commandBuffer);

        private:

            void createVertexBuffers(const std::vector<K3Vertex> &vertices);

            void createIndexBuffers(const std::vector<uint32_t> &indices);

            std::shared_ptr<KeDevice> m_device = nullptr;

            VkBuffer m_vertexBuffer;

            VkDeviceMemory m_vertexBufferMemory;

            uint32_t m_vertexCount = -1;

            bool m_hasIndexBuffer = false;

            VkBuffer m_indexBuffer;

            VkDeviceMemory m_indexBufferMemory;

            uint32_t m_indexCount = -1;

            const K3Builder m_builder;

    };

}