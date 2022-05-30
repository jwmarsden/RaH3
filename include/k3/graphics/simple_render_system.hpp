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

    class KeSimpleRenderSystem {
        public:

            KeSimpleRenderSystem() = default;

            ~KeSimpleRenderSystem() {if(m_initFlag) shutdown();}

            void init(std::shared_ptr<KeDevice> device, VkRenderPass renderPass);

            void shutdown();  

            void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<KeGameObject>& m_gameObjects, const KeCamera& camera);

        private:

            void createPipelineLayout();

            void createPipeline(VkRenderPass renderPass);

            bool m_initFlag = false;

            std::shared_ptr<KeDevice> m_device;

            VkPipelineLayout m_pipelineLayout;

            std::unique_ptr<KePipeline> m_pipeline = nullptr;
    };
}