#include "k3/graphics/graphics.hpp"

namespace k3::graphics  {
    
    static void check_vk_result(VkResult err) {
        if (err == 0) {
            return;
        }  
        fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
        KE_CRITICAL("Vulkan Error: {}", err);
        if (err < 0) {
            abort();
        }   
    }

    KeGraphics::KeGraphics(std::shared_ptr<logging::LogManger> logManager, std::shared_ptr<KeWindow> window) {
        KE_INFO("Kinetic Init {}.{}.{}",PROJECT_VER_MAJOR,PROJECT_VER_MINOR,PROJECT_VER_PATCH);
        m_logManger = logManager;
        KE_TRACE("Trace Logging On.");
        KE_DEBUG("Debug Logging On.");

        m_window = window;
        m_window->setWindowUserPointer(this);

        m_device = std::make_shared<KeDevice>(m_window);

        KE_INFO("Kinetic has connected to the Vulkan.");

        m_renderer = std::make_shared<KeRenderer>();
        m_renderer->init(m_window, m_device);
        VkRenderPass renderPass = m_renderer->getSwapChainRenderPass();
 
        uint32_t minImageCount = 2;
        uint32_t imageCount = (uint32_t) m_renderer->getSwapChainImageCount();

        int w, h;
        glfwGetFramebufferSize(m_window->getGLFWwindow(), &w, &h);
        ImGui_ImplVulkanH_Window* imgGUIWindow = &g_MainWindowData;

        imgGUIWindow->Surface = m_device->getSurface();
    
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        
        ImGui_ImplGlfw_InitForVulkan(m_window->getGLFWwindow(), true);
        ImGui_ImplVulkan_InitInfo imguiInit = {};
        imguiInit.Instance = m_device->getInstance();
        imguiInit.PhysicalDevice = m_device->getPhysicalDevice();
        imguiInit.Device = m_device->getDevice();
        imguiInit.QueueFamily = m_device->getGraphicsFamily();
        imguiInit.Queue = m_device->getGraphicsQueue();
        imguiInit.DescriptorPool = m_device->getDescriptorPool();
        imguiInit.Subpass = 0;
        imguiInit.MinImageCount = minImageCount;
        imguiInit.ImageCount = imageCount;
        imguiInit.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        imguiInit.Allocator = nullptr;
        imguiInit.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&imguiInit, renderPass);

        VkResult err;
        // Upload Fonts
        {
            VkCommandPool commandPool = m_device->getCommandPool();
            std::vector<VkCommandBuffer> m_commandBuffers;
            m_commandBuffers.resize(1);
            VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
            commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandBufferAllocateInfo.commandPool = m_device->getCommandPool();
            commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(1);    
            if (vkAllocateCommandBuffers(m_device->getDevice() , &commandBufferAllocateInfo, m_commandBuffers.data()) != VK_SUCCESS) {
                KE_CRITICAL("Failed to allocate command buffers.");
                throw std::runtime_error("Failed to allocate command buffers.");
            }
            VkCommandBuffer commandBuffer = m_commandBuffers[0];
            if (vkResetCommandPool(m_device->getDevice(), commandPool, 0) != VK_SUCCESS) {
                KE_CRITICAL("Failed Reset Command Pool.");
                throw std::runtime_error("Failed Reset Command Pool.");
            }
            VkCommandBufferBeginInfo begin_info = {};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            if (vkBeginCommandBuffer(commandBuffer, &begin_info) != VK_SUCCESS) {
                KE_CRITICAL("Failed to begin command buffer.");
                throw std::runtime_error("Failed to begin command buffer.");
            }

            ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

            if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
                KE_CRITICAL("Failed to end command buffer.");
                throw std::runtime_error("Failed to end command buffer.");
            }
            VkSubmitInfo end_info = {};
            end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            end_info.commandBufferCount = 1;
            end_info.pCommandBuffers = &commandBuffer;

            if (vkQueueSubmit(m_device->getGraphicsQueue(), 1, &end_info, VK_NULL_HANDLE) != VK_SUCCESS) {
                KE_CRITICAL("Failed to submit command buffer.");
                throw std::runtime_error("Failed to submit command buffer.");
            }
            vkDeviceWaitIdle(m_device->getDevice());
            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }
    
        m_renderSystem = std::make_shared<KeSimpleRenderSystem>(m_device, renderPass);
    }

    KeGraphics::~KeGraphics() {
        KE_IN(KE_NOARG);

        KE_INFO("Kinetic Shutting Down Graphics.");

        vkDeviceWaitIdle(m_device->getDevice());

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        if(m_renderSystem != nullptr) {
            KE_TRACE("m_renderSystem remaining references: {}. Releasing.", m_renderSystem.use_count());
            m_renderSystem = nullptr;
        }

        if(m_renderer != nullptr) {
            KE_TRACE("m_renderer remaining references: {}. Releasing.", m_renderer.use_count());
            m_renderer = nullptr;
        }

        vkDeviceWaitIdle(m_device->getDevice());
        if(m_device != nullptr) {
            KE_TRACE("m_device remaining references: {}. Releasing.", m_device.use_count());
            m_device = nullptr;
        }
        
        if(m_window != nullptr) {
            m_window = nullptr;
        }

        if(m_logManger != nullptr) {
            m_logManger = nullptr;
        }
        
        KE_OUT(KE_NOARG);
    }

    void KeGraphics::beginGUIFrameRender(VkCommandBuffer commandBuffer, float deltaTime) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();


        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(280, 200), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowCollapsed(true, ImGuiCond_Appearing);
        ImGui::Begin(PROJECT_TITLE);
    }

    void KeGraphics::endGUIFrameRender(VkCommandBuffer commandBuffer, float deltaTime) {

        ImGui::End();
        ImGui::EndFrame();

        ImGui::Render();
        ImDrawData* drawData = ImGui::GetDrawData();

        ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
    }

}