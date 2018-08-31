#pragma once
#include "stdafx.h"
#include "CompObj.h"
#include "TransformComponent.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "Component.h"
#include "Debug.h"

CompObj::CompObj(Flags flags) : m_Flags(flags)
{
	if (!(flags & Flags::Invisible))
		SceneManager::FlagDrawChanges();
}

void CompObj::Build(VulkanContext * vkContext)
{
	for (auto& comp : m_ComponentPtrs)
	{
		comp->Build(vkContext);
	}
	for (auto& child : m_ChildrenPtrs)
	{
		child->Build(vkContext);
	}
}

void CompObj::SceneStart()
{
	for (auto& comp : m_ComponentPtrs)
	{
		comp->SceneStart();
	}
	for (auto& child : m_ChildrenPtrs)
	{
		child->SceneStart();
	}
}

void CompObj::Update(VulkanContext * vkContext)
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

void CompObj::PostUpdate()
{
	for (auto& comp : m_ComponentPtrs)
	{
		comp->PostUpdate();
	}
	for (auto& child : m_ChildrenPtrs)
	{
		child->PostUpdate();
	}
}

void CompObj::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex)
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

void CompObj::SceneStop()
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


void CompObj::AttachChild(CompObj * pObject)
{
	if (pObject->m_pParent != this)
	{
		m_ChildrenPtrs.emplace_back(std::unique_ptr<CompObj>(pObject));
		pObject->m_pScene = m_pScene;
		pObject->m_pParent = this;

		auto childTransform = pObject->GetComponent<TransformComponent>();
		if (childTransform)
			childTransform->m_pParentTransform = GetComponent<TransformComponent>();
	}
	else
	{
		Debug::LogWarning(L"Object is already child.");
	}
}

void CompObj::DeleteChild(CompObj * pObject)
{
	if (pObject->m_pParent == this)
	{
		auto it = std::find_if(m_ChildrenPtrs.begin(), m_ChildrenPtrs.end(), [pObject](std::unique_ptr<CompObj> &pChildObject) {return pObject == pChildObject.get(); });
		m_ChildrenPtrs.erase(it);
		if (!(m_Flags & Flags::Invisible))
			SceneManager::FlagDrawChanges();
	}
	else
	{
		Debug::LogWarning(L"Object is not a child.");
	}
}

void CompObj::DetachChild(CompObj * pObject)
{
	if (pObject->m_pParent == this)
	{
		auto it = std::find_if(m_ChildrenPtrs.begin(), m_ChildrenPtrs.end(), [pObject](std::unique_ptr<CompObj> &pChildObject) {return pObject == pChildObject.get(); });
		auto& childPtr = (*it);
		childPtr->m_pParent = nullptr;

		auto childTransform = pObject->GetComponent<TransformComponent>();
		if (childTransform)
			childTransform->m_pParentTransform = nullptr;

		childPtr->m_pScene->AddGameObject(it->release());
		m_ChildrenPtrs.erase(it);
	}
	else
	{
		Debug::LogWarning(L"Object is not a child.");
	}
}

void CompObj::DetachChild(CompObj * pObject, CompObj * newParent)
{
	if (pObject->m_pParent == this)
	{
		auto it = std::find_if(m_ChildrenPtrs.begin(), m_ChildrenPtrs.end(), [pObject](std::unique_ptr<CompObj> &pChildObject) {return pObject == pChildObject.get(); });
		newParent->AttachChild(it->release());
		m_ChildrenPtrs.erase(it);
	}
	else
	{
		Debug::LogWarning(L"Object is not a child.");
	}
}
