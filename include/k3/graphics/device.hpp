#pragma once

#include "k3/logging/log.hpp"

#include "window.hpp"

#include <algorithm>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

namespace k3::graphics {

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        uint32_t graphicsFamily;
        uint32_t presentFamily;
        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;
        bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
    };

    class K3Device {
    
        public:

#if K3_BUILD_TYPE == 1
            const bool enableValidationLayers = true;
#else
            const bool enableValidationLayers = false;
#endif

            K3Device(std::shared_ptr<K3Window> window);

            ~K3Device();

            VkInstance getInstance() { 
                return m_instance; 
            }

            VkDevice getDevice() { 
                return m_device; 
            }

            VkSurfaceKHR getSurface() { 
                return m_surface; 
            }

            uint32_t getGraphicsFamily() {
                return m_graphicsFamily;
            }
            
            VkQueue getGraphicsQueue() { 
                return m_graphicsQueue; 
            }
            
            VkQueue presentQueue() { 
                return m_presentQueue; 
            }
            
            VkDescriptorPool getDescriptorPool() { 
                return m_descriptorPool; 
            }

            VkCommandPool getCommandPool() { 
                return m_commandPool; 
            }

            VkPhysicalDevice getPhysicalDevice() {
                return m_physicalDevice;
            }

            SwapChainSupportDetails getSwapChainSupport() { 
                return querySwapChainSupport(m_physicalDevice); 
            }

            QueueFamilyIndices findPhysicalQueueFamilies() { 
                return findQueueFamilies(m_physicalDevice); 
            }

            VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

            void createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);

            uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

            void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);

            VkCommandBuffer beginSingleTimeCommands();

            void endSingleTimeCommands(VkCommandBuffer commandBuffer);

            void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

            void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

            VkPhysicalDeviceProperties m_vk_properties;

        private:

            void createInstance(std::vector<std::string> requiredInstanceExtensions);

            bool checkValidationLayerSupport();

            std::vector<std::string> getRequiredInstanceExtensions();

            std::vector<std::string> getAvailableInstanceExtensions();

            void hasRequiredInstanceExtensions(std::vector<std::string> &requiredInstanceExtensions);

            void setupDebugMessenger();

            void createSurface();

            std::vector<std::string> selectGPUDevice(std::vector<std::string> &requestPhysicalExtensions);

            void createLogicalDevice(std::vector<std::string> &requestPhysicalExtensions);

            SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
            
            QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

            void createDescriptorPool();

            void createCommandPool();

            std::shared_ptr<K3Window> m_window = nullptr;

            VkInstance m_instance = nullptr; 

            VkDebugUtilsMessengerEXT m_debugMessenger = nullptr;

            VkSurfaceKHR m_surface = nullptr;

            VkPhysicalDevice m_physicalDevice = nullptr;

            VkDevice m_device = nullptr;

            VkDescriptorPool m_descriptorPool = nullptr;

            VkCommandPool m_commandPool = nullptr;

            uint32_t m_graphicsFamily = (uint32_t) -1;

            VkQueue m_graphicsQueue = nullptr;

            uint32_t m_presentFamily = (uint32_t) -1;
        
            VkQueue m_presentQueue = nullptr;

            const std::vector<const char *> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};
    };

}