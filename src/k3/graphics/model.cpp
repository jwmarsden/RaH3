#include "k3/graphics/model.hpp"

#include <cassert>
#include <cstring>

namespace k3::graphics {

    KeModel::KeModel(std::shared_ptr<KeDevice> device, const KeModel::Builder &builder) : m_device {device}, m_builder {builder} {
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

    KeModel::~KeModel() {
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

    void KeModel::createVertexBuffers(const std::vector<Vertex> &vertices) {
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

    void KeModel::createIndexBuffers(const std::vector<uint32_t> &indices) {
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
    
    void KeModel::bind(VkCommandBuffer commandBuffer) {
        //KE_IN(KE_NOARG);
        VkBuffer buffers[] = {m_vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        if(m_hasIndexBuffer) {
            vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
        //KE_OUT(KE_NOARG);
    }

    void KeModel::draw(VkCommandBuffer commandBuffer) {
        //KE_IN(KE_NOARG);
        if(m_hasIndexBuffer) {
            vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
        } else {
            vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
        }
        //KE_OUT(KE_NOARG);
    }

    std::vector<VkVertexInputBindingDescription> KeModel::Vertex::getBindingDescriptions() {
        KE_IN(KE_NOARG);
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        KE_OUT(KE_NOARG);
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> KeModel::Vertex::getAttributeDescriptions() {
        KE_IN(KE_NOARG);
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        KE_OUT(KE_NOARG);
        return attributeDescriptions;
    }

}