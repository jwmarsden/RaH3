#pragma once

#include "log.h"

#include "window.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace k3::graphics {
 
    class KeRenderer {

        public: 

            KeRenderer() = default;

            ~KeRenderer() {if(m_initFlag) shutdown();}

            void init(std::shared_ptr<KeWindow> window, std::shared_ptr<KeDevice> device);

            void shutdown();

            bool isFrameInProgress() const {return m_isFrameStarted; }

            //KeSwapChain getSwapChain() const {return *m_swapChain.get(); };

            uint32_t getSwapChainImageCount() const {
                return m_swapChain->imageCount();
            }

            VkRenderPass getSwapChainRenderPass() const {return m_swapChain->getRenderPass(); };

            float getAspectRatio() const { return m_swapChain->extentAspectRatio(); }

            VkCommandBuffer getCurrentCommandBuffer() const {
                assert(m_isFrameStarted && "Cannot get command buffer when frame not in progress");
                return m_commandBuffers[m_currentFrameIndex];
            };

            int getFrameIndex() const {
                assert(m_isFrameStarted && "Cannot get frame index when frame not in progress");
                return m_currentFrameIndex;    
            }

            VkCommandBuffer beginFrame();

            void endFrame();

            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);

            void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

        private: 

            void createCommandBuffers();

            void freeCommandBuffers();

            void recreateSwapChain();

            bool m_initFlag = false;

            std::shared_ptr<KeWindow> m_window;

            std::shared_ptr<KeDevice> m_device;

            std::unique_ptr<KeSwapChain> m_swapChain;

            std::vector<VkCommandBuffer> m_commandBuffers;

            uint32_t m_currentImageIndex;

            int m_currentFrameIndex;

            bool m_isFrameStarted;

    };

}