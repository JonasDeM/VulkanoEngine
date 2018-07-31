#pragma once
#include "stdafx.h"
#include "GameSettings.h"
#include "VulkanContext.h"
#include "BaseCamera.h"


GameSettings::GameSettings() :
	m_BackbufferExtent(VkExtent2D{ 1280,720 }),
	m_Title(L"VulkanoEngine"),
	m_VsyncActive(true),
	m_ClearColor(VkClearColorValue{0.7f,0.7f,0.7f}),
	m_pCameras()
{}

void GameSettings::AttachVkContextObserver(VulkanContext * observer)
{
	m_pVulkanContext = observer;
}

void GameSettings::AttachCameraObserver(BaseCamera * observer)
{
	m_pCameras.emplace_back(observer);
}

void GameSettings::SetBackBufferExtent(VkExtent2D extent)
{
	if (memcmp(&m_BackbufferExtent,&extent,sizeof(VkExtent2D))==0)
		return;
	m_BackbufferExtent = extent;
	for (BaseCamera* cam : m_pCameras)
	{
		cam->SetAspectRatio(static_cast<float>(m_BackbufferExtent.width) / static_cast<float>(m_BackbufferExtent.height));
	}
}

void GameSettings::SetVSync(bool enableVSync)
{
	if (enableVSync == m_VsyncActive)
		return;
	m_VsyncActive = enableVSync;
	//TODO set flag to vulkandrawer to use vsync
}

void GameSettings::SetClearColor(VkClearColorValue color)
{
	if (memcmp(&m_ClearColor, &color, sizeof(VkClearColorValue)) == 0)
		return;
	m_ClearColor = color;
	m_pVulkanContext->SetFlags(VkContextFlags::InvalidDrawCommandBuffers);
}
