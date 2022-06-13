#include "k3/graphics/simple_render_system.hpp"

namespace k3::graphics  {

    struct SimplePushConstantData {
        glm::mat4 transform{1.f};
        alignas(16) glm::vec3 color;
    };

    K3SimpleRenderSystem::K3SimpleRenderSystem(std::shared_ptr<K3Device> device, VkRenderPass renderPass) : m_device {device} {
        KE_IN(KE_NOARG);

        createPipelineLayout();
        createPipeline(renderPass);

        KE_OUT(KE_NOARG);
    }

    K3SimpleRenderSystem::~K3SimpleRenderSystem() {
        KE_IN(KE_NOARG);
  
        if(m_pipeline != nullptr) {
            m_pipeline = nullptr;
        }
        vkDestroyPipelineLayout(m_device->getDevice() , m_pipelineLayout, nullptr);
    
        if(m_device != nullptr) {
            m_device = nullptr;
        }
    
        KE_OUT(KE_NOARG);
    }

   void K3SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
        KE_IN(KE_NOARG);
        assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        K3Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;

        m_pipeline = std::make_unique<K3Pipeline>(m_device, "./shaders/simple_shader.vert.spv", "./shaders/simple_shader.frag.spv", pipelineConfig);

        KE_OUT(KE_NOARG);
    }

    void K3SimpleRenderSystem::createPipelineLayout() {
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
        
        KE_OUT("(): m_pipelineLayout@<{}>", fmt::ptr(&m_pipelineLayout));
    }

    void K3SimpleRenderSystem::renderGameObjects(k3::graphics::K3FrameInfo& frameInfo, std::vector<K3GameObject>& m_gameObjects) {
        m_pipeline->bind(frameInfo.commandBuffer);

        auto projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();

        for(auto& gameObject: m_gameObjects) {
            
            SimplePushConstantData push{};
            push.color = gameObject.color;
            push.transform = projectionView * gameObject.transform.mat4();

            vkCmdPushConstants(frameInfo.commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
            gameObject.model->bind(frameInfo.commandBuffer);
            gameObject.model->draw(frameInfo.commandBuffer);
        }
    }

}