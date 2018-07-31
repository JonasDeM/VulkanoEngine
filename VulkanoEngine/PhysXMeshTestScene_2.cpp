#pragma once
#include "stdafx.h"
#include "PhysXMeshTestScene_2.h"
#include "CubePosColorNorm.h"
#include "MeshObject.h"
#include "Debug.h"
#include "ContentManager.h"
#include "MeshObjectTex.h"
#include "ManualCamera.h"

PhysXMeshTestScene_2::PhysXMeshTestScene_2(GameSettings* pGameSettings):
	GameScene(L"PhysXMeshTestScene_2", pGameSettings),
	m_pConvexChair(nullptr)
{
}

PhysXMeshTestScene_2::~PhysXMeshTestScene_2(void)
{

}

void PhysXMeshTestScene_2::Initialize()
{
	auto physX = &m_pPhysxScene->getPhysics();

	auto defaultMaterial = physX->createMaterial(.5f,.5f,.1f);
	EnablePhysxDebugRendering(true);

	//GROUND PLANE
	auto groundPlaneActor = physX->createRigidStatic(PxTransform::createIdentity());
	auto groundShape = groundPlaneActor->createShape(PxPlaneGeometry(),*defaultMaterial,PxTransform(PxQuat(glm::half_pi<float>(),PxVec3(0,0,1))));
	m_pPhysxScene->addActor(*groundPlaneActor);

	m_pTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Chair.ovpt");
	m_pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Chair.ovpc");

	//Triangle mesh
	auto chairTriangle = new MeshObjectTex(L"Meshes/Chair.ovm", L"Textures/Chair_Dark.tga");
	m_pKinematicActor = physX->createRigidDynamic(PxTransform::createIdentity());
	m_pKinematicActor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
	m_pKinematicActor->createShape(PxTriangleMeshGeometry(m_pTriangleMesh.get()),*defaultMaterial);
	m_KinematicPosition = PxVec3(10.f,1.f,0.f);

	chairTriangle->AttachRigidActor(m_pKinematicActor);
	//chairTriangle->Scale(0.1f,0.1f,0.1f);
	chairTriangle->Translate(m_KinematicPosition.x, m_KinematicPosition.y,m_KinematicPosition.z);
	AddGameObject(chairTriangle);

	//Convex mesh
	m_pConvexChair = new MeshObjectTex(L"Meshes/Chair.ovm", L"Textures/Chair_Dark.tga");
	auto convexActor = physX->createRigidDynamic(PxTransform::createIdentity());
	convexActor->createShape(PxConvexMeshGeometry(m_pConvexMesh.get()),*defaultMaterial);
	PxRigidBodyExt::updateMassAndInertia(*convexActor, 10.f);

	m_pConvexChair->AttachRigidActor(convexActor);
	m_pConvexChair->Translate(0.f,2.f,0.f);
	//m_pConvexChair->Scale(0.1f,0.1f,0.1f);
	AddGameObject(m_pConvexChair);

}

void PhysXMeshTestScene_2::Update()
{ 
	if(glfwGetKey(GetWindow(), GLFW_KEY_R) == GLFW_PRESS) //down
	{
		m_pConvexChair->Translate(0.f,2.f,0.f);
		m_pConvexChair->Rotate(0.f,0.f,0.f);

		m_KinematicPosition = PxVec3(10.f, 1.f, 0.f);
		m_pKinematicActor->setGlobalPose(PxTransform(m_KinematicPosition)); //Teleporting (no collision interactions)
	}

	if(glfwGetKey(GetWindow(), GLFW_KEY_LEFT) == GLFW_PRESS) //down
	{
		m_KinematicPosition.x -= 5.f * (float)GetTimer()->GetElapsedSeconds();
		m_pKinematicActor->setKinematicTarget(PxTransform(m_KinematicPosition)); //Moving (collision interactions)
	}
	if (glfwGetKey(GetWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS) //down
	{
		m_KinematicPosition.x += 5.f * (float)GetTimer()->GetElapsedSeconds();
		m_pKinematicActor->setKinematicTarget(PxTransform(m_KinematicPosition)); //Moving (collision interactions)
	}
	if (glfwGetKey(GetWindow(), GLFW_KEY_DOWN) == GLFW_PRESS) //down
	{
		m_KinematicPosition.z -= 5.f * (float)GetTimer()->GetElapsedSeconds();
		m_pKinematicActor->setKinematicTarget(PxTransform(m_KinematicPosition)); //Moving (collision interactions)
	}
	if (glfwGetKey(GetWindow(), GLFW_KEY_UP) == GLFW_PRESS) //down
	{
		m_KinematicPosition.z += 5.f * (float)GetTimer()->GetElapsedSeconds();
		m_pKinematicActor->setKinematicTarget(PxTransform(m_KinematicPosition)); //Moving (collision interactions)
	}
}

void PhysXMeshTestScene_2::SceneActivated()
{
	Debug::LogInfo(L"ChairScene: Press the Arrow Keys to move the kinematic actor, so you can push the dynamic actor. \'R\' to reset.");

	//Reset Camera
	auto cam = GetCamera();
	auto pos = vec3(0.06f, 15.97f, -35.91f);
	auto tar = vec3(0.40f, 13.22f, -26.31f);
	static_cast<ManualCamera*>(cam)->SetPositionAndTarget(pos, tar);
}

void PhysXMeshTestScene_2::SceneDeactivated()
{

}
