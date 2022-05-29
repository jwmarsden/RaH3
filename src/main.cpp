#include "log.h"

#include "k3/logging/log_manager.hpp"

#include "k3/graphics/window.hpp"
#include "k3/graphics/device.hpp"
#include "k3/graphics/graphics.hpp"

#include <exception>
#include <iostream>
#include <array>
#include <chrono>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

std::shared_ptr<k3::logging::LogManger> m_logManger = nullptr;

std::shared_ptr<k3::graphics::KeWindow> m_window = nullptr;

std::shared_ptr<k3::graphics::KeGraphics> m_graphics = nullptr;

void init() {
    m_logManger = std::make_shared<k3::logging::LogManger>();
    m_logManger->init();

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    const std::string WINDOW_NAME = "K3 Activated!";
    m_window = std::make_shared<k3::graphics::KeWindow>();
    m_window->init(WIDTH, HEIGHT, WINDOW_NAME);

    m_graphics = std::make_shared<k3::graphics::KeGraphics>();
    m_graphics->init(m_logManger, m_window);
}

void shutdown() {
    KE_INFO("Kinetic Shutting Down.");

    if(m_graphics != nullptr) {
        m_graphics->shutdown();
        m_graphics = nullptr;
    }

    if(m_window != nullptr) {
        m_window->shutdown();
        KE_TRACE("m_window remaining references: {}. Releasing.", m_window.use_count());
        m_window = nullptr;
    }

    KE_INFO("Kinetic Has Shut Down All Subsystems. Shutting Down Logging.");
    // Leave _logManger last
    if(m_logManger != nullptr) {
        m_logManger->shutdown();
        m_logManger = nullptr;
    }
}

void criticalStop(std::exception_ptr eptr) {
    try {
        if (eptr) {
            std::rethrow_exception(eptr);
        }
    } catch(const std::exception& e) {
        KE_CRITICAL("Critical Error: {}", e.what());
    }
    shutdown();
}

void run() {

    auto device = m_graphics->getDevice();
    auto renderer = m_graphics->getRenderer();
    auto renderSystem = m_graphics->getRenderSystem();
    //auto swapChain = renderer->getSwapChain();

    auto currentTime = std::chrono::high_resolution_clock::now();

    while(!m_window->shouldClose()) {
        // This might block
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        float aspect = renderer->getAspectRatio();
        //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
        //camera.setPerspectiveProjection(glm::pi<float>()/4.f, aspect, 0.1f, 20.f);


        if(auto commandBuffer = renderer->beginFrame()) {

            renderer->beginSwapChainRenderPass(commandBuffer);
            //renderSystem->renderGameObjects(commandBuffer, m_gameObjects, camera);
            renderer->endSwapChainRenderPass(commandBuffer);
            renderer->endFrame();
        }
        //frameCounter++;


        m_graphics->handleUpdate(frameTime);
    }
}

int main() {
    init();
    std::exception_ptr eptr = nullptr;
    try {
        run();
    } catch(...) {
        eptr = std::current_exception();
    }
    if(eptr != nullptr) {
        criticalStop(eptr);
        return EXIT_FAILURE;
    } else {
        shutdown();
        return EXIT_SUCCESS;
    }
}