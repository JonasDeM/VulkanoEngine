#pragma once
#include <vector>
#include "DeriveableHandle.h"

struct QueueFamilyIndices;
class VkPhysicalDevice_Ext;

class VkDevice_Ext : public DeriveableHandle<VkDevice>
{
public:
	VkDevice_Ext(const VkPhysicalDevice_Ext&  physicalDevice, const std::vector<const char*>& deviceExtensions);

	void Destroy() const { vkDestroyDevice(*this, nullptr); }
private:

};


