#pragma once
#include "VkPosNormTexPipeline_Ext.h"

//contains the pipeline and pipelinelayout 
class VkPosNormTex2SPipeline_Ext : public VkPosNormTexPipeline_Ext
{
protected:
	virtual VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo() const override;
};
