#include "stdafx.h"
#include "RigidComponent.h"
#include "TransformComponent.h"
#include "BaseGameObject.h"
#include "PhysxManager.h"
#include "GameScene.h"
#include "ContentManager.h"


RigidComponent::RigidComponent(BaseGameObject * ownerObject, SimulationType simType, const PxMaterial* mat, PxGeometry geom) :
m_SimType(simType),
m_pMaterial(mat),
m_Geometry(geom)
{
	if (m_pGameObject->GetFlags() & BaseGameObject::Flags::Static)
	{
		Debug::LogWarning(L"You tried to attach a dynamic rigidactor to a static gameobject.");
		m_SimType = SimulationType::Static;
	}
}

void RigidComponent::Build(VulkanContext * vkContext)
{
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	switch (m_SimType)
	{
	case RigidComponent::Static:
		m_pRigidActor = physX->createRigidStatic(PxTransform::createIdentity());
		break;
	case RigidComponent::Dynamic:
		m_pRigidActor = physX->createRigidDynamic(PxTransform::createIdentity());
		break;
	case RigidComponent::Kinematic:
		m_pRigidActor = physX->createRigidDynamic(PxTransform::createIdentity());
		static_cast<PxRigidBody*>(m_pRigidActor)->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
		break;
	default:
		break;
	}
	m_pRigidActor->createShape(m_Geometry, *m_pMaterial);

	m_pTransform = m_pGameObject->GetTransform();
	auto worldPos = m_pTransform->GetWorldPosition();
	auto worldRot = m_pTransform->GetWorldRotation();

	PxTransform transform;
	transform.p = PxVec3(worldPos.x, worldPos.y, worldPos.z);
	transform.q = PxQuat(worldRot.x, worldRot.y, worldRot.z, worldRot.w);
	m_pRigidActor->setGlobalPose(transform);
	PxScaleRigidActor(*m_pRigidActor, m_pTransform->GetWorldScale().x);

	m_pRigidActor->userData = m_pGameObject;

	//Attach actor to physx scene
	auto scene = m_pGameObject->GetScene();
	if(scene != nullptr)
	{
		scene->GetPhysxScene()->addActor(*m_pRigidActor);
	}
}

void RigidComponent::Update(VulkanContext * vkContext)
{
	if (m_SimType == SimulationType::Static)
		return;

	//change to pointer and if(x==nullptr)
	bool isKinematic = m_pRigidActor->isRigidDynamic() && (((PxRigidDynamic*)m_pRigidActor)->getRigidDynamicFlags()&PxRigidDynamicFlag::eKINEMATIC);

	PxTransform globalPose = m_pRigidActor->getGlobalPose();

	auto transformFlags = m_pTransform->GetFlags();
	using TFlags = TransformComponent::Flags;

	if (TFlags::PosChanged & transformFlags)
	{
		auto pos = m_pTransform->GetWorldPosition();
		globalPose.p.x = pos.x;
		globalPose.p.y = pos.y;
		globalPose.p.z = pos.z;
		if (!isKinematic)((PxRigidDynamic*)m_pRigidActor)->setLinearVelocity(PxVec3(0.f, 0.f, 0.f));
	}
	else
	{
		m_pTransform->Translate(globalPose.p.x, globalPose.p.y, globalPose.p.z);
	}

	if (TFlags::RotChanged & transformFlags)
	{
		auto rot = m_pTransform->GetWorldRotation();
		globalPose.q.x = rot.x;
		globalPose.q.y = rot.y;
		globalPose.q.z = rot.z;
		globalPose.q.w = rot.w;
		if (!isKinematic)((PxRigidDynamic*)m_pRigidActor)->setAngularVelocity(PxVec3(0.f, 0.f, 0.f));
	}
	else
	{
		m_pTransform->RotateQuat(globalPose.q.x, globalPose.q.y, globalPose.q.z, globalPose.q.w);
	}

	if ((TFlags::PosChanged | TFlags::RotChanged) & transformFlags)
	{
		if (isKinematic)((PxRigidDynamic*)m_pRigidActor)->setKinematicTarget(globalPose);
		else m_pRigidActor->setGlobalPose(globalPose);
	}

	if (TFlags::ScaleChanged & transformFlags)
	{
		PxScaleRigidActor(*m_pRigidActor, m_pTransform->GetWorldScale().x);
	}
}

RigidConvexMeshComponent::RigidConvexMeshComponent(BaseGameObject * ownerObject, SimulationType simType, const PxMaterial* mat, const wstring & mesh) :
	m_pConvexMesh(ContentManager::Load<PxConvexMesh>(mesh)),
	RigidComponent(ownerObject, simType, mat, PxConvexMeshGeometry(m_pConvexMesh.get()))
{

}

RigidTriangleMeshComponent::RigidTriangleMeshComponent(BaseGameObject * ownerObject, SimulationType simType, const PxMaterial* mat, const wstring & mesh) :
	m_pTriangleMesh(ContentManager::Load<PxTriangleMesh>(mesh)),
	RigidComponent(ownerObject, simType, mat, PxTriangleMeshGeometry(m_pTriangleMesh.get()))
{
}
