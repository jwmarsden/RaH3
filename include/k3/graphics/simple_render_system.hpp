#pragma once

#include "log.h"

#include "device.hpp"
#include "pipeline.hpp"
#include "camera.hpp"
#include "game_object.hpp"

#include <cassert>
#include <memory>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace k3::graphics {

    class K3SimpleRenderSystem {
        public:

            K3SimpleRenderSystem(std::shared_ptr<K3Device> device, VkRenderPass renderPass);

            ~K3SimpleRenderSystem();

            void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<K3GameObject>& m_gameObjects, const K3Camera& camera);

        private:

            void createPipelineLayout();

            void createPipeline(VkRenderPass renderPass);

            std::shared_ptr<K3Device> m_device = nullptr;

            VkPipelineLayout m_pipelineLayout;

            std::unique_ptr<K3Pipeline> m_pipeline = nullptr;
    };
}