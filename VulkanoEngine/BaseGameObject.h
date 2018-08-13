#pragma once
#include "TransformComponent.h"

class GameScene;
class VulkanContext;

class BaseGameObject
{
public:
	enum Flags : char {
		Invisible = 1 << 1, // doesn't draw
		Static = 1 << 2, // doesn't update transform]
	};
public:
	BaseGameObject(Flags flags = (Flags)0);
	virtual ~BaseGameObject() = default;

	// Transfers ownership to this
	void AddChild(BaseGameObject* pObject);
	// Object is deleted
	void DeleteChild(BaseGameObject* pObject);
	// Moves ownership to scene
	void DetachChild(BaseGameObject* pObject);
	// Moves ownership to newParent
	void DetachChild(BaseGameObject* pObject, BaseGameObject* newParent);

	GameScene* GetScene() const { return m_pScene; };
	BaseGameObject* GetParent() const { return m_pParent; }
	TransformComponent* GetTransform() const { return m_pTransform.get(); }
	Flags GetFlags() const { return m_Flags; }

protected:
	virtual void Build(VulkanContext* vkContext) = 0;
	virtual void SceneStart() = 0;
	virtual void Update(VulkanContext* vkContext) = 0;
	virtual void PostUpdate() {};
	virtual void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex) = 0;
	virtual void SceneStop() = 0;

	bool m_IsBuilt = false;

	GameScene* m_pScene = nullptr;
	BaseGameObject* m_pParent = nullptr;
	std::vector<std::unique_ptr<BaseGameObject>> m_ChildrenPtrs;
	std::unique_ptr<TransformComponent> m_pTransform;
	Flags m_Flags;

private:
	friend GameScene;
	void RootBuild(VulkanContext* vkContext);
	void RootSceneStart();
	void RootUpdate(VulkanContext* vkContext);
	void RootPostUpdate();
	void RootRecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex);
	void RootSceneStop();
};

