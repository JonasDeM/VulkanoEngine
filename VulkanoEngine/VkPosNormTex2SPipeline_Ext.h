#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include "DeriveableHandle.h"
#include "HandleUtilities.h"
#include "VkPosNormTexPipeline_Ext.h"
class VulkanContext;

//contains the pipeline and pipelinelayout 
class VkPosNormTex2SPipeline_Ext : public VkPosNormTexPipeline_Ext
{
protected:
	virtual VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo() const override;
};
