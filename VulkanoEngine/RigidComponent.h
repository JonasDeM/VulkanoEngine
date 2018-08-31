#pragma once
#include "Component.h"

class TransformComponent;
class BaseGameObject;

// component that makes a rigidactor and transform of gameobject work together
// Not Supported: scaling rigidactor after build, transform has a parenttransform
// Idea: in postupdate change according to transformchange, get worldpositions, in transformcomp flag all children too
class RigidComponent :	public Component
{
public:
	enum SimulationType : uint8_t 
	{
		Static,
		Dynamic,
		Kinematic
	};
public:
	RigidComponent(SimulationType simType, const PxMaterial& mat, const PxGeometry& geom);
	virtual ~RigidComponent() = default;

private:
	void Build(VulkanContext* vkContext) override;
	void Update(VulkanContext* vkContext) override;

	TransformComponent* m_pTransform;
	PxRigidActor * m_pRigidActor;
};

class RigidConvexMeshComponent : public RigidComponent
{
public:
	RigidConvexMeshComponent(SimulationType simType, const PxMaterial& mat, const wstring & mesh);
private:
	shared_ptr<PxConvexMesh> m_pConvexMesh;
};

class RigidTriangleMeshComponent : public RigidComponent
{
public:
	RigidTriangleMeshComponent(SimulationType simType, const PxMaterial& mat, const wstring & mesh);
private:
	shared_ptr<PxTriangleMesh> m_pTriangleMesh;
};