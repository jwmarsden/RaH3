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

        vkDestroyBuffer(m_device->getDevice(), m_vertexBuffer, nullptr);
        vkFreeMemory(m_device->getDevice() , m_vertexBufferMemory, nullptr);
        if(m_hasIndexBuffer) {
            vkDestroyBuffer(m_device->getDevice(), m_indexBuffer, nullptr);
            vkFreeMemory(m_device->getDevice() , m_indexBufferMemory, nullptr);
        }

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

        return std::make_unique<K3Model>(device, builder);
        KE_OUT(KE_NOARG);
    }

    void K3Model::createVertexBuffers(const std::vector<K3Vertex> &vertices) {
        KE_IN(KE_NOARG);

        m_vertexCount = static_cast<uint32_t>(vertices.size());
        assert(m_vertexCount >= 3 && "Vertex Count Must Be At Least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        m_device->createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void *data;
        vkMapMemory(m_device->getDevice() , stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(m_device->getDevice() , stagingBufferMemory);

        m_device->createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            m_vertexBuffer,
            m_vertexBufferMemory);

        m_device->copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);
        vkDestroyBuffer(m_device->getDevice(), stagingBuffer, nullptr);
        vkFreeMemory(m_device->getDevice() , stagingBufferMemory, nullptr);

        KE_OUT(KE_NOARG);
    }

    void K3Model::createIndexBuffers(const std::vector<uint32_t> &indices) {
        KE_IN(KE_NOARG);
        
        m_indexCount = static_cast<uint32_t>(indices.size());
        VkDeviceSize bufferSize = sizeof(indices[0]) * m_indexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        m_device->createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void *data;
        vkMapMemory(m_device->getDevice() , stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(m_device->getDevice() , stagingBufferMemory);

        m_device->createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            m_indexBuffer,
            m_indexBufferMemory);

        m_device->copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);
        vkDestroyBuffer(m_device->getDevice(), stagingBuffer, nullptr);
        vkFreeMemory(m_device->getDevice() , stagingBufferMemory, nullptr);

        KE_OUT(KE_NOARG);
    }    
    
    void K3Model::bind(VkCommandBuffer commandBuffer) {
        //KE_IN(KE_NOARG);
        VkBuffer buffers[] = {m_vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        if(m_hasIndexBuffer) {
            vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
        //KE_OUT(KE_NOARG);
    }

    void K3Model::draw(VkCommandBuffer commandBuffer) {
        //KE_IN(KE_NOARG);
        if(m_hasIndexBuffer) {
            vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
        } else {
            vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
        }
        //KE_OUT(KE_NOARG);
    }


}