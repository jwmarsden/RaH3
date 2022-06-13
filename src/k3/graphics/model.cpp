#include "k3/graphics/model.hpp"

#include <cassert>
#include <cstring>

namespace k3::graphics {

    K3Model::K3Model(std::shared_ptr<K3Device> device, const K3Builder &builder) : m_device {device}, m_builder {builder} {
        KE_IN(KE_NOARG);

        createVertexBuffers(builder.vertices);
        if(builder.indices.size() > 0) {
            m_hasIndexBuffer = true;
        }
        if(m_hasIndexBuffer) {
            createIndexBuffers(builder.indices);
        }

        KE_OUT(KE_NOARG);
    }

    K3Model::~K3Model() {
        KE_IN(KE_NOARG);

        if(m_device != nullptr) {
            m_device = nullptr;
        }
        
        KE_OUT(KE_NOARG);
    }

    std::unique_ptr<K3Model> K3Model::createModelFromFile(std::shared_ptr<K3Device> device, const std::string &filePath, bool flipY) {
        KE_IN(KE_NOARG);
        K3Builder builder{};
        builder.loadModel(filePath, flipY);
        KE_DEBUG("Vertex Count: {}", builder.vertices.size());
        KE_OUT(KE_NOARG);
        return std::make_unique<K3Model>(device, builder);
    }

    void K3Model::createVertexBuffers(const std::vector<K3Vertex> &vertices) {
        KE_IN(KE_NOARG);

        m_vertexCount = static_cast<uint32_t>(vertices.size());
        assert(m_vertexCount >= 3 && "Vertex Count Must Be At Least 3");
        
        VkDeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;
        uint32_t vertexSize = sizeof(vertices[0]);

        K3Buffer stagingBuffer {
            m_device,
            vertexSize,
            m_vertexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *) vertices.data());

        m_vertexBuffer = std::make_unique<K3Buffer>(
            m_device,
            vertexSize,
            m_vertexCount,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        );

        m_device->copyBuffer(stagingBuffer.getBuffer(), m_vertexBuffer->getBuffer(), bufferSize);
        KE_OUT(KE_NOARG);
    }

    void K3Model::createIndexBuffers(const std::vector<uint32_t> &indices) {
        KE_IN(KE_NOARG);
        
        m_indexCount = static_cast<uint32_t>(indices.size());
        VkDeviceSize bufferSize = sizeof(indices[0]) * m_indexCount;
        uint32_t indexSize = sizeof(indices[0]);

        K3Buffer stagingBuffer {
            m_device,
            indexSize,
            m_indexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *) indices.data());

        m_indexBuffer = std::make_unique<K3Buffer>(
            m_device,
            indexSize,
            m_indexCount,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        );

        m_device->copyBuffer(stagingBuffer.getBuffer(), m_indexBuffer->getBuffer(), bufferSize);

        KE_OUT(KE_NOARG);
    }    
    
    void K3Model::bind(VkCommandBuffer commandBuffer) {
        KE_IN_SPAM(KE_NOARG);
        VkBuffer buffers[] = {m_vertexBuffer->getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        if(m_hasIndexBuffer) {
            vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }
        KE_OUT_SPAM(KE_NOARG);
    }

    void K3Model::draw(VkCommandBuffer commandBuffer) {
        KE_IN_SPAM(KE_NOARG);
        if(m_hasIndexBuffer) {
            vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
        } else {
            vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
        }
        KE_OUT_SPAM(KE_NOARG);
    }


}