#pragma once
#include "DeriveableHandle.h"

class VkBasicSampler_Ext : public DeriveableHandle<VkSampler>
{
public:
	VkBasicSampler_Ext(VkDevice device);
	void Destroy(VkDevice device) const
	{
		vkDestroySampler(device, *this, nullptr);
	}
};

