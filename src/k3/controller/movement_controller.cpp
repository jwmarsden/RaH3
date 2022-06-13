#include "k3/controller/movement_controller.hpp"

namespace k3::controller {

    void KeyboardMovementController::init() {
        KE_IN(KE_NOARG);
        assert(!m_initFlag && "Already had init.");
        m_initFlag = true;
        KE_OUT(KE_NOARG);
    }

    void KeyboardMovementController::shutdown() {
        KE_IN(KE_NOARG); 
        assert(m_initFlag && "Must have been init to shutdown.");
        if(m_initFlag) {
            m_initFlag = false;
        }
        KE_OUT(KE_NOARG); 
    }


    void KeyboardMovementController::handleMovementInPlaneXZ(std::shared_ptr<k3::graphics::K3Window> window, float dt, k3::graphics::K3GameObject& gameObject) {
        KE_IN_SPAM(KE_NOARG);
        if(window->isInputCaptured()) {
            rotateInPlaneXZ(window, dt, gameObject);
            moveInPlaneXZ(window, dt, gameObject);
        }
        KE_OUT_SPAM(KE_NOARG);
    }

    void KeyboardMovementController::rotateInPlaneXZ(std::shared_ptr<k3::graphics::K3Window> window, float dt, k3::graphics::K3GameObject& gameObject) {
        double xPos, yPos;
        glfwGetCursorPos(window->getGLFWwindow(), &xPos, &yPos);

        float xDelta = xPos - m_xPos;
        float yDelta = yPos - m_yPos;
        glm::vec3 rotate{0};

        // TODO: Mouse Acceleration
        if(sensitivity != 0.f) {
            //KE_DEBUG("Sensitivity: {}", sensitivity);
            rotate.x += ((!invert) ? -1 : 1) * yDelta * sensitivity;
            rotate.y += xDelta * sensitivity;
        } else {
            rotate.x += ((!invert) ? -1 : 1) * yDelta;
            rotate.y += xDelta;
        }


        if(glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
            gameObject.transform.rotation += lookSpeed * dt * rotate;
        }

        // Limit pitch between +/- 15 degrees until this much better
        gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -glm::pi<float>()/9.f, glm::pi<float>()/9.f);
        // Stop rotation > 0 and < 360
        gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

        m_xPos = xPos;
        m_yPos = yPos;
    }

    void KeyboardMovementController::moveInPlaneXZ(std::shared_ptr<k3::graphics::K3Window> window, float dt, k3::graphics::K3GameObject& gameObject) {
        float yaw = gameObject.transform.rotation.y;

        const glm::vec3 forwardDirection{sin(yaw), 0.f, cos(yaw)};
        const glm::vec3 rightDirection{forwardDirection.z, 0.f, -forwardDirection.x};
        const glm::vec3 upDirection{0.f, -1.f, 0.f};

        glm::vec3 moveDirection{0.f};

        if(glfwGetKey(window->getGLFWwindow(), keys.moveForward) == GLFW_PRESS) {
            moveDirection += forwardDirection;
        }
        if(glfwGetKey(window->getGLFWwindow(), keys.moveBackward) == GLFW_PRESS) {
            moveDirection -= forwardDirection;
        }
        if(glfwGetKey(window->getGLFWwindow(), keys.moveRight) == GLFW_PRESS) {
            moveDirection += rightDirection;
        }
        if(glfwGetKey(window->getGLFWwindow(), keys.moveLeft) == GLFW_PRESS) {
            moveDirection -= rightDirection;
        }
        if(glfwGetKey(window->getGLFWwindow(), keys.moveUp) == GLFW_PRESS) {
            moveDirection += upDirection;
        }
        if(glfwGetKey(window->getGLFWwindow(), keys.moveDown) == GLFW_PRESS) {
            moveDirection -= upDirection;
        }

        if(glm::dot(moveDirection, moveDirection) > std::numeric_limits<float>::epsilon()) {
            gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDirection);
        }
    }

}