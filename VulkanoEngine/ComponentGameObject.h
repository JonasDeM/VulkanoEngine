#pragma once
#include "Component.h"
#include "BaseGameObject.h"
#include <vector>
#include <memory>
class VulkanContext;

// Only derive from this class to create fill the component array inside your constructor
template<uint8_t C>
class CompGameObj : public BaseGameObject
{
public:
	CompGameObj() = default;
	~CompGameObj() = default; //Non-virtal destructor
	CompGameObj(const CompGameObj& t) = delete;
	CompGameObj& operator=(const CompGameObj& t) = delete;

protected:
	std::array<std::unique_ptr<Component>, C> m_ComponentPtrArr;

private:
	void Build(VulkanContext* vkContext) override;
	void SceneStart() override;
	void Update(VulkanContext* vkContext) override;
	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex) override;
	void SceneStop() override;
};

template<uint8_t C>
void CompGameObj<C>::Build(VulkanContext * vkContext)
{
	for (auto& comp : m_ComponentPtrs)
	{
		comp->Build(vkContext);
	}
}

template<uint8_t C>
void CompGameObj<C>::SceneStart()
{
	for (auto& comp : m_ComponentPtrs)
	{
		comp->SceneStart();
	}
}

template<uint8_t C>
void CompGameObj<C>::Update(VulkanContext * vkContext)
{
	for (auto& comp : m_ComponentPtrs)
	{
		comp->Update(vkContext);
	}
	for (auto& child : m_ChildrenPtrs)
	{
		child->Update(vkContext);
	}
}

template<uint8_t C>
void CompGameObj<C>::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex)
{
	for (auto& comp : m_ComponentPtrs)
	{
		comp->RecordVulkanDrawCommands(cmdBuffer, frameBufferIndex);
	}
	for (auto& child : m_ChildrenPtrs)
	{
		child->RecordVulkanDrawCommands(cmdBuffer, frameBufferIndex);
	}
}

template<uint8_t C>
void CompGameObj<C>::SceneStop()
{
	for (auto& comp : m_ComponentPtrs)
	{
		comp->SceneStop();
	}
	for (auto& child : m_ChildrenPtrs)
	{
		child->SceneStop();
	}
}
