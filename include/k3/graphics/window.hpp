#pragma once

#include "log.h"
 
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vulkan/vulkan.h"

#include <string>

namespace k3::graphics {

    class K3Window {

        public:

            K3Window(int width, int height, const std::string& windowName);

            ~K3Window();

            bool shouldClose() { 
                return glfwWindowShouldClose(m_glfw_window); 
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

            GLFWwindow *getGLFWwindow() const { return m_glfw_window; };

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

            int m_width = -1;

            int m_height = -1;

            bool m_framebufferResized = false;

            const std::string m_windowName;

            GLFWwindow *m_glfw_window;

            bool m_inputCaptured = false;

    };

}