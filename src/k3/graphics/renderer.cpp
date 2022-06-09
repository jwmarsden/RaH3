#include "k3/graphics/renderer.hpp"

namespace k3::graphics  {

    K3Renderer::K3Renderer(std::shared_ptr<K3Window> window, std::shared_ptr<K3Device> device) : m_window {window}, m_device {device} {
        KE_IN(KE_NOARG);
        
        recreateSwapChain();
        createCommandBuffers();

        KE_OUT(KE_NOARG);
    }

    K3Renderer::~K3Renderer() {
        KE_IN(KE_NOARG);

        freeCommandBuffers();
        if(m_swapChain != nullptr) {
            m_swapChain = nullptr;
        }
        if(m_device != nullptr) {
            m_device = nullptr;
        }
        if(m_window != nullptr) {
            m_window = nullptr;
        }
        
        KE_OUT(KE_NOARG);
    }

    VkCommandBuffer K3Renderer::beginFrame() {
        //KE_IN(KE_NOARG);
        assert(!m_isFrameStarted && "Cant call beginFrame while in progress.");

        auto result = m_swapChain->acquireNextImage(&m_currentImageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }
        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            KE_CRITICAL("Failed to aquire swapchain!");
            throw std::runtime_error("Failed to aquire swapchain!");
        }
        m_isFrameStarted = true;
        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            KE_CRITICAL("Failed to begin recording command buffer!");
            throw std::runtime_error("Failed to begin recording command buffer!");
        }
        //KE_OUT(KE_NOARG);
        return commandBuffer;
    }

    void K3Renderer::endFrame() {
        assert(m_isFrameStarted && "Cant call endFrame while frame is not in progress.");
        auto commandBuffer = getCurrentCommandBuffer();
        if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            KE_CRITICAL("Failed to record comand buffer!");
            throw std::runtime_error("Failed to record comand buffer!");
        }
        auto result = m_swapChain->submitCommandBuffers(&commandBuffer, &m_currentImageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window->wasWindowResized()) {
            if(m_window->wasWindowResized()) {
                KE_DEBUG("Window Resize Triggering Swapchain Recreation");
            } else {
                KE_DEBUG("Command Buffer Submit Triggering Swapchain Recreation: {}", result);
            }
            m_window->resetFramebufferResized();
            recreateSwapChain();
        }
        m_isFrameStarted = false;
        m_currentFrameIndex = (m_currentFrameIndex + 1) % K3SwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void K3Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(m_isFrameStarted && "Cant call beginSwapChainRenderPass while frame is not in progress.");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame.");
        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_swapChain->getRenderPass();
        renderPassBeginInfo.framebuffer = m_swapChain->getFrameBuffer(m_currentImageIndex);

        renderPassBeginInfo.renderArea.offset = {0,0};
        renderPassBeginInfo.renderArea.extent = m_swapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.2f, 0.2f, 0.2f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        //KE_TRACE("Set Clear Values:{}", clearValues[0].color.float32);

        renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size()); 
        renderPassBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        //KE_TRACE("Begin Render Pass");

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_swapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(m_swapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{{0, 0}, m_swapChain->getSwapChainExtent()};
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void K3Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(m_isFrameStarted && "Cant call endSwapChainRenderPass while frame is not in progress.");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame.");
        vkCmdEndRenderPass(commandBuffer);
    }

    void K3Renderer::createCommandBuffers() {
        KE_IN(KE_NOARG);
        m_commandBuffers.resize(K3SwapChain::MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandPool = m_device->getCommandPool();
        commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());    

        if (vkAllocateCommandBuffers(m_device->getDevice() , &commandBufferAllocateInfo, m_commandBuffers.data()) != VK_SUCCESS) {
            KE_CRITICAL("Failed to allocate command buffers.");
            throw std::runtime_error("Failed to allocate command buffers.");
        }
        KE_OUT(KE_NOARG);
    }

    void K3Renderer::freeCommandBuffers() {
        KE_IN(KE_NOARG);
        vkFreeCommandBuffers(m_device->getDevice() , m_device->getCommandPool(), static_cast<uint32_t>(m_commandBuffers.size()),m_commandBuffers.data());
        m_commandBuffers.clear();
        KE_OUT(KE_NOARG);
    }

    void K3Renderer::recreateSwapChain() {
        KE_IN(KE_NOARG);
        auto extent = m_window->getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = m_window->getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(m_device->getDevice() );
        KE_DEBUG("Make new m_swapChain");
        if (m_swapChain == nullptr) {
            m_swapChain = std::make_unique<K3SwapChain>(m_device, extent);
        } else {
            KE_DEBUG("Creating temp swapchain.");
            std::unique_ptr<K3SwapChain> newSwapChain = std::make_unique<K3SwapChain>(m_device, extent, std::move(m_swapChain));
            
            KE_DEBUG("Moving temp swapchain to primary swapchain.");
            m_swapChain = std::move(newSwapChain);
        }
        KE_TRACE("Swapchain Created m_swapChain@<{}>.", fmt::ptr(&m_swapChain));
        KE_OUT(KE_NOARG);
    }

}