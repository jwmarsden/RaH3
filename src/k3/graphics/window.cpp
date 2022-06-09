#include "k3/graphics/window.hpp"

#include <cstring>
#include <iostream>
#include <stdexcept>

//#include "ke/graphics/graphics.hpp"

namespace k3::graphics { 

    K3Window::K3Window(int width, int height, const std::string& windowName) : m_width{ width }, m_height{ height }, m_windowName{ windowName } {
        KE_IN("({},{},{})", width, height, windowName);
        glfwInit();
        // Tell GLFW not to use OpenGL
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        m_glfw_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
        
        glfwSetFramebufferSizeCallback(m_glfw_window, framebufferResizeCallback);

        KE_INFO("Kinetic Created Window (Title \"{}\")", m_windowName.c_str());
        KE_OUT("(): m_window@<{}>", fmt::ptr(&m_glfw_window));
    }

    K3Window::~K3Window() {
        KE_IN(KE_NOARG);

        glfwDestroyWindow(m_glfw_window);
        glfwTerminate();

        KE_OUT(KE_NOARG);
    }

    void K3Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        KE_IN(KE_NOARG);
        if(glfwCreateWindowSurface(instance, m_glfw_window, nullptr, surface) != VK_SUCCESS) {
            KE_CRITICAL("Failed to Create Window Surface");
            throw std::runtime_error("Failed to Create Window Surface");
        }
        KE_OUT("(): surface@<{}>", fmt::ptr(&surface));
    }

    void K3Window::framebufferResizeCallback(GLFWwindow *glfwWindow, int width, int height) {
        KE_IN(KE_NOARG);
        /*
        auto graphics = reinterpret_cast<KeGraphics *>(glfwGetWindowUserPointer(glfwWindow));
        auto window = graphics->getWindow();
        window->m_framebufferResized = true;
        window->m_width = width;
        window->m_height = height;
        */
        KE_OUT("(): Window Size({},{})", width, height);
    } 


    void K3Window::setWindowUserPointer(void *userPointer) {
        // Bind this to GLFW Window Callback
        glfwSetWindowUserPointer(m_glfw_window, userPointer);
    }

    void K3Window::setWindowName(std::string windowName) {
        glfwSetWindowTitle(m_glfw_window, windowName.c_str());
    }

    void K3Window::captureInput() {
        KE_IN(KE_NOARG); 

        m_inputCaptured = true;

        glfwSetInputMode(m_glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        
        //if (glfwRawMouseMotionSupported()) {
        //    glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        //}

        KE_OUT(KE_NOARG); 
    }

    void K3Window::releaseInput() {
        KE_IN(KE_NOARG); 

        m_inputCaptured = false;

        glfwSetInputMode(m_glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        
        //if (glfwRawMouseMotionSupported()) {
        //    glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
        //}

        KE_OUT(KE_NOARG); 
    }

}