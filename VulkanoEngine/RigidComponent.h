#pragma once
#include "Component.h"

class TransformComponent;
class BaseGameObject;

// component that makes a rigidactor and transform of gameobject work together
class RigidComponent :	public Component
{
public:
	enum SimulationType : char 
	{
		Static,
		Dynamic,
		Kinematic
	};
public:
	RigidComponent(BaseGameObject* ownerObject, SimulationType simType, const PxMaterial* mat, PxGeometry geom);
	virtual ~RigidComponent() = default;

private:
	void Build(VulkanContext* vkContext) override;
	void Update(VulkanContext* vkContext) override;

	BaseGameObject* m_pGameObject;
	TransformComponent* m_pTransform;
	PxRigidActor * m_pRigidActor;

	SimulationType m_SimType;
	const PxMaterial* m_pMaterial;
	PxGeometry m_Geometry;
};

class RigidConvexMeshComponent : public RigidComponent
{
public:
	RigidConvexMeshComponent(BaseGameObject * ownerObject, SimulationType simType, const PxMaterial* mat, const wstring & mesh);
private:
	shared_ptr<PxConvexMesh> m_pConvexMesh;
};

class RigidTriangleMeshComponent : public RigidComponent
{
public:
	RigidTriangleMeshComponent(BaseGameObject * ownerObject, SimulationType simType, const PxMaterial* mat, const wstring & mesh);
private:
	shared_ptr<PxTriangleMesh> m_pTriangleMesh;
};