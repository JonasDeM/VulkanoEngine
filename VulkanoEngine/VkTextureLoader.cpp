#include "stdafx.h"
#include "VkTextureLoader.h"
#include "VkDevice_Ext.h"
#include "VulkanContext.h"
#include "HandleUtilities.h"

std::shared_ptr<VkTextureImage_Ext> VkTextureLoader::LoadContent(const wstring& fileName, VulkanContext* vkContext) const
{
	auto pTex = CreateSharedExtendedHandle<VkTextureImage_Ext>(new VkTextureImage_Ext(vkContext, fileName),	*vkContext->GetVkDevice());
	return pTex;
}