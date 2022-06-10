#include "k3/graphics/buffer.hpp"
 
#include <cassert>
#include <cstring>
 
namespace k3::graphics {
 
    /**
     * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
     *
     * @param instanceSize The size of an instance
     * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
     * minUniformBufferOffsetAlignment)
     *
     * @return VkResult of the buffer mapping call
     */
    VkDeviceSize K3Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
    if (minOffsetAlignment > 0) {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
    }
    
    K3Buffer::K3Buffer(
            std::shared_ptr<K3Device> device,
            VkDeviceSize instanceSize,
            uint32_t instanceCount,
            VkBufferUsageFlags usageFlags,
            VkMemoryPropertyFlags memoryPropertyFlags,
            VkDeviceSize minOffsetAlignment)
            : m_device{device},
            m_vk_instanceSize{instanceSize},
            m_instanceCount{instanceCount},
            m_vk_usageFlags{usageFlags},
            m_vk_memoryPropertyFlags{memoryPropertyFlags} {
        m_vk_alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
        m_vk_bufferSize = m_vk_alignmentSize * instanceCount;
        device->createBuffer(m_vk_bufferSize, m_vk_usageFlags, m_vk_memoryPropertyFlags, m_vk_buffer, m_vk_memory);
    }
    
    K3Buffer::~K3Buffer() {
        unmap();
        vkDestroyBuffer(m_device->getDevice(), m_vk_buffer, nullptr);
        vkFreeMemory(m_device->getDevice(), m_vk_memory, nullptr);
    }
    
    /**
     * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
     *
     * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
     * buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the buffer mapping call
     */
    VkResult K3Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
        assert(m_vk_buffer && m_vk_memory && "Called map on buffer before create");
        return vkMapMemory(m_device->getDevice(), m_vk_memory, offset, size, 0, &m_mapped);
    }
    
    /**
     * Unmap a mapped memory range
     *
     * @note Does not return a result as vkUnmapMemory can't fail
     */
    void K3Buffer::unmap() {
        if (m_mapped) {
            vkUnmapMemory(m_device->getDevice(), m_vk_memory);
            m_mapped = nullptr;
        }
    }
    
    /**
     * Copies the specified data to the mapped buffer. Default value writes whole buffer range
     *
     * @param data Pointer to the data to copy
     * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
     * range.
     * @param offset (Optional) Byte offset from beginning of mapped region
     *
     */
    void K3Buffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
        assert(m_mapped && "Cannot copy to unmapped buffer");
        
        if (size == VK_WHOLE_SIZE) {
            memcpy(m_mapped, data, m_vk_bufferSize);
        } else {
            char *memOffset = (char *)m_mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }
    
    /**
     * Flush a memory range of the buffer to make it visible to the device
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
     * complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the flush call
     */
    VkResult K3Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_vk_memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(m_device->getDevice(), 1, &mappedRange);
    }
    
    /**
     * Invalidate a memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
     * the complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the invalidate call
     */
    VkResult K3Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_vk_memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(m_device->getDevice(), 1, &mappedRange);
    }
    
    /**
     * Create a buffer info descriptor
     *
     * @param size (Optional) Size of the memory range of the descriptor
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkDescriptorBufferInfo of specified offset and range
     */
    VkDescriptorBufferInfo K3Buffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
        return VkDescriptorBufferInfo{
            m_vk_buffer,
            offset,
            size,
        };
    }
    
    /**
     * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
     *
     * @param data Pointer to the data to copy
     * @param index Used in offset calculation
     *
     */
    void K3Buffer::writeToIndex(void *data, int index) {
        writeToBuffer(data, m_vk_instanceSize, index * m_vk_alignmentSize);
    }
    
    /**
     *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
     *
     * @param index Used in offset calculation
     *
     */
    VkResult K3Buffer::flushIndex(int index) { 
        return flush(m_vk_alignmentSize, index * m_vk_alignmentSize); 
    }
    
    /**
     * Create a buffer info descriptor
     *
     * @param index Specifies the region given by index * alignmentSize
     *
     * @return VkDescriptorBufferInfo for instance at index
     */
    VkDescriptorBufferInfo K3Buffer::descriptorInfoForIndex(int index) {
        return descriptorInfo(m_vk_alignmentSize, index * m_vk_alignmentSize);
    }
    
    /**
     * Invalidate a memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param index Specifies the region to invalidate: index * alignmentSize
     *
     * @return VkResult of the invalidate call
     */
    VkResult K3Buffer::invalidateIndex(int index) {
        return invalidate(m_vk_alignmentSize, index * m_vk_alignmentSize);
    }
 
} 