#include "config.h"

#include "k3/logging/log.hpp"

#include "k3/graphics/window.hpp"
#include "k3/graphics/device.hpp"
#include "k3/graphics/buffer.hpp"
#include "k3/graphics/graphics.hpp"
#include "k3/graphics/camera.hpp"
#include "k3/graphics/frame_info.hpp"
#include "k3/graphics/game_object.hpp"

#include "k3/controller/movement_controller.hpp"
#include "k3/controller/window_behavior_controller.hpp"

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

std::shared_ptr<k3::graphics::K3Window> m_window = nullptr;

std::shared_ptr<k3::graphics::K3Graphics> m_graphics = nullptr;

std::vector<k3::graphics::K3GameObject> m_gameObjects;


struct GlobalUbo {
    glm::mat4 projectionView {1.f};
    glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f,-3.f,-1.f});
};

void loadGameObjects() {
    KE_IN(KE_NOARG);

    glm::vec3 offset{};

    std::shared_ptr<k3::graphics::K3Model> model = k3::graphics::K3Model::createModelFromFile(m_graphics->getDevice(), "models/tea.obj");

    k3::graphics::K3GameObject gameObject = k3::graphics::K3GameObject::createGameObject("teapot");
    gameObject.model = model;
    gameObject.transform.translation = {0.f, 0.5f, 5.f};
    gameObject.transform.scale = {.5f, .5f, .5f};

    m_gameObjects.push_back(std::move(gameObject));

    KE_OUT(KE_NOARG);
}

void init() {
    k3::logging::LogManger::getInstance().initialise();

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    const std::string WINDOW_NAME = "K3 Activated!";
    m_window = std::make_shared<k3::graphics::K3Window>(WIDTH, HEIGHT, WINDOW_NAME);

    m_graphics = std::make_shared<k3::graphics::K3Graphics>(m_logManger, m_window);

    loadGameObjects();
}

void shutdown() {
    KE_INFO("Kinetic Shutting Down.");

    if(!m_gameObjects.empty()) {
        m_gameObjects.clear();
    }

    if(m_graphics != nullptr) {
        m_graphics = nullptr;
    }

    if(m_window != nullptr) {
        KE_TRACE("m_window remaining references: {}. Releasing.", m_window.use_count());
        m_window = nullptr;
    }

    KE_INFO("Kinetic Has Shut Down All Subsystems. Exit Logging.");

    k3::logging::LogManger::getInstance().shutdown();
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

    k3::graphics::K3Buffer globalUboBuffer {
        m_graphics->getDevice(),
        sizeof(GlobalUbo),
        k3::graphics::K3SwapChain::MAX_FRAMES_IN_FLIGHT,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        device->m_vk_properties.limits.minUniformBufferOffsetAlignment
    };
    globalUboBuffer.map();

    k3::graphics::K3Camera camera{};
    camera.setViewTarget(glm::vec3(-20.f,-2.0f, 2.0f), glm::vec3(0.0f, 0.f, 1.5f));

    k3::controller::WindowBehaviorController windowController{m_window, m_graphics};

    auto viewerObject = k3::graphics::K3GameObject::createGameObject("camera");
    k3::controller::KeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

    // Store frametime average
    int fps = 0;
    float avgRenderTime = 0;
    const int FRAME_TIME_SIZE = 100;
    int currentFrameTime = 0;
    float frameTimeStore[FRAME_TIME_SIZE];
    for(int i=0;i<FRAME_TIME_SIZE;i++) {
        frameTimeStore[i] = -1.f;
    }
    const float FRAME_TIME_SIZE_FLOAT = static_cast<float>(FRAME_TIME_SIZE);

    uint32_t frameCounter = 0;

    KE_TRACE("Enter Game Loop {}", frameCounter);
    while(!m_window->shouldClose()) {

        // This might block
        glfwPollEvents();
        KE_TRACE_SPAM("GLFW Polled Events {}", frameCounter);

        // Check GUI Updates - Update the fps every 100 frames. 
        if(frameCounter%200 == 0) {
            avgRenderTime = 0;
            for(int i=0;i<FRAME_TIME_SIZE;i++) {
                if(frameTimeStore[i] == -1.f) {
                    avgRenderTime = 0;
                    break;
                }
                avgRenderTime += frameTimeStore[i];
            }
            if(avgRenderTime > 0 && FRAME_TIME_SIZE > 0) {
                fps = 1.f / (avgRenderTime/FRAME_TIME_SIZE_FLOAT);
            }
        }
        KE_TRACE_SPAM("Calculated GUI Updates {}", frameCounter);

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        // Track frametime
        frameTimeStore[currentFrameTime] = frameTime;
        if(++currentFrameTime >= FRAME_TIME_SIZE) {
            currentFrameTime = 0;
        }
        KE_TRACE_SPAM("Stored Time {}", frameCounter);

        cameraController.handleMovementInPlaneXZ(m_window, frameTime, viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        KE_TRACE_SPAM("Moved Camera {}", frameCounter);
        
        float aspect = renderer->getAspectRatio();
        //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
        camera.setPerspectiveProjection(glm::pi<float>()/4.f, aspect, 0.1f, 20.f);
        KE_TRACE_SPAM("Set Projection {}", frameCounter);

        if(auto commandBuffer = renderer->beginFrame()) {
            KE_TRACE_SPAM("Enter Frame {}", frameCounter);
            int frameIndex = renderer->getFrameIndex();
            k3::graphics::K3FrameInfo frameInfo {
                frameIndex,
                frameTime,
                commandBuffer,
                camera,
            };

            // Update
            GlobalUbo ubo{};
            ubo.projectionView = camera.getProjection() * camera.getView();
            globalUboBuffer.writeToBuffer(&ubo, frameIndex);
            globalUboBuffer.flushIndex(frameIndex);

            // Render
            renderer->beginSwapChainRenderPass(commandBuffer);
            renderSystem->renderGameObjects(frameInfo, m_gameObjects);
            m_graphics->beginGUIFrameRender(commandBuffer, frameTime);

            ImGuiIO& io = ImGui::GetIO(); (void)io;
            ImGui::Text("Average Render %.2f ms (%d fps)", avgRenderTime, fps);
            ImGui::PlotLines("Times", frameTimeStore, IM_ARRAYSIZE(frameTimeStore), ((currentFrameTime+1>=FRAME_TIME_SIZE) ? 0 : currentFrameTime+1));
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0,0.6,0.8,1), "Mouse Settings");
            ImGui::SliderFloat("Sensitivity", &cameraController.sensitivity, 0.f, 20.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
            ImGui::Checkbox("Invert", &cameraController.invert);
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0,0.6,0.8,1), "Control Settings");
            ImGui::SliderFloat("Walk Speed", &cameraController.moveSpeed, 1.f, 20.0f, "%.4f");
            m_graphics->endGUIFrameRender(commandBuffer, frameTime);

            renderer->endSwapChainRenderPass(commandBuffer);
            renderer->endFrame();
            KE_TRACE_SPAM("Exit Frame {}", frameCounter);
        }
        frameCounter++;
    }
    vkDeviceWaitIdle(device->getDevice());
    KE_INFO("Vulkan Device Idle. Exiting.");
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
        std::cout << "KBye." << std::endl;
        return EXIT_SUCCESS;
    }
}