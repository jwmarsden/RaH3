#include "k3/graphics/pipeline.hpp"

namespace k3::graphics {

    K3Pipeline::K3Pipeline(std::shared_ptr<K3Device> device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& pipelineConfigInfo) : m_device {device} {
        KE_IN(KE_NOARG); 

        createGraphicsPipeline(vertFilePath, fragFilePath, pipelineConfigInfo);

        KE_OUT(KE_NOARG); 
    }

    K3Pipeline::~K3Pipeline() {
        KE_IN(KE_NOARG); 

        vkDestroyShaderModule(m_device->getDevice() , m_vertexShaderModule, nullptr);
        vkDestroyShaderModule(m_device->getDevice() , m_fragmentShaderModule, nullptr);
        vkDestroyPipeline(m_device->getDevice() , m_graphicsPipeline, nullptr);

        KE_OUT(KE_NOARG); 
    }

    std::vector<char> K3Pipeline::readFile(const std::string& filePath) {
        KE_IN(KE_NOARG);

        std::ifstream file(filePath, std::ios::ate | std::ios::binary);

        if(!file.is_open()) {
            KE_CRITICAL("Failed to open file: \"{}\"", filePath);
            throw std::runtime_error("Failed to open file:" + filePath);
        }

        KE_DEBUG("Loading File: {}", filePath);

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        KE_DEBUG("Read {} Bytes from File: {}", fileSize, filePath);
        KE_OUT(KE_NOARG);
        return buffer;
    }

    void K3Pipeline::createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& pipelineConfigInfo) {
        KE_IN(KE_NOARG);
        auto vertCode = readFile(vertFilePath);
        auto fragCode = readFile(fragFilePath);
        
        createShaderModule(vertCode, &m_vertexShaderModule);
        createShaderModule(fragCode, &m_fragmentShaderModule);

        VkPipelineShaderStageCreateInfo shaderStages[2];
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = m_vertexShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[0].flags = 0;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = m_fragmentShaderModule;
        shaderStages[1].pName = "main";
        shaderStages[1].flags = 0;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;

        auto bindingDescriptions = K3Vertex::getBindingDescriptions();
        auto attributeDescription = K3Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &pipelineConfigInfo.inputAssemblyInfo;
        pipelineInfo.pViewportState = &pipelineConfigInfo.viewportInfo;
        pipelineInfo.pRasterizationState = &pipelineConfigInfo.rasterizationInfo;
        pipelineInfo.pMultisampleState = &pipelineConfigInfo.multisampleInfo;
        pipelineInfo.pColorBlendState = &pipelineConfigInfo.colorBlendInfo;
        pipelineInfo.pDepthStencilState = &pipelineConfigInfo.depthStencilInfo;
        pipelineInfo.pDynamicState = &pipelineConfigInfo.dynamicStateInfo;

        pipelineInfo.layout = pipelineConfigInfo.pipelineLayout;
        pipelineInfo.renderPass = pipelineConfigInfo.renderPass;
        pipelineInfo.subpass = pipelineConfigInfo.subpass;

        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        KE_TRACE("Creating graphics pipeline.");
        if(vkCreateGraphicsPipelines(m_device->getDevice() , VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
            KE_CRITICAL("Failed to create graphics pipeline.");
            throw std::runtime_error("Failed to create graphics pipeline.");
        }
        KE_OUT(KE_NOARG);
    }

    void K3Pipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
        KE_IN(KE_NOARG);
        VkShaderModuleCreateInfo createShaderModuleInfo{};
        createShaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createShaderModuleInfo.codeSize = code.size();
        createShaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if(vkCreateShaderModule(m_device->getDevice() , &createShaderModuleInfo, nullptr, shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to Create Shader Module");
        }
        KE_OUT(KE_NOARG);
    }

    void K3Pipeline::bind(VkCommandBuffer commandBuffer) {
        //KE_IN(KE_NOARG);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
        //KE_OUT(KE_NOARG);
    }

    void K3Pipeline::defaultPipelineConfigInfo(PipelineConfigInfo& configInfo) {
        KE_IN(KE_NOARG);
        
        configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
        
        configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        configInfo.viewportInfo.viewportCount = 1;
        configInfo.viewportInfo.pViewports = nullptr;
        configInfo.viewportInfo.scissorCount = 1;
        configInfo.viewportInfo.pScissors = nullptr;

        configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
        configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        //configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;
        configInfo.rasterizationInfo.lineWidth = 1.0f;
        configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
        configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
        configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
        configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional
        
        configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
        configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
        configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
        configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional
        
        configInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
        configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
        configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional
        
        configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
        configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
        configInfo.colorBlendInfo.attachmentCount = 1;
        configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
        configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
        configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
        configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
        configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional
        
        configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
        configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
        configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
        configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
        configInfo.depthStencilInfo.front = {};  // Optional
        configInfo.depthStencilInfo.back = {};   // Optional

        configInfo.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
        configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
        configInfo.dynamicStateInfo.flags = 0;
        
        KE_OUT(KE_NOARG);
    }
}