#pragma once

#include "config.h"

#include "k3/logging/log.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "window.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "pipeline.hpp"
#include "model.hpp"
#include "simple_render_system.hpp"
#include "camera.hpp"
#include "game_object.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <cassert>
#include <random>

namespace k3::graphics {
 
    class K3Graphics {

        public: 

            K3Graphics(std::shared_ptr<logging::LogManger> logManager, std::shared_ptr<K3Window> window);

            ~K3Graphics();

            void handleUpdate(float deltaTime);

            std::shared_ptr<K3Window> getWindow() {return m_window;};

            std::shared_ptr<K3Device> getDevice() {return m_device;};

            std::shared_ptr<K3SimpleRenderSystem> getRenderSystem() {return m_renderSystem;};

            std::shared_ptr<K3Renderer> getRenderer() {return m_renderer;};

            void beginGUIFrameRender(VkCommandBuffer commandBuffer, float deltaTime);

            void endGUIFrameRender(VkCommandBuffer commandBuffer, float deltaTime);

        private:

            std::shared_ptr<logging::LogManger> m_logManger;

            std::shared_ptr<K3Window> m_window = nullptr;

            std::shared_ptr<K3Device> m_device = nullptr;

            std::shared_ptr<K3SimpleRenderSystem> m_renderSystem = nullptr;

            std::shared_ptr<K3Renderer> m_renderer = nullptr;

            ImGui_ImplVulkanH_Window g_MainWindowData {};
    };
}