#include "k3/graphics/graphics.hpp"

namespace k3::graphics  {
    
    static void check_vk_result(VkResult err) {
        if (err == 0) {
            return;
        }  
        fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
        if (err < 0) {
            abort();
        }   
    }

    void KeGraphics::init(std::shared_ptr<logging::LogManger> logManager, std::shared_ptr<KeWindow> window) {
        KE_INFO("Kinetic Init {}.{}.{}",0,0,3);
        assert(!m_initFlag && "Already had init.");
        m_logManger = logManager;
        KE_TRACE("Trace Logging On.");
        KE_DEBUG("Debug Logging On.");

        m_window = window;
        m_window->setWindowUserPointer(this);

        m_device = std::make_shared<KeDevice>();
        m_device->init(m_window);
        KE_INFO("Kinetic has connected to the Vulkan.");

        m_renderer = std::make_shared<KeRenderer>();
        m_renderer->init(m_window, m_device);

        uint32_t minImageCount = 2;
        uint32_t imageCount = (uint32_t) m_renderer->getSwapChainImageCount();

        int w, h;
        glfwGetFramebufferSize(m_window->getGLFWwindow(), &w, &h);
        ImGui_ImplVulkanH_Window* imgGUIWindow = &g_MainWindowData;

        imgGUIWindow->Surface = m_device->getSurface();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui::StyleColorsDark();

        VkRenderPass renderPass = m_renderer->getSwapChainRenderPass();

        ImGui_ImplGlfw_InitForVulkan(m_window->getGLFWwindow(), true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = m_device->getInstance();
        init_info.PhysicalDevice = m_device->getPhysicalDevice();
        init_info.Device = m_device->getDevice();
        init_info.QueueFamily = m_device->getGraphicsFamily();
        init_info.Queue = m_device->getGraphicsQueue();
        //init_info.PipelineCache = g_PipelineCache;
        init_info.DescriptorPool = m_device->getDescriptorPool();
        init_info.Subpass = 0;
        init_info.MinImageCount = minImageCount;
        init_info.ImageCount = imageCount;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info, renderPass);


        VkResult err;
        // Upload Fonts
        {
            // Use any command queue
            VkCommandPool commandPool = m_device->getCommandPool();
            VkCommandBuffer commandBuffer = m_renderer->getSystemCommandBuffer();

            err = vkResetCommandPool(m_device->getDevice(), commandPool, 0);
            check_vk_result(err);

            VkCommandBufferBeginInfo begin_info = {};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            err = vkBeginCommandBuffer(commandBuffer, &begin_info);
            check_vk_result(err);

            ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

            VkSubmitInfo end_info = {};
            end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            end_info.commandBufferCount = 1;
            end_info.pCommandBuffers = &commandBuffer;
            err = vkEndCommandBuffer(commandBuffer);
            check_vk_result(err);
            err = vkQueueSubmit(m_device->getGraphicsQueue(), 1, &end_info, VK_NULL_HANDLE);
            check_vk_result(err);

            err = vkDeviceWaitIdle(m_device->getDevice());
            check_vk_result(err);
            //ImGui_ImplVulkan_DestroyFontUploadObjects();
        }


        m_renderSystem = std::make_shared<KeSimpleRenderSystem>();
        m_renderSystem->init(m_device, renderPass);

        m_initFlag = true;
    }

    void KeGraphics::shutdown() {
        KE_IN(KE_NOARG);
        assert(m_initFlag && "Must have been init to shutdown.");
        if(m_initFlag) {
            m_initFlag = false;
            KE_INFO("Kinetic Shutting Down Graphics.");

            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            if(m_renderSystem != nullptr) {
                m_renderSystem->shutdown();
                KE_TRACE("m_renderSystem remaining references: {}. Releasing.", m_renderSystem.use_count());
                m_renderSystem = nullptr;
            }

            if(m_renderer != nullptr) {
                m_renderer->shutdown();
                KE_TRACE("m_renderer remaining references: {}. Releasing.", m_renderer.use_count());
                m_renderer = nullptr;
            }

            vkDeviceWaitIdle(m_device->getDevice());
            if(m_device != nullptr) {
                m_device->shutdown();
                KE_TRACE("m_device remaining references: {}. Releasing.", m_device.use_count());
                m_device = nullptr;
            }
            
            if(m_window != nullptr) {
                m_window = nullptr;
            }

            if(m_logManger != nullptr) {
                m_logManger = nullptr;
            }
        }
        KE_OUT(KE_NOARG);
    }

    void KeGraphics::handleUpdate(float deltaTime) {
        //KE_IN("(deltaTime: {})", deltaTime);
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        
        ImGui::End();
        
        ImGuiIO& io = ImGui::GetIO();
        
        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
        //const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
        /*
        if (!is_minimized)
        {
            wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
            wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
            wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
            wd->ClearValue.color.float32[3] = clear_color.w;
            FrameRender(wd, draw_data);
            FramePresent(wd);
        }
        */
        //KE_OUT(KE_NOARG);
    }

}