#pragma once

#include "log.h"
 
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vulkan/vulkan.h"

#include <string>

namespace k3::graphics {

    class KeWindow {
        public:

            KeWindow() = default;

            ~KeWindow() {if(m_initFlag) shutdown();}

            void init(int width, int height, const std::string& windowName);

            void shutdown();

            bool shouldClose() { 
                return glfwWindowShouldClose(m_window); 
            }

            VkExtent2D getExtent() {
                return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)};
            }

            bool wasWindowResized() {
                return m_framebufferResized;
            }

            void resetFramebufferResized() {
                KE_IN(KE_NOARG);
                m_framebufferResized = false;
                KE_OUT(KE_NOARG);
            }

            GLFWwindow *getGLFWwindow() const { return m_window; };

            void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

            void setWindowName(std::string windowName);

            void setWindowUserPointer(void *userPointer);

            void captureInput();

            void releaseInput();

            bool isInputCaptured() { 
                return m_inputCaptured; 
            }

        private:

            static void framebufferResizeCallback(GLFWwindow *glfwWindow, int width, int height);

            bool m_initFlag = false;

            int m_width;

            int m_height;

            bool m_framebufferResized = false;

            std::string m_windowName;

            GLFWwindow *m_window;

            bool m_inputCaptured = false;

    };

}