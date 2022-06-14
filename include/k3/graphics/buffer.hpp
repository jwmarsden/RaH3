#pragma once

#include "k3/logging/log.hpp"

#include "device.hpp"

#include <memory>

namespace k3::graphics {
 
    class K3Buffer {

        public:
    
            K3Buffer(
                std::shared_ptr<K3Device> device,
                VkDeviceSize instanceSize,
                uint32_t instanceCount,
                VkBufferUsageFlags usageFlags,
                VkMemoryPropertyFlags memoryPropertyFlags,
                VkDeviceSize minOffsetAlignment = 1);

            ~K3Buffer();
    
            K3Buffer(const K3Buffer&) = delete;
            K3Buffer& operator=(const K3Buffer&) = delete;
    
            VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

            void unmap();
    
            void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

            VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

            VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

            VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    
            void writeToIndex(void* data, int index);

            VkResult flushIndex(int index);

            VkDescriptorBufferInfo descriptorInfoForIndex(int index);

            VkResult invalidateIndex(int index);

            VkBuffer getBuffer() const { return m_vk_buffer; }

            void* getMappedMemory() const { return m_mapped; }

            uint32_t getInstanceCount() const { return m_instanceCount; }

            VkDeviceSize getInstanceSize() const { return m_vk_instanceSize; }

            VkDeviceSize getAlignmentSize() const { return m_vk_instanceSize; }

            VkBufferUsageFlags getUsageFlags() const { return m_vk_usageFlags; }

            VkMemoryPropertyFlags getMemoryPropertyFlags() const { return m_vk_memoryPropertyFlags; }

            VkDeviceSize getBufferSize() const { return m_vk_bufferSize; }

        private:

            static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

            std::shared_ptr<K3Device> m_device = nullptr;

            void* m_mapped = nullptr;

            VkBuffer m_vk_buffer = VK_NULL_HANDLE;

            VkDeviceMemory m_vk_memory = VK_NULL_HANDLE;

            VkDeviceSize m_vk_bufferSize;

            uint32_t m_instanceCount;

            VkDeviceSize m_vk_instanceSize;

            VkDeviceSize m_vk_alignmentSize;

            VkBufferUsageFlags m_vk_usageFlags;

            VkMemoryPropertyFlags m_vk_memoryPropertyFlags;

    };
    
}  
 