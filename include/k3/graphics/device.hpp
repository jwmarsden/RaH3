#pragma once

#include "log.h"

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

    class KeDevice {
    
        public:

#ifndef KE_CONFIG_RELEASE
            const bool enableValidationLayers = true;
#else
            const bool enableValidationLayers = false;
#endif

            KeDevice() = default;

            ~KeDevice() {if(m_initFlag) shutdown();}

            void init(std::shared_ptr<KeWindow> window);

            void shutdown();

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

            bool m_initFlag = false;

            std::shared_ptr<KeWindow> m_window;

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