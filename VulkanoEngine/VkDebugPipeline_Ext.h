#pragma once
#include "VkBasicGeometryPipeline_Ext.h"

class VulkanContext;

//Doesn't do framebufferring
class VkDebugPipeline_Ext : public VkBasicGeometryPipeline_Ext
{
public:
	typedef struct VertexPosCol VertexType; // each pipeline that implements it's own vertexInputState needs to typedef the used Vertex struct as VertexType like this
protected:
	virtual std::vector<VkPipelineShaderStageCreateInfo> CreateShaderStageCreateInfos(HoldingContentLoader<VkShaderModule>& shaderModuleLoader) const final override;
	virtual VkPipelineVertexInputStateCreateInfo CreateVertexInputStateCreateInfo() const final override;
	virtual VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateCreateInfo() const final override;
	virtual VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo() const final override;
};
