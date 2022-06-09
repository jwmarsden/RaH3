#include "k3/graphics/device.hpp"

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

namespace k3::graphics { 

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
        KE_WARN("Vulkan[{}] {}", pCallbackData->messageIdNumber, pCallbackData->pMessage);
        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, 
    const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &debugUtilCreateInfo) {
        debugUtilCreateInfo = {};
        debugUtilCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugUtilCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugUtilCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugUtilCreateInfo.pfnUserCallback = debugCallback;
        debugUtilCreateInfo.pUserData = nullptr; 
    }

    K3Device::K3Device(std::shared_ptr<K3Window> window) : m_window{window} {
        KE_IN("(window@<{}>)", fmt::ptr(window));

        // Define Available Extensions
        std::vector<std::string> availableInstanceExtensions = getAvailableInstanceExtensions();

        // Define Instance Extensions
        std::vector<std::string> requiredInstanceExtensions = getRequiredInstanceExtensions();
        // Add VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME if its supported.
        if(std::find(availableInstanceExtensions.begin(), availableInstanceExtensions.end(), VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) != availableInstanceExtensions.end()) {
            KE_DEBUG("Adding \"{}\" to Instance Extensions.", VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
            requiredInstanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        }
        // Add VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME if its supported.
        if(std::find(availableInstanceExtensions.begin(), availableInstanceExtensions.end(), VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) != availableInstanceExtensions.end()) {
            KE_DEBUG("Adding \"{}\" to Instance Extensions.", VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            requiredInstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        }

        createInstance(requiredInstanceExtensions);
        setupDebugMessenger();
        createSurface();

        // Define Device Extensions
        std::vector<std::string> requestDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        std::vector<std::string> availableDeviceExtensions = selectGPUDevice(requestDeviceExtensions);
        // If supported, add VK_KHR_portability_subset
        
        std::string VK_KHR_PORTABILITY_SUBSET = "VK_KHR_portability_subset";

        if(std::find(availableDeviceExtensions.begin(), availableDeviceExtensions.end(), VK_KHR_PORTABILITY_SUBSET) != availableDeviceExtensions.end()) {
            KE_DEBUG("Adding \"{}\" to Device Extensions.", VK_KHR_PORTABILITY_SUBSET);
            requestDeviceExtensions.push_back(VK_KHR_PORTABILITY_SUBSET.c_str());
        } 
        
        createLogicalDevice(requestDeviceExtensions);
        createDescriptorPool();
        createCommandPool();

        KE_OUT(KE_NOARG);
    }

    K3Device::~K3Device() {
        KE_IN(KE_NOARG);

        if (m_commandPool != nullptr) {
            vkDestroyCommandPool(m_device, m_commandPool, nullptr);
            m_commandPool = nullptr;
        }
        if (m_descriptorPool != nullptr) {
            vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
            m_descriptorPool = nullptr;
        }
        if (m_device != nullptr) {
            vkDestroyDevice(m_device, nullptr);
            m_device = nullptr;
        }
        if (enableValidationLayers && m_debugMessenger != nullptr) {
            DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
            m_debugMessenger = nullptr;
        }
        if (m_surface != nullptr) {
            vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
            m_surface = nullptr;
        }
        if (m_instance != nullptr) {
            vkDestroyInstance(m_instance, nullptr);
            m_instance = nullptr;
        }
        m_window = nullptr;

        KE_OUT(KE_NOARG);
    }

    void K3Device::createInstance(std::vector<std::string> requiredInstanceExtensions) {
        KE_IN(KE_NOARG);

        bool validationLayerAvailable = false;
        if(enableValidationLayers) {
            validationLayerAvailable = checkValidationLayerSupport();
        }

        // Create Application Information
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Kinetic Engine App";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Kinetic Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        // Create Instance Information
        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo;
        KE_TRACE("Required instance extensions: {}", requiredInstanceExtensions);
        std::vector<const char*> ptrs;
        for (std::string const& str : requiredInstanceExtensions) {
            ptrs.push_back(str.data());
        }
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(ptrs.size());
        instanceCreateInfo.ppEnabledExtensionNames = ptrs.data();
        if(std::find(requiredInstanceExtensions.begin(), requiredInstanceExtensions.end(), VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) != requiredInstanceExtensions.end()) {
            KE_DEBUG("Adding \"{}\" to VkInstanceCreateInfo.flags", "VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR");
            instanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        } 

        // Check Instance Extensions
        hasRequiredInstanceExtensions(requiredInstanceExtensions);

        KE_DEBUG("Enable Validation Layers: {}",  enableValidationLayers);
        // Enable Validation Layers
        if (enableValidationLayers && validationLayerAvailable) {
            VkDebugUtilsMessengerCreateInfoEXT debugUtilCreateInfo;
            populateDebugMessengerCreateInfo(debugUtilCreateInfo);
            instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugUtilCreateInfo;
            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = m_validationLayers.data();          
        } else {
            instanceCreateInfo.enabledLayerCount = 0;
            instanceCreateInfo.pNext = nullptr;
        }

        KE_DEBUG("Create Instance");
        if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance) != VK_SUCCESS) {
            KE_CRITICAL("Failed to create instance!");
        }

        KE_OUT("(): m_instance@<{}>", fmt::ptr(&m_instance));
    }

    bool K3Device::checkValidationLayerSupport() {
        KE_IN(KE_NOARG);
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        bool layerFound = false;
        for (const char *layerName : m_validationLayers) {
            KE_DEBUG("Checking For Validation Layer: \"{}\"", layerName);
            for (const auto &layerProperties : availableLayers) {
                KE_DEBUG("Checking Available Layer: \"{}\": \"{}\"", layerProperties.layerName, layerProperties.description);
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
        }
        if(layerFound == false) {
            KE_ERROR("Kinetic did not find an available validation layer.");
        }
        KE_OUT("(): {}", layerFound);
        return layerFound;
    }

    std::vector<std::string> K3Device::getRequiredInstanceExtensions() {
        KE_IN(KE_NOARG);
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<std::string> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        KE_OUT(KE_NOARG);
        return extensions;
    }

    std::vector<std::string> K3Device::getAvailableInstanceExtensions() {
        KE_IN(KE_NOARG);
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensionProperties(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensionProperties.data());
        std::vector<std::string> extensions;
        for (const auto &availableExtension : availableExtensionProperties) {
            extensions.push_back(availableExtension.extensionName);
        }
        KE_OUT(KE_NOARG);
        return extensions;
    }

    void K3Device::hasRequiredInstanceExtensions(std::vector<std::string> &requiredInstanceExtensions) {
        KE_IN(KE_NOARG);
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensionProperties(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensionProperties.data());
        std::unordered_set<std::string> availableExtensions;
        for (const auto &availableExtension : availableExtensionProperties) {
            availableExtensions.insert(availableExtension.extensionName);
        }
        KE_TRACE("Available instance extensions: {}", availableExtensions);
        bool extensionsAllFound = true;
        for (const auto &requiredExtension : requiredInstanceExtensions) {
            if (availableExtensions.find(requiredExtension) == availableExtensions.end()) {
                extensionsAllFound = false;
                KE_CRITICAL("Missing Extension: \"{}\"", requiredExtension);
            }
        }
        KE_OUT(KE_NOARG);
    }

    void K3Device::setupDebugMessenger() {
        KE_IN(KE_NOARG);
        if (!enableValidationLayers) {
            return;
        }
        VkDebugUtilsMessengerCreateInfoEXT debugUtilCreateInfo;
        populateDebugMessengerCreateInfo(debugUtilCreateInfo);
        if (CreateDebugUtilsMessengerEXT(m_instance, &debugUtilCreateInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
            KE_CRITICAL("Failed to set up debug messenger!");
        }
        KE_OUT("(): m_debugMessenger@<{}>", fmt::ptr(&m_debugMessenger));
    }

    void K3Device::createSurface() { 
        KE_IN(KE_NOARG);
        m_window->createWindowSurface(m_instance, &m_surface); 
        KE_OUT("(): m_surface@<{}>", fmt::ptr(&m_surface));
    }

    std::vector<std::string> K3Device::selectGPUDevice(std::vector<std::string> &requestPhysicalExtensions) {
        KE_IN(KE_NOARG);
        uint32_t gpuCount = 0;
        vkEnumeratePhysicalDevices(m_instance, &gpuCount, nullptr);
        if (gpuCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        KE_DEBUG("Devices with Vulkan support: {}", gpuCount);
        std::vector<VkPhysicalDevice> gpus(gpuCount);
        vkEnumeratePhysicalDevices(m_instance, &gpuCount, gpus.data());
        VkPhysicalDeviceProperties properties;
        int useGpu = 0;
        for (int i = 0; i < (int) gpuCount; i++) {
            VkPhysicalDevice gpu = gpus[i];
            vkGetPhysicalDeviceProperties(gpu, &properties);
            KE_DEBUG("Checking GPU ({}) Name: \"{}\"", properties.deviceID, properties.deviceName);
            
            // Physical Device Extensions
            uint32_t physicalDeviceExtensionCount;
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &physicalDeviceExtensionCount, nullptr);
            std::vector<VkExtensionProperties> physicalDeviceExtensions(physicalDeviceExtensionCount);
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &physicalDeviceExtensionCount, physicalDeviceExtensions.data());
            
            std::vector<const char *> currentDeviceAvailableExtensions = {};
            for (const auto &physicalDeviceExtension : physicalDeviceExtensions) {
                currentDeviceAvailableExtensions.push_back(physicalDeviceExtension.extensionName);
            }
            KE_TRACE("Available device extensions: {}", currentDeviceAvailableExtensions);

            bool foundAllPhysicalDeviceExtensions = true;
            for (int i=0; i < requestPhysicalExtensions.size(); i++) {
                bool foundPhysicalDeviceExtension = false;
                for (int j = 0; j < (int) physicalDeviceExtensionCount; j++) {
                    VkExtensionProperties physicalDeviceExtension = physicalDeviceExtensions[j];
                    if(std::strcmp(requestPhysicalExtensions[i].data(), physicalDeviceExtension.extensionName)==0) {
                        foundPhysicalDeviceExtension = true;
                        KE_TRACE("Found Support for \"{}\" on device ({}): \"{}\"", physicalDeviceExtension.extensionName, properties.deviceID, properties.deviceName);
                        break;
                    }
                }
                if(!foundPhysicalDeviceExtension) {
                    foundAllPhysicalDeviceExtensions = false;
                    KE_ERROR("Missing Extension: {}", requestPhysicalExtensions[i]);
                }
            }

            VkPhysicalDeviceFeatures supportedFeatures;
            vkGetPhysicalDeviceFeatures(gpu, &supportedFeatures);

            if (foundAllPhysicalDeviceExtensions && properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                useGpu = i;
            }
        }
        m_physicalDevice = gpus[useGpu];

        // Fetch Extensions for Selected Physical Device
        std::vector<std::string> physicalDeviceAvailableExtensions;
        uint32_t physicalDeviceExtensionCount;
        vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &physicalDeviceExtensionCount, nullptr);
        std::vector<VkExtensionProperties> physicalDeviceExtensions(physicalDeviceExtensionCount);
        vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &physicalDeviceExtensionCount, physicalDeviceExtensions.data());
        for (const auto &physicalDeviceExtension : physicalDeviceExtensions) {
            physicalDeviceAvailableExtensions.push_back(physicalDeviceExtension.extensionName);
        }
        vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);

        // Checks and Logs
        if (m_physicalDevice == VK_NULL_HANDLE) {
            KE_CRITICAL("Failed to find GPU!");
        }
        KE_DEBUG("Use Physical GPU ({}): \"{}\"", properties.deviceID, properties.deviceName);
        KE_INFO("Kinetic Selected Physical GPU: \"{}\"", properties.deviceName);
        KE_OUT("(): physicalDeviceAvailableExtensions[{}]@<{}>, m_physicalDevice@<{}>", physicalDeviceAvailableExtensions.size(), fmt::ptr(&physicalDeviceAvailableExtensions), fmt::ptr(&m_physicalDevice));
        return physicalDeviceAvailableExtensions;
    }

    SwapChainSupportDetails K3Device::querySwapChainSupport(VkPhysicalDevice device) {
        KE_IN(KE_NOARG);
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
        }
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
        }
        KE_OUT(KE_NOARG);
        return details;
    }

    QueueFamilyIndices K3Device::findQueueFamilies(VkPhysicalDevice device) {
        KE_IN(KE_NOARG);
        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        int i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
                indices.graphicsFamilyHasValue = true;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
            if (queueFamily.queueCount > 0 && presentSupport) {
                indices.presentFamily = i;
                indices.presentFamilyHasValue = true;
            }
            if (indices.isComplete()) {
                break;
            }
            i++;
        }
        KE_OUT(KE_NOARG);
        return indices;
    }

    void K3Device::createLogicalDevice(std::vector<std::string> &requestPhysicalExtensions) {
        KE_IN(KE_NOARG);
        QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
        
        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        std::vector<const char*> ptrs;
        for (std::string const& str : requestPhysicalExtensions) {
            ptrs.push_back(str.data());
        }
        createInfo.enabledExtensionCount = static_cast<uint32_t>(ptrs.size());
        createInfo.ppEnabledExtensionNames = ptrs.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
            createInfo.ppEnabledLayerNames = m_validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }
        if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
            KE_CRITICAL("failed to create logical device!");
        }

        m_graphicsFamily = indices.graphicsFamily;
        vkGetDeviceQueue(m_device, m_graphicsFamily, 0, &m_graphicsQueue);
        m_presentFamily = indices.presentFamily;
        vkGetDeviceQueue(m_device, m_presentFamily, 0, &m_presentQueue);
        KE_OUT("(): m_device@<{}>, m_graphicsQueue@<{}>, m_presentQueue@<{}>", fmt::ptr(&m_device), fmt::ptr(&m_graphicsQueue), fmt::ptr(&m_presentQueue));
    }

    void K3Device::createDescriptorPool() {
        KE_IN(KE_NOARG);
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        if (vkCreateDescriptorPool(m_device, &pool_info, nullptr, &m_descriptorPool) != VK_SUCCESS) {
            KE_CRITICAL("failed to descriptor pool!");
        }

        KE_OUT("(): m_descriptorPool@<{}>", fmt::ptr(&m_descriptorPool));
    }

    void K3Device::createCommandPool() {
        KE_IN(KE_NOARG);
        QueueFamilyIndices queueFamilyIndices = findPhysicalQueueFamilies();
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
            KE_CRITICAL("failed to create command pool!");
        }
        KE_OUT("(): m_commandPool@<{}>", fmt::ptr(&m_commandPool));
    }

    VkFormat K3Device::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        KE_IN(KE_NOARG);
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);
            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                KE_OUT(KE_NOARG);
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                KE_OUT(KE_NOARG);
                return format;
            }
        }
        KE_CRITICAL("failed to find supported format!");
        throw std::runtime_error("failed to find supported format!");
    }

    void K3Device::createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory) {
        KE_IN(KE_NOARG);
        if (vkCreateImage(m_device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            KE_CRITICAL("failed to create image!");
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            KE_CRITICAL("failed to allocate image memory!");
            throw std::runtime_error("failed to allocate image memory!");
        }

        if (vkBindImageMemory(m_device, image, imageMemory, 0) != VK_SUCCESS) {
            KE_CRITICAL("failed to bind image memory!");
            throw std::runtime_error("failed to bind image memory!");
        }
        KE_OUT(KE_NOARG);
    }

    uint32_t K3Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        KE_IN(KE_NOARG);
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                KE_OUT(KE_NOARG);
                return i;
            }
        }
        KE_CRITICAL("failed to find suitable memory type!");
        throw std::runtime_error("failed to find suitable memory type!");
    }


    void K3Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory) {
        KE_IN(KE_NOARG);
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
        KE_OUT("(): bufferMemory@<{}>", fmt::ptr(&bufferMemory));
    }

    VkCommandBuffer K3Device::beginSingleTimeCommands() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
    }

    void K3Device::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_graphicsQueue);

        vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
    }

    void K3Device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;  // Optional
        copyRegion.dstOffset = 0;  // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }
}