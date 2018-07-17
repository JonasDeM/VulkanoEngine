#include "stdafx.h"
#include "VkPipelineManager.h"
#include "VkPosColTexPipeline_Ext.h"
#include "VkDebugPipeline_Ext.h"
#include "VkPosColNormPipeline_Ext.h"
#include "VkPosNormTexPipeline_Ext.h"
#include "VkPosNormTex2SPipeline_Ext.h"
#include "VulkanContext.h"
#include "VkDevice_Ext.h"
#include "VkSwapChainKHR_Ext.h"


void VkPipelineManager::Initialize(VulkanContext* pVkContext)
{
	VkDevice device = *pVkContext->GetVkDevice();

	m_pVkPosColTexPipeline = CreateExtendedHandle(new VkPosColTexPipeline_Ext(pVkContext), device);
	m_pVkPosColNormPipeline = CreateExtendedHandle(new VkPosColNormPipeline_Ext(pVkContext), device);
	m_pVkPosNormTexPipeline = CreateExtendedHandle(new VkPosNormTexPipeline_Ext(pVkContext), device);
	m_pVkPosNormTex2SPipeline = CreateExtendedHandle(new VkPosNormTex2SPipeline_Ext(pVkContext), device);
	m_pVkDebugPipeline = CreateExtendedHandle(new VkDebugPipeline_Ext(pVkContext), device);
}

