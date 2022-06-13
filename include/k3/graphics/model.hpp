#pragma once

#include "log.h"

#include "vertex.hpp"
#include "device.hpp"
#include "buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace k3::graphics {

    class K3Model {

        public:

            K3Model(std::shared_ptr<K3Device> device, const K3Builder &builder);

            ~K3Model();

            static std::unique_ptr<K3Model> createModelFromFile(std::shared_ptr<K3Device> device, const std::string &filePath, bool flipY=false);

            void bind(VkCommandBuffer commandBuffer);

            void draw(VkCommandBuffer commandBuffer);

        private:

            void createVertexBuffers(const std::vector<K3Vertex> &vertices);

            void createIndexBuffers(const std::vector<uint32_t> &indices);

            std::shared_ptr<K3Device> m_device = nullptr;

            std::unique_ptr<K3Buffer> m_vertexBuffer;

            uint32_t m_vertexCount = -1;

            bool m_hasIndexBuffer = false;

            std::unique_ptr<K3Buffer> m_indexBuffer;

            uint32_t m_indexCount = -1;

            const K3Builder m_builder;

    };

}