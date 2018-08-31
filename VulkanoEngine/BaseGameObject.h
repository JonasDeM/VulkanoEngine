//#pragma once
//#include "Debug.h"
//#include <vector>
//
//class GameScene;
//class VulkanContext;
//class Component;
//
//class BaseGameObject
//{
//public:
//	enum Flags : uint8_t {
//		Invisible = 1 << 0, // doesn't need to be drawn
//		Static = 1 << 1, // doesn't move
//		//Resilient = 1 << 2, // needs to be moved to new scene
//	};
//public:
//	BaseGameObject(Flags flags = (Flags)0);
//	virtual ~BaseGameObject() = default;
//
//	// Transfers ownership to this
//	void AttachChild(BaseGameObject* pObject);
//	// Object is deleted
//	void DeleteChild(BaseGameObject* pObject);
//	// Moves ownership to scene
//	void DetachChild(BaseGameObject* pObject);
//	// Moves ownership to newParent
//	void DetachChild(BaseGameObject* pObject, BaseGameObject* newParent);
//
//	GameScene* GetScene() const { return m_pScene; };
//	BaseGameObject* GetParent() const { return m_pParent; }
//	Flags GetFlags() const { return m_Flags; }
//	// if setflags gets implemented, update draw if invisible is set to true from false
//
//protected:
//	virtual void Build(VulkanContext* vkContext) = 0;
//	virtual void SceneStart() = 0;
//	virtual void Update(VulkanContext* vkContext) = 0;
//	virtual void PostUpdate() = 0;
//	virtual void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex) = 0;
//	virtual void SceneStop() = 0;
//
//	GameScene* m_pScene = nullptr;
//	BaseGameObject* m_pParent = nullptr;
//	std::vector<std::unique_ptr<BaseGameObject>> m_ChildrenPtrs;
//	Flags m_Flags;
//private:
//	friend GameScene;
//	void RootBuild(VulkanContext* vkContext);
//	void RootSceneStart();
//	void RootUpdate(VulkanContext* vkContext);
//	void RootPostUpdate();
//	void RootRecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex);
//	void RootSceneStop();
//};
//
