#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include "DeriveableHandle.h"
#include "HandleUtilities.h"
#include "VkTexPipeline_Ext.h"


class VulkanContext;
//contains the pipeline, the pipelinelayout 

class VkPosColTexPipeline_Ext final: public VkTexPipeline_Ext
{
	typedef struct VertexPosColTex VertexType; // each pipeline that implements it's own vertexInputState needs to typedef the used Vertex struct as VertexType like this
protected:
	virtual std::vector<VkPipelineShaderStageCreateInfo> CreateShaderStageCreateInfos(HoldingContentLoader<VkShaderModule>& shaderModuleLoader) const override;
	virtual VkPipelineVertexInputStateCreateInfo CreateVertexInputStateCreateInfo() const override;
};
