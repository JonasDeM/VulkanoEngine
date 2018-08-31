#pragma once
#include <vulkan/vulkan.h>

class CompObj;
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

	friend class CompObj; // sets the gameobject to this
	CompObj* m_pCompObj = nullptr;
};