#pragma once

#include "log.h"
#include "k3/logging/log_manager.hpp"

#include "k3/graphics/window.hpp"
#include "k3/graphics/game_object.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace k3::controller {

    class KeyboardMovementController {

        public:

            struct KeyMappings {
                int moveLeft = GLFW_KEY_A;
                int moveRight = GLFW_KEY_D;
                int moveForward = GLFW_KEY_W;
                int moveBackward = GLFW_KEY_S;
                int moveUp = GLFW_KEY_E;
                int moveDown = GLFW_KEY_Q;
            };

            KeyboardMovementController() = default;

            ~KeyboardMovementController() {if(m_initFlag) shutdown();}

            void init();

            void shutdown();

            void handleMovementInPlaneXZ(std::shared_ptr<k3::graphics::KeWindow> window, float dt, k3::graphics::KeGameObject& gameObject);

            void rotateInPlaneXZ(std::shared_ptr<k3::graphics::KeWindow> window, float dt, k3::graphics::KeGameObject& gameObject);

            void moveInPlaneXZ(std::shared_ptr<k3::graphics::KeWindow> window, float dt, k3::graphics::KeGameObject& gameObject);

            KeyMappings keys{};

            float moveSpeed{4.f};

            float lookSpeed{3.f};

        private:

            bool m_initFlag = false;

            double m_xPos = 0.0;
            
            double m_yPos = 0.0;

    };

}