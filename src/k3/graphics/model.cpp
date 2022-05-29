#include "k3/graphics/model.hpp"

#include <cassert>
#include <cstring>

namespace k3::graphics {

    void KeModel::init(std::shared_ptr<KeDevice> device, std::vector<Vertex> verticies) {
        KE_IN(KE_NOARG);
        assert(!m_initFlag && "Already had init.");
        m_device = device;
        m_vertices = verticies;
        createVertexBuffers(verticies);
        m_initFlag = true;
        KE_OUT(KE_NOARG);
    }

    void KeModel::shutdown() {
        KE_IN(KE_NOARG);
        assert(m_initFlag && "Must have been init to shutdown.");
        if(m_initFlag) {
            m_initFlag = false;
            vkDestroyBuffer(m_device->getDevice(), m_vertexBuffer, nullptr);
            vkFreeMemory(m_device->getDevice() , m_vertexBufferMemory, nullptr);
            if(m_device != nullptr) {
                m_device = nullptr;
            }
        }
        KE_OUT(KE_NOARG);
    }

    void KeModel::createVertexBuffers(const std::vector<Vertex> verticies) {
        KE_IN(KE_NOARG);
        m_vertexCount = static_cast<uint32_t>(verticies.size());
        assert(m_vertexCount >= 3 && "Vertex Count Must Be At Least 3");
        VkDeviceSize bufferSize = sizeof(verticies[0]) * m_vertexCount;
        m_device->createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            m_vertexBuffer,
            m_vertexBufferMemory);

        void *data;
        vkMapMemory(m_device->getDevice() , m_vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, verticies.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(m_device->getDevice() , m_vertexBufferMemory);
        KE_OUT(KE_NOARG);
    }
    
    void KeModel::bind(VkCommandBuffer commandBuffer) {
        //KE_IN(KE_NOARG);
        VkBuffer buffers[] = {m_vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        //KE_OUT(KE_NOARG);
    }

    void KeModel::draw(VkCommandBuffer commandBuffer) {
        //KE_IN(KE_NOARG);
        vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
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