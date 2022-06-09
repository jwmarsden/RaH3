#include "k3/controller/window_behavior_controller.hpp"

namespace k3::controller {

    void WindowBehaviorController::init(std::shared_ptr<graphics::K3Window> window, std::shared_ptr<graphics::KeGraphics> graphics) {
        KE_IN(KE_NOARG);
        assert(!m_initFlag && "Already had init.");

        m_initFlag = true;
        m_window = window;
        m_graphics = graphics;

        registerForWindowEvents();

        KE_OUT(KE_NOARG);
    }

    void WindowBehaviorController::shutdown() {
        KE_IN(KE_NOARG); 
        assert(m_initFlag && "Must have been init to shutdown.");

        if(m_initFlag) {
            m_initFlag = false;
            if(m_window != nullptr) {
                m_window = nullptr;
            }
            if(m_graphics != nullptr) {
                m_graphics = nullptr;
            }
        }

        KE_OUT(KE_NOARG); 
    }

    void WindowBehaviorController::registerForWindowEvents() {
        KE_IN(KE_NOARG); 

        // Set Key callback
        glfwSetKeyCallback(m_window->getGLFWwindow(), keyCallback);
        // Mouse button callback
        glfwSetMouseButtonCallback(m_window->getGLFWwindow(), &mouseButtonCallback);
        // Set maximise callback
        glfwSetWindowMaximizeCallback(m_window->getGLFWwindow(), &windowMaximizeCallback);
        // Set focus callback
        //glfwSetWindowFocusCallback(m_window->getGLFWwindow(), &windowFocusCallback);
        // Mouse move callback
        //glfwSetCursorPosCallback(m_window->getGLFWwindow(), &cursorPositionCallback);
        
        glfwSetScrollCallback(m_window->getGLFWwindow(), scrollCallback);

        KE_OUT(KE_NOARG); 
    }

    void WindowBehaviorController::keyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
        auto graphics = reinterpret_cast<k3::graphics::KeGraphics *>(glfwGetWindowUserPointer(glfwWindow));
        auto window = graphics->getWindow();

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            KE_DEBUG("Kinetic Escape Detected!");
            window->releaseInput();
        }
    }

    void WindowBehaviorController::mouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods) {
        auto graphics = reinterpret_cast<k3::graphics::KeGraphics *>(glfwGetWindowUserPointer(glfwWindow));
        auto window = graphics->getWindow();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.AddMouseButtonEvent(button, (action == GLFW_PRESS) ? true : false);
        
        if (!io.WantCaptureMouse && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            KE_DEBUG("Kinetic Window Mouse Left Click ({},{},{})!", button, action, mods);
            window->captureInput();
        }
    }

    void WindowBehaviorController::windowFocusCallback(GLFWwindow* glfwWindow, int focused) {
        auto graphics = reinterpret_cast<k3::graphics::KeGraphics *>(glfwGetWindowUserPointer(glfwWindow));
        auto window = graphics->getWindow();

        if (focused) {
            KE_DEBUG("Kinetic Window Focus!");
            window->captureInput();
        } else {
            KE_DEBUG("Kinetic Window Out of Focus!");
            window->releaseInput();
        }
    }

    void WindowBehaviorController::windowMaximizeCallback(GLFWwindow* glfwWindow, int maximized) {
        auto graphics = reinterpret_cast<k3::graphics::KeGraphics *>(glfwGetWindowUserPointer(glfwWindow));
        auto window = graphics->getWindow();

        if (maximized) {
            KE_DEBUG("Kinetic Window Maximized!");
            window->captureInput();
        } else {
            KE_DEBUG("Kinetic Window Restored!");
            window->captureInput();
        }
    }

    void WindowBehaviorController::cursorPositionCallback(GLFWwindow* glfwWindow, double xpos, double ypos) {
        auto graphics = reinterpret_cast<k3::graphics::KeGraphics *>(glfwGetWindowUserPointer(glfwWindow));
        auto window = graphics->getWindow();

        KE_DEBUG("Kinetic Window Mouse Move ({},{})!", xpos, ypos);
    }


    void WindowBehaviorController::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        KE_DEBUG("Kinetic Window Scroll({},{})!", xoffset, yoffset);
    }


}