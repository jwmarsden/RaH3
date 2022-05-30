#include "k3/graphics/simple_render_system.hpp"

namespace k3::graphics  {

    struct SimplePushConstantData {
        glm::mat4 transform{1.f};
        alignas(16) glm::vec3 color;
    };

    void KeSimpleRenderSystem::init(std::shared_ptr<KeDevice> device, VkRenderPass renderPass) {
        KE_IN(KE_NOARG);
        assert(!m_initFlag && "Already had init.");
        m_device = device;

        createPipelineLayout();
        createPipeline(renderPass);
        
        m_initFlag = true;
        KE_OUT(KE_NOARG);
    }

    void KeSimpleRenderSystem::shutdown() {
        KE_IN(KE_NOARG);
        assert(m_initFlag && "Must have been init to shutdown.");
        if(m_initFlag) {
            m_initFlag = false;
        
            if(m_pipeline != nullptr) {
                m_pipeline->shutdown();
                m_pipeline = nullptr;
            }
            vkDestroyPipelineLayout(m_device->getDevice() , m_pipelineLayout, nullptr);
        
            if(m_device != nullptr) {
                m_device = nullptr;
            }
        }
        KE_OUT(KE_NOARG);
    }

   void KeSimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
        KE_IN(KE_NOARG);
        assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        KePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;

        m_pipeline = std::make_unique<KePipeline>();
        m_pipeline->init(m_device, "./shaders/simple_shader.vert.spv", "./shaders/simple_shader.frag.spv", pipelineConfig);
        KE_OUT(KE_NOARG);
    }

    void KeSimpleRenderSystem::createPipelineLayout() {
        KE_IN(KE_NOARG);

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if(vkCreatePipelineLayout(m_device->getDevice() , &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            KE_CRITICAL("Failed to create pipeline layout.");
        }
        KE_OUT("(): _pipelineLayout@<{}>", fmt::ptr(&m_pipelineLayout));
    }

    void KeSimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<KeGameObject>& m_gameObjects, const KeCamera& camera) {
        m_pipeline->bind(commandBuffer);

        auto projectionView = camera.getProjection() * camera.getView();

        for(auto& gameObject: m_gameObjects) {
            
            SimplePushConstantData push{};
            push.color = gameObject.color;
            push.transform = projectionView * gameObject.transform.mat4();

            vkCmdPushConstants(commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
            gameObject.model->bind(commandBuffer);
            gameObject.model->draw(commandBuffer);
        }
    }

}