#pragma once
#include "stdafx.h"
#include "VkTextureLoader.h"
#include "VkDevice_Ext.h"
#include "VulkanContext.h"
#include "HandleUtilities.h"

std::shared_ptr<VkTextureImage_Ext> VkTextureLoader::LoadContent(const wstring& fileName) const
{
	auto pTex = CreateSharedExtendedHandle<VkTextureImage_Ext>(new VkTextureImage_Ext(m_pVulkanContext, fileName),	*m_pVulkanContext->GetVkDevice());
	return pTex;
}