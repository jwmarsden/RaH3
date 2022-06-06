#include "config.h"

#include "log.h"

#include "k3/logging/log_manager.hpp"

#include "k3/graphics/window.hpp"
#include "k3/graphics/device.hpp"
#include "k3/graphics/graphics.hpp"
#include "k3/graphics/camera.hpp"
#include "k3/graphics/game_object.hpp"

#include "movement_controller.hpp"
#include "window_behavior_controller.hpp"

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

std::vector<k3::graphics::KeGameObject> m_gameObjects;

std::shared_ptr<k3::graphics::KeModel> createCubeModel(std::shared_ptr<k3::graphics::KeGraphics> graphics, glm::vec3 offset) {
    k3::graphics::KeModel::Builder modelBuilder{};

    std::random_device rd;  
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    float r1 = dis(gen); float g1 = dis(gen); float b1 = dis(gen);
    float r2 = dis(gen); float g2 = dis(gen); float b2 = dis(gen);
    float r3 = dis(gen); float g3 = dis(gen); float b3 = dis(gen);
    float r4 = dis(gen); float g4 = dis(gen); float b4 = dis(gen);
    float r5 = dis(gen); float g5 = dis(gen); float b5 = dis(gen);
    float r6 = dis(gen); float g6 = dis(gen); float b6 = dis(gen);

    modelBuilder.verticies = {
        
        // left face
        {{-.5f, -.5f, -.5f}, {r1, g1, b1}},
        {{-.5f, .5f, .5f}, {r1, g1, b1}},
        {{-.5f, -.5f, .5f}, {r1, g1, b1}},
        {{-.5f, -.5f, -.5f}, {r1, g1, b1}},
        {{-.5f, .5f, -.5f}, {r1, g1, b1}},
        {{-.5f, .5f, .5f}, {r1, g1, b1}},
    
        // right face
        {{.5f, -.5f, -.5f}, {r2, g2, b2}},
        {{.5f, .5f, .5f}, {r2, g2, b2}},
        {{.5f, -.5f, .5f}, {r2, g2, b2}},
        {{.5f, -.5f, -.5f}, {r2, g2, b2}},
        {{.5f, .5f, -.5f}, {r2, g2, b2}},
        {{.5f, .5f, .5f}, {r2, g2, b2}},
    
        // top face (remember y axis points down)
        {{-.5f, -.5f, -.5f}, {r3, g3, b3}},
        {{.5f, -.5f, .5f}, {r3, g3, b3}},
        {{-.5f, -.5f, .5f}, {r3, g3, b3}},
        {{-.5f, -.5f, -.5f}, {r3, g3, b3}},
        {{.5f, -.5f, -.5f}, {r3, g3, b3}},
        {{.5f, -.5f, .5f}, {r3, g3, b3}},
    
        // bottom face 
        {{-.5f, .5f, -.5f}, {r4, g4, b4}},
        {{.5f, .5f, .5f}, {r4, g4, b4}},
        {{-.5f, .5f, .5f}, {r4, g4, b4}},
        {{-.5f, .5f, -.5f}, {r4, g4, b4}},
        {{.5f, .5f, -.5f}, {r4, g4, b4}},
        {{.5f, .5f, .5f}, {r4, g4, b4}},
    
        // nose face 
        {{-.5f, -.5f, 0.5f}, {r5, g5, b5}},
        {{.5f, .5f, 0.5f}, {r5, g5, b5}},
        {{-.5f, .5f, 0.5f}, {r5, g5, b5}},
        {{-.5f, -.5f, 0.5f}, {r5, g5, b5}},
        {{.5f, -.5f, 0.5f}, {r5, g5, b5}},
        {{.5f, .5f, 0.5f}, {r5, g5, b5}},
    
        // tail face 
        {{-.5f, -.5f, -0.5f}, {r6, g6, b6}},
        {{.5f, .5f, -0.5f}, {r6, g6, b6}},
        {{-.5f, .5f, -0.5f}, {r6, g6, b6}},
        {{-.5f, -.5f, -0.5f}, {r6, g6, b6}},
        {{.5f, -.5f, -0.5f}, {r6, g6, b6}},
        {{.5f, .5f, -0.5f}, {r6, g6, b6}},
    
    };
    for (auto& v : modelBuilder.verticies) {
        v.position += offset;
    }
    std::shared_ptr<k3::graphics::KeModel> model = std::make_shared<k3::graphics::KeModel>();
    model->init(graphics->getDevice(), modelBuilder);
    return model;
}

void loadGameObjects() {
    KE_IN(KE_NOARG);

    glm::vec3 offset{};

    const int HALF_SIZE = 1;

    for (int i=-HALF_SIZE; i<=HALF_SIZE; i++) {

        for (int j=-HALF_SIZE; j<=HALF_SIZE; j++) {

            for (int k=-HALF_SIZE; k<=HALF_SIZE; k++) {
                std::shared_ptr<k3::graphics::KeModel> model = createCubeModel(m_graphics, offset);

                auto cube = k3::graphics::KeGameObject::createGameObject();
                cube.init();
                cube.model = model;
                cube.transform.translation = {i, j, k+5.f};
                cube.transform.scale = {.5f, .5f, .5f};

                m_gameObjects.push_back(std::move(cube));
            }
        }
    }

    KE_OUT(KE_NOARG);
}

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

    loadGameObjects();
}

void shutdown() {
    KE_INFO("Kinetic Shutting Down.");
    for(auto& gameObject: m_gameObjects) {
        gameObject.shutdown();
    }

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

    k3::graphics::KeCamera camera{};
    camera.setViewTarget(glm::vec3(-20.f,-2.0f, 2.0f), glm::vec3(0.0f, 0.f, 1.5f));

    auto device = m_graphics->getDevice();
    auto renderer = m_graphics->getRenderer();
    auto renderSystem = m_graphics->getRenderSystem();
    //auto swapChain = renderer->getSwapChain();
    
    k3::controller::WindowBehaviorController windowController{};
    windowController.init(m_window, m_graphics);

    auto viewerObject = k3::graphics::KeGameObject::createGameObject();
    viewerObject.init();
    k3::controller::KeyboardMovementController cameraController{};
    cameraController.init();

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

    while(!m_window->shouldClose()) {
        // This might block
        glfwPollEvents();

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

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        // Track frametime
        frameTimeStore[currentFrameTime] = frameTime;
        if(++currentFrameTime >= FRAME_TIME_SIZE) {
            currentFrameTime = 0;

        }

        cameraController.handleMovementInPlaneXZ(m_window, frameTime, viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
        
        float aspect = renderer->getAspectRatio();
        //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
        camera.setPerspectiveProjection(glm::pi<float>()/4.f, aspect, 0.1f, 20.f);

        if(auto commandBuffer = renderer->beginFrame()) {
            renderer->beginSwapChainRenderPass(commandBuffer);

            // Render
            renderSystem->renderGameObjects(commandBuffer, m_gameObjects, camera);
            m_graphics->beginGUIFrameRender(commandBuffer, frameTime);

            ImGuiIO& io = ImGui::GetIO(); (void)io;
            if (io.WantCaptureMouse) {

            }
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
        }
        frameCounter++;
    }
    vkDeviceWaitIdle(device->getDevice());
    cameraController.shutdown();
    viewerObject.shutdown();
    windowController.shutdown();
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