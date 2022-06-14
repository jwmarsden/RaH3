#pragma once

#include "k3/logging/log.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <cassert>
#include <limits>

namespace k3::graphics {
 
    class K3Camera {

        public: 

            void setOrthographicProjection (float left, float right, float top, float bottom, float near_plane, float far_plane);

            void setPerspectiveProjection(float fov, float aspect, float near_plane, float far_plane);

            void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3 {0.f, -1.f, 0.f});

            void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3 {0.f, -1.f, 0.f});

            void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

            const glm::mat4& getProjection() const { return m_projectionMatrix; };

            const glm::mat4& getView() const { return m_viewMatrix; };

        private:

            glm::mat4 m_projectionMatrix{1.f};

            glm::mat4 m_viewMatrix{1.f};

    };

}