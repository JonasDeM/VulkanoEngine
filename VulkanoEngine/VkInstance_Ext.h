#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "DeriveableHandle.h"

class VkInstance_Ext : public DeriveableHandle<VkInstance>
{
public:
	VkInstance_Ext(bool enableValidationLayers=true, const std::vector<const char*>& extensions={ "VK_LAYER_LUNARG_standard_validation" });

	void Destroy() const { vkDestroyInstance(*this, nullptr); }

private:
	static bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);
	static std::vector<const char*> ChooseVulkanExtensions(bool enableValidationLayers);
};

