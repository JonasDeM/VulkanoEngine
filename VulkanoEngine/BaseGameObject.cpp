#include "stdafx.h"
//#include "BaseGameObject.h"
//#include "SceneManager.h"
//#include "GameScene.h"
//#include "TransformComponent.h"
//#include "Debug.h"
//
//BaseGameObject::BaseGameObject(Flags flags) : m_Flags(flags)
//{
//	if (!(flags & Flags::Invisible))
//		SceneManager::FlagDrawChanges();
//}
//
//void BaseGameObject::AttachChild(BaseGameObject * pObject)
//{
//	if (pObject->m_pParent != this)
//	{
//		m_ChildrenPtrs.emplace_back(std::unique_ptr<BaseGameObject>(pObject));
//		pObject->m_pScene = m_pScene;
//		pObject->m_pParent = this;
//
//		auto childTransform = pObject->GetComponent<TransformComponent>();
//		if (childTransform)
//			childTransform->m_pParentTransform = GetComponent<TransformComponent>();
//	}
//	else
//	{
//		Debug::LogWarning(L"Object is already child.");
//	}
//}
//
//void BaseGameObject::DeleteChild(BaseGameObject * pObject)
//{
//	if (pObject->m_pParent == this)
//	{
//		auto it = std::find_if(m_ChildrenPtrs.begin(), m_ChildrenPtrs.end(), [pObject](std::unique_ptr<BaseGameObject> &pChildObject) {return pObject == pChildObject.get(); });
//		m_ChildrenPtrs.erase(it);
//		if (!(m_Flags & Flags::Invisible))
//			SceneManager::FlagDrawChanges();
//	}
//	else
//	{
//		Debug::LogWarning(L"Object is not a child.");
//	}
//}
//
//void BaseGameObject::DetachChild(BaseGameObject * pObject)
//{
//	if (pObject->m_pParent == this)
//	{
//		auto it = std::find_if(m_ChildrenPtrs.begin(), m_ChildrenPtrs.end(), [pObject](std::unique_ptr<BaseGameObject> &pChildObject) {return pObject == pChildObject.get(); });
//		auto& childPtr = (*it);
//		childPtr->m_pParent = nullptr;
//
//		auto childTransform = pObject->GetComponent<TransformComponent>();
//		if (childTransform)
//			childTransform->m_pParentTransform = nullptr;
//
//		childPtr->m_pScene->AddGameObject(it->release());
//		m_ChildrenPtrs.erase(it);
//	}
//	else
//	{
//		Debug::LogWarning(L"Object is not a child.");
//	}
//}
//
//void BaseGameObject::DetachChild(BaseGameObject * pObject, BaseGameObject * newParent)
//{
//	if (pObject->m_pParent == this)
//	{
//		auto it = std::find_if(m_ChildrenPtrs.begin(), m_ChildrenPtrs.end(), [pObject](std::unique_ptr<BaseGameObject> &pChildObject) {return pObject == pChildObject.get(); });
//		newParent->AttachChild(it->release());
//		m_ChildrenPtrs.erase(it);
//	}
//	else
//	{
//		Debug::LogWarning(L"Object is not a child.");
//	}
//}
//
//void BaseGameObject::RootBuild(VulkanContext * vkContext)
//{
//	Build(vkContext);
//	for (auto& child : m_ChildrenPtrs)
//	{
//		child->RootBuild(vkContext);
//	}
//}
//
//void BaseGameObject::RootSceneStart()
//{
//	SceneStart();
//	for (auto& child : m_ChildrenPtrs)
//	{
//		child->RootSceneStart();
//	}
//}
//
//void BaseGameObject::RootUpdate(VulkanContext * vkContext)
//{
//	Update(vkContext);
//
//	for (auto& child : m_ChildrenPtrs)
//	{
//		child->RootUpdate(vkContext);
//	}
//}
//
//void BaseGameObject::RootPostUpdate()
//{
//	PostUpdate();
//	for (auto& child : m_ChildrenPtrs)
//	{
//		child->RootPostUpdate();
//	}
//}
//
//void BaseGameObject::RootRecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex)
//{
//	if (!(m_Flags & Flags::Invisible))
//		RecordVulkanDrawCommands(cmdBuffer, frameBufferIndex);
//
//	for (auto& child : m_ChildrenPtrs)
//	{
//		child->RootRecordVulkanDrawCommands(cmdBuffer, frameBufferIndex);
//	}
//}
//
//void BaseGameObject::RootSceneStop()
//{
//	SceneStop();
//	for (auto& child : m_ChildrenPtrs)
//	{
//		child->RootSceneStop();
//	}
//}
