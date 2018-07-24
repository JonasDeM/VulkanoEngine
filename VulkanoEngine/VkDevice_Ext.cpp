#pragma once

#include "stdafx.h"

#include <set>
#include "VkDevice_Ext.h"
#include "VkPhysicalDevice_Ext.h"

VkDevice_Ext::VkDevice_Ext(const VkPhysicalDevice_Ext& physicalDevice, const std::vector<const char*>& deviceExtensions)
{
	auto queuefamilyIndices = physicalDevice.GetQueueFamilyIndices();

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = { queuefamilyIndices->graphicsFamily, queuefamilyIndices->presentFamily };

	float queuePriority = 1.0f;
	for (const int queuFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queuFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}


	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.fillModeNonSolid = VK_TRUE;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = (uint32_t)(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size(); //device specific features
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
#ifdef DEBUG
		createInfo.enabledLayerCount = m_ValidationLayers.size();
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
#else
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
#endif

	vkCreateDevice(physicalDevice, &createInfo, nullptr, this->BasePtr());
}
