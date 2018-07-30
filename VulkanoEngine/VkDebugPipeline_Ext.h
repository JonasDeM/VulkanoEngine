#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include "DeriveableHandle.h"
#include "HandleUtilities.h"
#include "VkBasicGeometryPipeline_Ext.h"

class VulkanContext;

//Doesn't do framebufferring
class VkDebugPipeline_Ext : public VkBasicGeometryPipeline_Ext
{
protected:
	virtual std::vector<VkPipelineShaderStageCreateInfo> CreateShaderStageCreateInfos(HoldingContentLoader<VkShaderModule>& shaderModuleLoader) const final override;
	virtual VkPipelineVertexInputStateCreateInfo CreateVertexInputStateCreateInfo() const final override;
	virtual VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateCreateInfo() const final override;
	virtual VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo() const final override;
};
