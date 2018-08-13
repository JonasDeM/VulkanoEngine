#include "stdafx.h"
#include "BaseGameObject.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "TransformComponent.h"
#include "Debug.h"

BaseGameObject::BaseGameObject(Flags flags) : m_Flags(flags)
{
	m_pTransform = std::make_unique<TransformComponent>();
}

void BaseGameObject::AddChild(BaseGameObject * pObject)
{
	if (pObject->m_pParent != this)
	{
		m_ChildrenPtrs.emplace_back(std::unique_ptr<BaseGameObject>(pObject));
		pObject->m_pScene = m_pScene;
		pObject->m_pParent = this;
		pObject->m_pTransform->m_pParentTransform = this->m_pTransform.get();

		SceneManager::FlagDrawChanges();
	}
	else
	{
		Debug::LogWarning(L"Object is already child.");
	}
}

void BaseGameObject::DeleteChild(BaseGameObject * pObject)
{
	if (pObject->m_pParent == this)
	{
		auto it = std::find_if(m_ChildrenPtrs.begin(), m_ChildrenPtrs.end(), [pObject](std::unique_ptr<BaseGameObject> &pChildObject) {return pObject == pChildObject.get(); });
		m_ChildrenPtrs.erase(it);
		SceneManager::FlagDrawChanges();
	}
	else
	{
		Debug::LogWarning(L"Object is not a child.");
	}
}

void BaseGameObject::DetachChild(BaseGameObject * pObject)
{
	if (pObject->m_pParent == this)
	{
		auto it = std::find_if(m_ChildrenPtrs.begin(), m_ChildrenPtrs.end(), [pObject](std::unique_ptr<BaseGameObject> &pChildObject) {return pObject == pChildObject.get(); });
		auto& childPtr = (*it);
		childPtr->m_pParent = nullptr;
		childPtr->m_pTransform->m_pParentTransform = nullptr;
		childPtr->m_pScene->AddGameObject(it->release());
		m_ChildrenPtrs.erase(it);
	}
	else
	{
		Debug::LogWarning(L"Object is not a child.");
	}
}

void BaseGameObject::DetachChild(BaseGameObject * pObject, BaseGameObject * newParent)
{
	if (pObject->m_pParent == this)
	{
		auto it = std::find_if(m_ChildrenPtrs.begin(), m_ChildrenPtrs.end(), [pObject](std::unique_ptr<BaseGameObject> &pChildObject) {return pObject == pChildObject.get(); });
		newParent->AddChild(it->release());
		m_ChildrenPtrs.erase(it);
	}
	else
	{
		Debug::LogWarning(L"Object is not a child.");
	}
}

void BaseGameObject::RootBuild(VulkanContext * vkContext)
{
	Build(vkContext);
	m_pTransform->Build(vkContext);
	m_IsBuilt = true;
	for (auto& child : m_ChildrenPtrs)
	{
		child->RootBuild(vkContext);
	}
}

void BaseGameObject::RootSceneStart()
{
	SceneStart();
	m_pTransform->SceneStart();
	for (auto& child : m_ChildrenPtrs)
	{
		child->RootSceneStart();
	}
}

void BaseGameObject::RootUpdate(VulkanContext * vkContext)
{
	Update(vkContext);
	m_pTransform->Update(vkContext);
	for (auto& child : m_ChildrenPtrs)
	{
		child->RootUpdate(vkContext);
	}
}

void BaseGameObject::RootPostUpdate()
{
	PostUpdate();
	m_pTransform->PostUpdate();
	for (auto& child : m_ChildrenPtrs)
	{
		child->RootPostUpdate();
	}
}

void BaseGameObject::RootRecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex)
{
	if (!(m_Flags & Flags::Invisible))
	{
		RecordVulkanDrawCommands(cmdBuffer, frameBufferIndex);
		m_pTransform->RecordVulkanDrawCommands(cmdBuffer, frameBufferIndex);
	}

	for (auto& child : m_ChildrenPtrs)
	{
		child->RootRecordVulkanDrawCommands(cmdBuffer, frameBufferIndex);
	}
}

void BaseGameObject::RootSceneStop()
{
	SceneStop();
	m_pTransform->SceneStop();
	for (auto& child : m_ChildrenPtrs)
	{
		child->RootSceneStop();
	}
}
