#pragma once
#include "vulkan/vulkan.h"
#include <vector>

class VulkanContext;
class BaseCamera; 

class GameSettings
{
public:
	GameSettings();

	void AttachVkContextObserver(VulkanContext* observer);
	void AttachCameraObserver(BaseCamera* observer);

	VkExtent2D GetWindowExtent() const { return m_BackbufferExtent; }
		
	void SetBackBufferExtent(VkExtent2D extent);
	void SetVSync(bool enableVSync);
	void SetClearColor(VkClearColorValue color);
	VkClearColorValue GetClearColor() const { return m_ClearColor; };

	//disabling copy
	GameSettings(const GameSettings&) = delete;
	void operator=(GameSettings const &x) = delete;

private:
	VkExtent2D m_BackbufferExtent;
	wstring m_Title;
	bool m_VsyncActive;
	VkClearColorValue m_ClearColor;

	//observers, they need to be notified when something changed, be it genereal or specific
	VulkanContext* m_pVulkanContext;
	std::vector<BaseCamera*> m_pCameras;
};
