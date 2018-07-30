
#include "stdafx.h"
#include "VkPosNormTex2SPipeline_Ext.h"

VkPipelineRasterizationStateCreateInfo VkPosNormTex2SPipeline_Ext::CreateRasterizationStateCreateInfo() const
{
	auto info = VkPosNormTexPipeline_Ext::CreateRasterizationStateCreateInfo();
	info.cullMode = VK_CULL_MODE_NONE;
	return info;
}
