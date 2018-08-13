#pragma once
#include <vulkan/vulkan.h>

class VulkanContext;

class Component
{
public:
	Component() = default;
	virtual ~Component() = default;
	Component(const Component& t) = delete;
	Component& operator=(const Component& t) = delete;
protected:
	virtual void Build(VulkanContext* vkContext) {};
	virtual void SceneStart() {};
	virtual void Update(VulkanContext* vkContext) {};
	virtual void PostUpdate() {};
	virtual void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex) {};
	virtual void SceneStop() {};

private:
	template<uint8_t C>
	friend class CompGameObj;
};