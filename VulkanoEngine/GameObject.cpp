#include "stdafx.h"
#include "GameObject.h"
#include "GameContext.h"
#include "GameScene.h"
#include "Debug.h"
#include "VulkanContext.h"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"


GameObject::GameObject(void) :
	m_pScene(nullptr),
	m_pParent(nullptr),
	m_IsInitialized(false),
	m_vecChildren(vector<std::unique_ptr<GameObject>>()),
	m_WorldMatrix(mat4()),
	m_Position(vec3(0, 0, 0)),
	m_Scale(vec3(1, 1, 1)),
	m_Rotation(vec4(0, 0, 0, 1)),
	m_WorldPosition(vec3(0, 0, 0)),
	m_WorldScale(vec3(1, 1, 1)),
	m_WorldRotation(vec4(0, 0, 0, 1)),
	m_pRigidActor(nullptr),
	m_PositionChanged(false),
	m_RotationChanged(false),
	m_ScaleChanged(false),
	m_RigidActorAdded(false),
	m_CanDraw(true)
{}


GameObject::~GameObject(void)
{
}

GameScene* GameObject::GetScene() const
{
	if (m_pScene != nullptr)
		return m_pScene;

	if (m_pParent != nullptr)
		return m_pParent->GetScene();

	return nullptr;
}

void GameObject::AddChild(GameObject* pObject)
{
	auto it = find_if(m_vecChildren.begin(), m_vecChildren.end(), [pObject](std::unique_ptr<GameObject> &pChildObject) {return pObject == pChildObject.get(); });

	if (it == m_vecChildren.end())
	{
		m_vecChildren.emplace_back(pObject);
		pObject->m_pScene = nullptr;
		pObject->m_pParent = this;

		if (m_IsInitialized)
			m_pScene->RootInitializeSceneObject(pObject);
			
		SceneManager::UpdateChanges();
	}
}

void GameObject::AttachRigidActor(PxRigidActor* pRigidActor)
{
	m_RigidActorAdded = false;
	if (m_pRigidActor != nullptr)
	{
		m_pRigidActor->release();
		Debug::LogWarning(L"GameObject::AttachRigidActor(...) > Overwritting current Actor!");
	}

	m_pRigidActor = pRigidActor;
	m_pRigidActor->userData = this;

	if (m_IsInitialized)
	{
		//Attach RigidBody
		if (m_pRigidActor)
		{
			PxTransform transform;
			transform.p = PxVec3(m_WorldPosition.x, m_WorldPosition.y, m_WorldPosition.z);
			transform.q = PxQuat(m_WorldRotation.x, m_WorldRotation.y, m_WorldRotation.z, m_WorldRotation.w);
			m_pRigidActor->setGlobalPose(transform);
			PxScaleRigidActor(*m_pRigidActor, m_Scale.x);

			//Attach actor to physx scene
			auto scene = GetScene();
			if (scene)
			{
				scene->GetPhysxScene()->addActor(*m_pRigidActor);
				m_RigidActorAdded = true;
			}
		}
	}
}

PxRigidActor* GameObject::DetachRigidActor()
{
	auto actor = m_pRigidActor;
	m_pRigidActor->userData = nullptr;
	m_pRigidActor = nullptr;
	m_RigidActorAdded = false;
	return actor;
}

void GameObject::RemoveChild(GameObject* pObject)
{
	auto it = find_if(m_vecChildren.begin(), m_vecChildren.end(), [pObject](std::unique_ptr<GameObject> &pChildObject) {return pObject == pChildObject.get(); });
	if (it != m_vecChildren.end())
	{
		m_vecChildren.erase(it);
		pObject->m_pScene = nullptr;
		pObject->m_pParent = nullptr;

		SceneManager::UpdateChanges();
	}
}

void GameObject::Translate(float x, float y, float z)
{
	if (m_RigidActorAdded)
	{
		if (m_pRigidActor->isRigidStatic())
		{
			Debug::LogWarning(L"GameObject::Translate(...) > Can't translate a STATIC RigidActor!");
			return;
		}

		m_PositionChanged = true;
	}

	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void GameObject::Rotate(float x, float y, float z)
{
	if (m_RigidActorAdded)
	{
		if (m_pRigidActor->isRigidStatic())
		{
			Debug::LogWarning(L"GameObject::Rotate(...) > Can't rotate a STATIC RigidActor!");
			return;
		}

		m_RotationChanged = true;
	}
	m_Rotation = quat(glm::eulerAngleYXZ(y,x,z)); // i force radians with a GLM preprocessor directive
}

void GameObject::RotateEuler(float x, float y, float z)
{
	if (m_RigidActorAdded)
	{
		if (m_pRigidActor->isRigidStatic())
		{
			Debug::LogWarning(L"GameObject::Rotate(...) > Can't rotate a STATIC RigidActor!");
			return;
		}

		m_RotationChanged = true;
	}

	m_Rotation = quat(glm::eulerAngleYXZ(y/180.0f*glm::pi<float>(), x/180.0f*glm::pi<float>(), z/180.0f*glm::pi<float>())); // i force radians with a GLM preprocessor directive
}

void GameObject::Scale(float x, float y, float z)
{
	if (m_RigidActorAdded)
	{
		if (m_pRigidActor->isRigidStatic())
		{
			Debug::LogWarning(L"GameObject::Scale(...) > Can't scale a STATIC RigidActor!");
			return;
		}

		if (x == y && y == z)
		{
			m_ScaleChanged = true;
		}
		else
		{
			Debug::LogWarning(L"GameObject::Scale(...) > Only Uniform scales are allowed (RigidActor)!");
			return;
		}
	}

	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;
}

void GameObject::RootInitialize(VulkanContext* pVkContext)
{
	if (m_IsInitialized)
		return;

	Initialize(pVkContext);

	for (auto& pObject : m_vecChildren)
	{
		pObject->RootInitialize(pVkContext);
	}

	//Set Initial Pose. (Only update a Root-GameObject (Children are updated by parent) => m_Parent == nullptr)
	if (!m_pParent)
	{
		CalculateWorldMatrix(mat4());
	}

	//Attach RigidBody
	if (m_pRigidActor)
	{
		PxTransform transform;
		transform.p = PxVec3(m_WorldPosition.x, m_WorldPosition.y, m_WorldPosition.z);
		transform.q = PxQuat(m_WorldRotation.x, m_WorldRotation.y, m_WorldRotation.z, m_WorldRotation.w);
		m_pRigidActor->setGlobalPose(transform);
		PxScaleRigidActor(*m_pRigidActor, m_Scale.x);

		//Attach actor to physx scene
		auto scene = GetScene();
		if (scene)
		{
			scene->GetPhysxScene()->addActor(*m_pRigidActor);
			m_RigidActorAdded = true;
		}
	}

	m_IsInitialized = true;
}

void GameObject::RootUpdate(VulkanContext* pVkContext)
{
	Update(pVkContext);

	for (auto& pObject : m_vecChildren)
	{
		pObject->RootUpdate(pVkContext);
	}
}


void GameObject::RootRecordVulkanDrawCommands(VkCommandBuffer cmdBuffer)
{
	if (!m_CanDraw)
		return;

	RecordVulkanDrawCommands(cmdBuffer);

	for (auto& pObject : m_vecChildren)
	{
		pObject->RootRecordVulkanDrawCommands(cmdBuffer);
	}
}

void GameObject::RootRecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, int frameBufferIndex)
{
	if (!m_CanDraw)
		return;

	RecordVulkanDrawCommands(cmdBuffer, frameBufferIndex);

	for (auto& pObject : m_vecChildren)
	{
		pObject->RootRecordVulkanDrawCommands(cmdBuffer, frameBufferIndex);
	}
}

void GameObject::CalculateWorldMatrix(const mat4 &parentWorld, bool updateChildren)
{
	if (m_RigidActorAdded)
	{
		bool isKinematic = m_pRigidActor->isRigidDynamic() && (((PxRigidDynamic*)m_pRigidActor)->getRigidDynamicFlags()&PxRigidDynamicFlag::eKINEMATIC);

		PxTransform globalPose = m_pRigidActor->getGlobalPose();

		if (m_PositionChanged)
		{
			globalPose.p.x = m_Position.x;
			globalPose.p.y = m_Position.y;
			globalPose.p.z = m_Position.z;
			if (!isKinematic)((PxRigidDynamic*)m_pRigidActor)->setLinearVelocity(PxVec3(0.f, 0.f, 0.f));
		}
		else
		{
			m_Position.x = globalPose.p.x;
			m_Position.y = globalPose.p.y;
			m_Position.z = globalPose.p.z;
		}

		if (m_RotationChanged)
		{
			globalPose.q.x = m_Rotation.x;
			globalPose.q.y = m_Rotation.y;
			globalPose.q.z = m_Rotation.z;
			globalPose.q.w = m_Rotation.w;
			if (!isKinematic)((PxRigidDynamic*)m_pRigidActor)->setAngularVelocity(PxVec3(0.f, 0.f, 0.f));
		}
		else
		{
			m_Rotation.x = globalPose.q.x;
			m_Rotation.y = globalPose.q.y;
			m_Rotation.z = globalPose.q.z;
			m_Rotation.w = globalPose.q.w;
		}

		if (m_PositionChanged || m_RotationChanged)
		{
			if (isKinematic)((PxRigidDynamic*)m_pRigidActor)->setKinematicTarget(globalPose);
			else m_pRigidActor->setGlobalPose(globalPose);
		}

		if (m_ScaleChanged)
		{
			PxScaleRigidActor(*m_pRigidActor, m_Scale.x);
		}
	}

	m_PositionChanged = m_RotationChanged = m_ScaleChanged = false;

	//make the ParentTranslateRotateScale (PTRS) matrix => this scales, rotates, translates and then applies parent transform (order: SRTP)
	m_WorldMatrix = mat4(1.0f);
	m_WorldMatrix *= parentWorld;
	m_WorldMatrix = glm::translate(m_WorldMatrix, m_Position);
	m_WorldMatrix *= glm::toMat4(m_Rotation);
	m_WorldMatrix = glm::scale(m_WorldMatrix, m_Scale);


	//Retrieve World Transform Information
	glm::decompose(m_WorldMatrix, m_WorldScale, m_WorldRotation, m_WorldPosition, vec3(), vec4());

	if (!updateChildren)
		return;

	for (auto& pObject : m_vecChildren)
	{
		pObject->CalculateWorldMatrix(m_WorldMatrix, updateChildren);
	}
}