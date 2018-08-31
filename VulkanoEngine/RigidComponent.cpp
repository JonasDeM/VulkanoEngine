#include "stdafx.h"
#include "RigidComponent.h"
#include "TransformComponent.h"
#include "BaseGameObject.h"
#include "PhysxManager.h"
#include "GameScene.h"
#include "ContentManager.h"
#include "CompObj.h"


RigidComponent::RigidComponent(SimulationType simType, const PxMaterial& mat, const PxGeometry& geom)
{
	// I do this in constructor, because simtype, material and geometry only need to be used at creation of these objects
	// usually i try to keep the amount of work in the contructor to a minimum

	auto physX = PhysxManager::GetInstance()->GetPhysics();
	switch (simType)
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

	m_pRigidActor->createShape(geom, mat);
}

void RigidComponent::Build(VulkanContext * vkContext)
{
	if (m_pCompObj->GetFlags() & CompObj::Flags::Static)
		Debug::LogError(L"You tried to attach a dynamic rigidactor to a static gameobject.");

	m_pTransform = m_pCompObj->GetComponent<TransformComponent>();
	if (m_pTransform->GetParentTransform())
	{
		Debug::LogError(L"Transforms with a parenttransform can't be controlled by a rigidcomponent (not yet supported).");
	}
	auto worldPos = m_pTransform->GetPosition();
	auto worldRot = m_pTransform->GetRotation();

	PxTransform transform;
	transform.p = PxVec3(worldPos.x, worldPos.y, worldPos.z);
	transform.q = PxQuat(worldRot.x, worldRot.y, worldRot.z, worldRot.w);
	m_pRigidActor->setGlobalPose(transform);
	PxScaleRigidActor(*m_pRigidActor, m_pTransform->GetScale().x);

	m_pRigidActor->userData = m_pCompObj;

	//Attach actor to physx scene
	auto scene = m_pCompObj->GetScene();
	if(scene != nullptr)
	{
		scene->GetPhysxScene()->addActor(*m_pRigidActor);
	}
}

void RigidComponent::Update(VulkanContext * vkContext)
{
	if (m_pRigidActor->isRigidStatic())
		return;

	//change to pointer and if(x==nullptr)
	bool isKinematic = m_pRigidActor->isRigidDynamic() && (((PxRigidDynamic*)m_pRigidActor)->getRigidDynamicFlags()&PxRigidDynamicFlag::eKINEMATIC);

	PxTransform globalPose = m_pRigidActor->getGlobalPose();

	auto transformFlags = m_pTransform->GetFlags();
	using TFlags = TransformComponent::Flags;

	if (TFlags::PosChanged & transformFlags)
	{
		auto pos = m_pTransform->GetPosition();
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
		auto rot = m_pTransform->GetRotation();
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
}

RigidConvexMeshComponent::RigidConvexMeshComponent(SimulationType simType, const PxMaterial& mat, const wstring & mesh) :
	m_pConvexMesh(ContentManager::Load<PxConvexMesh>(mesh)),
	RigidComponent(simType, mat, PxConvexMeshGeometry(m_pConvexMesh.get()))
{

}

RigidTriangleMeshComponent::RigidTriangleMeshComponent(SimulationType simType, const PxMaterial& mat, const wstring & mesh) :
	m_pTriangleMesh(ContentManager::Load<PxTriangleMesh>(mesh)),
	RigidComponent(simType, mat, PxTriangleMeshGeometry(m_pTriangleMesh.get()))
{
}
