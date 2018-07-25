#pragma once

#include "GameContext.h"
#include "PhysxManager.h"
#include <vector>
#include "glm/gtx/quaternion.hpp"

class GameScene;
class VulkanContext;
using namespace std;
using namespace glm;

class GameObject
{
public:
	GameObject(void);
	virtual ~GameObject(void);

	vec3 GetPosition() const { return m_Position; }
	vec3 GetScale() const { return m_Scale; }
	quat GetRotation() const { return m_Rotation; }
	mat4 GetWorldMatrix() const { return m_WorldMatrix; }

	GameScene* GetScene() const;
	GameObject* GetParent() const { return m_pParent; }
	PxRigidActor* GetRigidActor() const { return m_pRigidActor; }

	void AddChild(GameObject* pObject);
	void AttachRigidActor(PxRigidActor* pRigidActor);
	PxRigidActor* DetachRigidActor();
	void RemoveChild(GameObject* pObject);

	void Translate(float x, float y, float z);
	void Rotate(float x, float y, float z);
	void RotateEuler(float x, float y, float z);
	void Scale(float x, float y, float z);
	void CanDraw(bool canDraw) { m_CanDraw = canDraw; }

	virtual void Initialize(VulkanContext* pVkContext) = 0;
	virtual void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex) {};
	virtual void Update(VulkanContext* pVkContext) = 0;

protected:
	
	bool m_IsInitialized;
	vector<std::unique_ptr<GameObject>> m_vecChildren;

	mat4 m_WorldMatrix;
	vec3 m_Position, m_Scale;
	quat m_Rotation;

	//Is not updated every frame
	vec3 m_WorldPosition, m_WorldScale;
	quat m_WorldRotation;

	PxRigidActor* m_pRigidActor;

private:

	friend GameScene;

	void RootInitialize(VulkanContext* pVkContext);
	void RootUpdate(VulkanContext* pVkContext);
	void RootRecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex);
	void CalculateWorldMatrix(const mat4 &parentWorld, bool updateChildren = true);

	GameScene* m_pScene;
	GameObject* m_pParent;
	bool m_PositionChanged, m_RotationChanged, m_ScaleChanged, m_RigidActorAdded;
	bool m_CanDraw;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	GameObject(const GameObject& t) = delete;
	GameObject& operator=(const GameObject& t) = delete;
};

