#include "stdafx.h"
#include "PipelineManager.h"
#include "VkPosColTexPipeline_Ext.h"
#include "VkDebugPipeline_Ext.h"
#include "VkBasicGeometryPipeline_Ext.h"
#include "VkPosNormTexPipeline_Ext.h"
#include "VkPosNormTex2SPipeline_Ext.h"
#include "VulkanContext.h"
#include "VkDevice_Ext.h"
#include "VkSwapChainKHR_Ext.h"
#include "VkShaderModuleLoader.h"

unordered_map<std::type_index, unique_ptr_del<VkPipeline_Ext>> PipelineManager::m_Pipelines{};

void PipelineManager::Initialize(VulkanContext* pVkContext)
{
	VkDevice device = *pVkContext->GetVkDevice();
	VkShaderModuleLoader shaderLoader(pVkContext);

	// Pipelines with NO BASEPIPELINE
	AddPipeline<VkPosColTexPipeline_Ext>(device);
	AddPipeline<VkBasicGeometryPipeline_Ext>(device);
	AddPipeline<VkPosNormTexPipeline_Ext>(device);
	AddPipeline<VkPosNormTex2SPipeline_Ext>(device);
	AddPipeline<VkDebugPipeline_Ext>(device);
	for (auto& pipeline : m_Pipelines)
	{
		pipeline.second->Build(pVkContext, shaderLoader);
	}
}

void PipelineManager::CleanUp()
{
	m_Pipelines.clear();
}
