#include "k3/graphics/descriptors.hpp"

#include <cassert>
#include <stdexcept>
 
namespace k3::graphics {

    K3DescriptorSetLayout::Builder &K3DescriptorSetLayout::Builder::addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count) {
        assert(m_vk_bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        m_vk_bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<K3DescriptorSetLayout> K3DescriptorSetLayout::Builder::build() const {
        return std::make_unique<K3DescriptorSetLayout>(m_device, m_vk_bindings);
    }

    /*****************************************************************************************/
 
    K3DescriptorSetLayout::K3DescriptorSetLayout(std::shared_ptr<K3Device> device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings): m_device {device}, m_vk_bindings{bindings} {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(m_device->getDevice(), &descriptorSetLayoutInfo, nullptr, &m_vk_descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }
 
    K3DescriptorSetLayout::~K3DescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(m_device->getDevice(), m_vk_descriptorSetLayout, nullptr);
    }

    /*****************************************************************************************/

    K3DescriptorPool::Builder &K3DescriptorPool::Builder::addPoolSize(VkDescriptorType descriptorType, uint32_t count) {
        m_vk_poolSizes.push_back({descriptorType, count});
        return *this;
    }
    
    K3DescriptorPool::Builder &K3DescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags) {
        m_vk_poolFlags = flags;
        return *this;
    }

    K3DescriptorPool::Builder &K3DescriptorPool::Builder::setMaxSets(uint32_t count) {
        m_maxSets = count;
        return *this;
    }
    
    std::unique_ptr<K3DescriptorPool> K3DescriptorPool::Builder::build() const {
        return std::make_unique<K3DescriptorPool>(m_device, m_maxSets, m_vk_poolFlags, m_vk_poolSizes);
    }
 
    /*****************************************************************************************/
    
    K3DescriptorPool::K3DescriptorPool(std::shared_ptr<K3Device> device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize> &poolSizes) : m_device{device} {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;
        
        if (vkCreateDescriptorPool(m_device->getDevice(), &descriptorPoolInfo, nullptr, &m_vk_descriptorPool) != VK_SUCCESS) {
                throw std::runtime_error("failed to create descriptor pool!");
        }
    }
        
    K3DescriptorPool::~K3DescriptorPool() {
        vkDestroyDescriptorPool(m_device->getDevice(), m_vk_descriptorPool, nullptr);
    }
    
    bool K3DescriptorPool::allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_vk_descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;
        
        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(m_device->getDevice(), &allocInfo, &descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }
    
    void K3DescriptorPool::freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const {
        vkFreeDescriptorSets(m_device->getDevice(), m_vk_descriptorPool, static_cast<uint32_t>(descriptors.size()), descriptors.data());
    }
    
    void K3DescriptorPool::resetPool() {
        vkResetDescriptorPool(m_device->getDevice(), m_vk_descriptorPool, 0);
    }
 
    /*****************************************************************************************/
 
    K3DescriptorWriter::K3DescriptorWriter(K3DescriptorSetLayout &setLayout, K3DescriptorPool &pool) : m_setLayout{setLayout}, m_pool{pool} {}
    
    K3DescriptorWriter &K3DescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo) {
        assert(m_setLayout.m_vk_bindings.count(binding) == 1 && "Layout does not contain specified binding");
        
        auto &bindingDescription = m_setLayout.m_vk_bindings[binding];
        
        assert( bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");
        
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;
        
        m_writes.push_back(write);
        return *this;
    }
    
    K3DescriptorWriter &K3DescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo) {
        assert(m_setLayout.m_vk_bindings.count(binding) == 1 && "Layout does not contain specified binding");
        
        auto &bindingDescription = m_setLayout.m_vk_bindings[binding];
        
        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");
        
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;
        
        m_writes.push_back(write);
        return *this;
    }
    
    bool K3DescriptorWriter::build(VkDescriptorSet &set) {
        bool success = m_pool.allocateDescriptor(m_setLayout.getDescriptorSetLayout(), set);
        if (!success) {
            return false;
        }
        overwrite(set);
        return true;
    }
    
    void K3DescriptorWriter::overwrite(VkDescriptorSet &set) {
        for (auto &write : m_writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(m_pool.m_device->getDevice(), m_writes.size(), m_writes.data(), 0, nullptr);
    }
 
}