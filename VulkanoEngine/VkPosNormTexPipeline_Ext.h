#pragma once
#include "VkTexPipeline_Ext.h"
class VulkanContext;

//contains the pipeline and pipelinelayout 
class VkPosNormTexPipeline_Ext : public VkTexPipeline_Ext
{
public:
	using VertexType = VertexPosNormTex; // each pipeline that implements it's own vertexInputState should typedef the used Vertex struct as VertexType like this
protected:
	virtual std::vector<VkPipelineShaderStageCreateInfo> CreateShaderStageCreateInfos(HoldingContentLoader<VkShaderModule>& shaderModuleLoader) const final override;
	virtual VkPipelineVertexInputStateCreateInfo CreateVertexInputStateCreateInfo() const final override;
};
