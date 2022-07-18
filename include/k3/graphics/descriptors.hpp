#pragma once

 
#include "device.hpp"
 
// std
#include <memory>
#include <unordered_map>
#include <vector>
 
namespace k3::graphics {
 
    class K3DescriptorSetLayout {

        public:
            
            class Builder {
                
                public:
                    
                    Builder(std::shared_ptr<K3Device> device) : m_device{device} {}

                    Builder &addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);

                    std::unique_ptr<K3DescriptorSetLayout> build() const;

                private:

                    std::shared_ptr<K3Device> m_device;
                    
                    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_vk_bindings{};
            };

            K3DescriptorSetLayout(std::shared_ptr<K3Device> device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);

            ~K3DescriptorSetLayout();

            K3DescriptorSetLayout(const K3DescriptorSetLayout &) = delete;
            K3DescriptorSetLayout &operator=(const K3DescriptorSetLayout &) = delete;

            VkDescriptorSetLayout getDescriptorSetLayout() const { return m_vk_descriptorSetLayout; }

            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_vk_bindings;

        private:

            std::shared_ptr<K3Device> m_device;

            VkDescriptorSetLayout m_vk_descriptorSetLayout;
            
        //friend class L3DescriptorWriter;
    };

    /*****************************************************************************************/


    class K3DescriptorPool {

        public:
        
            class Builder {
        
                public:
        
                    Builder(std::shared_ptr<K3Device> device) : m_device{device} {}

                    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
                    
                    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
                    
                    Builder &setMaxSets(uint32_t count);
                    
                    std::unique_ptr<K3DescriptorPool> build() const;

                private:
                    
                    std::shared_ptr<K3Device> m_device;

                    std::vector<VkDescriptorPoolSize> m_vk_poolSizes{};
                    
                    uint32_t m_maxSets = 1000;
                    
                    VkDescriptorPoolCreateFlags m_vk_poolFlags = 0;

            };

            K3DescriptorPool(std::shared_ptr<K3Device> device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize> &poolSizes);
            
            ~K3DescriptorPool();

            K3DescriptorPool(const K3DescriptorPool &) = delete;
            K3DescriptorPool &operator=(const K3DescriptorPool &) = delete;

            bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

            void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

            void resetPool();

             std::shared_ptr<K3Device> m_device;

        private:

           

            VkDescriptorPool m_vk_descriptorPool;

        //friend class K3DescriptorWriter;

    };

    /*****************************************************************************************/

    class K3DescriptorWriter {
        
        public:
            
            K3DescriptorWriter(K3DescriptorSetLayout &setLayout, K3DescriptorPool &pool);

            K3DescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
            
            K3DescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

            bool build(VkDescriptorSet &set);

            void overwrite(VkDescriptorSet &set);

        private:

            K3DescriptorSetLayout &m_setLayout;

            K3DescriptorPool &m_pool;
            
            std::vector<VkWriteDescriptorSet> m_writes;

    };

}