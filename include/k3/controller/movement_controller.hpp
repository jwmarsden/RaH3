#pragma once

#include "k3/logging/log.hpp"

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

            KeyboardMovementController();

            ~KeyboardMovementController();

            void handleMovementInPlaneXZ(std::shared_ptr<k3::graphics::K3Window> window, float dt, k3::graphics::K3GameObject& gameObject);

            void rotateInPlaneXZ(std::shared_ptr<k3::graphics::K3Window> window, float dt, k3::graphics::K3GameObject& gameObject);

            void moveInPlaneXZ(std::shared_ptr<k3::graphics::K3Window> window, float dt, k3::graphics::K3GameObject& gameObject);

            KeyMappings keys{};

            float moveSpeed{3.f};

            float lookSpeed{2.f};

            float sensitivity = 1.f;

            bool invert = false;

        private:

            double m_xPos = 0.0;
            
            double m_yPos = 0.0;

    };

}