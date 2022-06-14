#pragma once

#include "k3/logging/log.hpp"

#include "k3/graphics/window.hpp"
#include "k3/graphics/graphics.hpp"

namespace k3::controller {

    class WindowBehaviorController {

        public:

            struct KeyMappings {
                int escapeWindow = GLFW_KEY_ESCAPE;
            };

            WindowBehaviorController(std::shared_ptr<k3::graphics::K3Window> window, std::shared_ptr<k3::graphics::K3Graphics> m_graphics);

            ~WindowBehaviorController();

        private:

            static void windowFocusCallback(GLFWwindow* glfwWindow, int focused);

            static void windowMaximizeCallback(GLFWwindow* glfwWindow, int maximized);

            static void keyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);

            static void cursorPositionCallback(GLFWwindow* glfwWindow, double xpos, double ypos);

            static void mouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods);

            static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

            void registerForWindowEvents();

            std::shared_ptr<k3::graphics::K3Window> m_window = nullptr;

            std::shared_ptr<k3::graphics::K3Graphics> m_graphics = nullptr;

    };

}