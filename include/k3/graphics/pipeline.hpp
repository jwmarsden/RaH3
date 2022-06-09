#pragma once

#include "log.h"

#include "device.hpp"
#include "model.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace k3::graphics {

    struct PipelineConfigInfo {
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class K3Pipeline {

        public:

            K3Pipeline(std::shared_ptr<K3Device> device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& pipelineConfigInfo);

            ~K3Pipeline();
            
            void bind(VkCommandBuffer commandBuffer);

            static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

        private:

            static std::vector<char> readFile(const std::string& filePath);

            void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& pipelineConfigInfo);

            void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

            std::shared_ptr<K3Device> m_device = nullptr;

            VkPipeline m_graphicsPipeline;

            VkShaderModule m_vertexShaderModule;

            VkShaderModule m_fragmentShaderModule;

    };

}