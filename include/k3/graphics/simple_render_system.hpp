#pragma once

#include "k3/logging/log.hpp"

#include "device.hpp"
#include "pipeline.hpp"
#include "camera.hpp"
#include "game_object.hpp"
#include "frame_info.hpp"

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

            void renderGameObjects(k3::graphics::K3FrameInfo& frameInfo, std::vector<K3GameObject>& m_gameObjects);

        private:

            void createPipelineLayout();

            void createPipeline(VkRenderPass renderPass);

            std::shared_ptr<K3Device> m_device = nullptr;

            VkPipelineLayout m_pipelineLayout;

            std::unique_ptr<K3Pipeline> m_pipeline = nullptr;
    };
}