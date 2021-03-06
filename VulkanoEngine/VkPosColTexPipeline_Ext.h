#pragma once
#include "VkTexPipeline_Ext.h"

class VkPosColTexPipeline_Ext final: public VkTexPipeline_Ext
{
public:
	using VertexType = VertexPosColTex; // each pipeline that implements it's own vertexInputState should typedef the used Vertex struct as VertexType like this
protected:
	virtual std::vector<VkPipelineShaderStageCreateInfo> CreateShaderStageCreateInfos(HoldingContentLoader<VkShaderModule>& shaderModuleLoader) const override;
	virtual VkPipelineVertexInputStateCreateInfo CreateVertexInputStateCreateInfo() const override;
};
