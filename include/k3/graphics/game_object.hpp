#pragma once

#include "k3/logging/log.hpp"

#include "model.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <string>

namespace k3::graphics {

    struct TransformComponent {

        glm::vec3 translation{};  // (position offset)

        glm::vec3 scale{1.f, 1.f, 1.f};

        glm::vec3 rotation{};

        // Matrix corresponds to translate * Ry * Rx * Rz * scale transformation
        // Tait-bryan with axis order Y, X, Z
        glm::mat4 mat4();

        glm::mat3 normalMatrix();
    };

    class K3GameObject {

        public:
            using id_t = unsigned int;

            static K3GameObject createGameObject() {
                static id_t currentId = 0;
                return K3GameObject(currentId++);
            };

            static K3GameObject createGameObject(std::string name) {
                static id_t currentId = 0;
                return K3GameObject(currentId++, name);
            };

            ~K3GameObject();

            K3GameObject(const K3GameObject &) = delete;
            K3GameObject &operator=(const K3GameObject &) = delete;
            K3GameObject(K3GameObject &&) = default;
            K3GameObject &operator=(K3GameObject &&) = default;

            id_t getId() { return m_id; };

            

            std::shared_ptr<K3Model> model;

            glm::vec3 color{};

            TransformComponent transform{};
        
        private:

            K3GameObject(id_t objId, std::string name);

            K3GameObject(id_t objId);
            
            id_t m_id = -1;

            std::string m_name;
    };
}