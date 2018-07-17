#pragma once
#include "GameObject.h"
class VkGameObject : public GameObject
{
public:
	VkGameObject();
	~VkGameObject();

	virtual void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer) = 0;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	VkGameObject(const VkGameObject& t) = delete;
	VkGameObject& operator=(const VkGameObject& t) = delete;
};

