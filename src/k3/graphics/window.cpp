#include "k3/graphics/window.hpp"

#include <cstring>
#include <iostream>
#include <stdexcept>

//#include "ke/graphics/graphics.hpp"

namespace k3::graphics { 

    void KeWindow::init(int width, int height, const std::string& windowName) {
        KE_IN("({},{},{})", width, height, windowName);
        assert(!m_initFlag && "Already had init.");
        m_width = width;
        m_height = height;
        m_windowName = windowName;
        glfwInit();
        // Tell GLFW not to use OpenGL
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
        
        glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);

        m_initFlag = true;

        KE_INFO("Kinetic Created Window (Title \"{}\")", m_windowName.c_str());
        KE_OUT("(): m_window@<{}>", fmt::ptr(&m_window));
    }

    void KeWindow::shutdown() {
        KE_IN(KE_NOARG);
        assert(m_initFlag && "Must have been init to shutdown.");
        if(m_initFlag) {
            m_initFlag = false;
            glfwDestroyWindow(m_window);
            glfwTerminate();
        }
        KE_OUT(KE_NOARG);
    }

    void KeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        KE_IN(KE_NOARG);
        if(glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS) {
            KE_CRITICAL("Failed to Create Window Surface");
            throw std::runtime_error("Failed to Create Window Surface");
        }
        KE_OUT("(): surface@<{}>", fmt::ptr(&surface));
    }

    void KeWindow::framebufferResizeCallback(GLFWwindow *glfwWindow, int width, int height) {
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


    void KeWindow::setWindowUserPointer(void *userPointer) {
        // Bind this to GLFW Window Callback
        glfwSetWindowUserPointer(m_window, userPointer);
    }

    void KeWindow::setWindowName(std::string windowName) {
        glfwSetWindowTitle(m_window, windowName.c_str());
    }

    void KeWindow::captureInput() {
        KE_IN(KE_NOARG); 

        m_inputCaptured = true;

        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        
        //if (glfwRawMouseMotionSupported()) {
        //    glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        //}

        KE_OUT(KE_NOARG); 
    }

    void KeWindow::releaseInput() {
        KE_IN(KE_NOARG); 

        m_inputCaptured = false;

        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        
        //if (glfwRawMouseMotionSupported()) {
        //    glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
        //}

        KE_OUT(KE_NOARG); 
    }

}