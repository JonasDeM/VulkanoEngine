#pragma once
#include "stdafx.h"

#include "BouncingBallScene.h"
#include "MeshObject.h"
#include "Debug.h"
#include "ManualCamera.h"
#include <GLFW/glfw3.h>


BouncingBallScene::BouncingBallScene(GameSettings* pSettings):
	GameScene(L"BouncingBallScene", pSettings),
	m_pSphereNoBounce(nullptr), 
	m_pSphereMidBounce(nullptr), 
	m_pSphereMaxBounce(nullptr),
	m_SpaceKeyStateLastFrame(GLFW_RELEASE)
{
}


BouncingBallScene::~BouncingBallScene(void)
{

}

void BouncingBallScene::Initialize()
{
	EnablePhysxDebugRendering(true);
	auto physX = &m_pPhysxScene->getPhysics();

	auto defaultMaterial = physX->createMaterial(.5f,.5f,.1f);

	//!SPHERE DIAMETER = 2 (RADIUS = 1)

	//GROUND PLANE
	auto groundPlaneActor = physX->createRigidStatic(PxTransform::createIdentity());
	groundPlaneActor->createShape(PxPlaneGeometry(),*defaultMaterial,PxTransform(PxQuat(glm::half_pi<float>(),PxVec3(0,0,1))));
	m_pPhysxScene->addActor(*groundPlaneActor);

	//SPHERE NO-BOUNCE
	auto sphereMaterial = physX->createMaterial(.5f,.5f,.1f);
	m_pSphereNoBounce = new MeshObject(L"Meshes/Sphere.ovm");
	auto sphereActor = physX->createRigidDynamic(PxTransform::createIdentity());
	sphereActor->createShape(PxSphereGeometry(1.f),*sphereMaterial);
	m_pSphereNoBounce->AttachRigidActor(sphereActor);
	AddGameObject(m_pSphereNoBounce);

	//SPHERE MID-BOUNCE
	sphereMaterial = physX->createMaterial(.5f,.5f,.7f);
	m_pSphereMidBounce = new MeshObject(L"Meshes/Sphere.ovm");
	sphereActor = physX->createRigidDynamic(PxTransform::createIdentity());
	sphereActor->createShape(PxSphereGeometry(1.f),*sphereMaterial);
	m_pSphereMidBounce->AttachRigidActor(sphereActor);
	AddGameObject(m_pSphereMidBounce);

	//SPHERE MAX-BOUNCE
	sphereMaterial = physX->createMaterial(.5f,.5f,1.0f);
	m_pSphereMaxBounce = new MeshObject(L"Meshes/Sphere.ovm");
	sphereActor = physX->createRigidDynamic(PxTransform::createIdentity());
	sphereActor->createShape(PxSphereGeometry(1.f),*sphereMaterial);
	m_pSphereMaxBounce->AttachRigidActor(sphereActor);
	AddGameObject(m_pSphereMaxBounce);


	//CONTROLLABLE SPHERE
	m_pControllableSphere = new MeshObject(L"Meshes/Sphere.ovm");
	auto actor = physX->createRigidDynamic(PxTransform::createIdentity());
	auto shape = actor->createShape(PxSphereGeometry(1.f), *defaultMaterial);

	m_pControllableSphere->AttachRigidActor(actor);
	m_pControllableSphere->Translate(0.f, 3.f, 0.f);
	AddGameObject(m_pControllableSphere);

	auto d6Joint = PxD6JointCreate(*physX, nullptr, PxTransform::createIdentity(), actor, PxTransform::createIdentity());
	d6Joint->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
	d6Joint->setMotion(PxD6Axis::eY, PxD6Motion::eFREE);
	d6Joint->setMotion(PxD6Axis::eZ, PxD6Motion::eFREE);
	d6Joint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
	d6Joint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
	d6Joint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);

	//SCALE SPHERES
	auto sphereScale = 2.0f;
	m_pSphereNoBounce->Scale(sphereScale,sphereScale,sphereScale);
	m_pSphereMidBounce->Scale(sphereScale,sphereScale,sphereScale);
	m_pSphereMaxBounce->Scale(sphereScale,sphereScale,sphereScale);
	m_pControllableSphere->Scale(sphereScale, sphereScale, sphereScale);

}

void BouncingBallScene::Update()
{
	if(glfwGetKey(GetWindow(), GLFW_KEY_R) == GLFW_PRESS)
	{
		//Lazy reset
		SceneActivated();
	}

	// Controlling the sphere
	if (glfwGetKey(GetWindow(), GLFW_KEY_LEFT) == GLFW_PRESS)
		((PxRigidDynamic*)m_pControllableSphere->GetRigidActor())->addTorque(PxVec3(0.f, 0.f, 100.f), PxForceMode::eFORCE);
	if (glfwGetKey(GetWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS)
		((PxRigidDynamic*)m_pControllableSphere->GetRigidActor())->addTorque(PxVec3(0.f, 0.f, -100.f), PxForceMode::eFORCE);
	if (glfwGetKey(GetWindow(), GLFW_KEY_DOWN) == GLFW_PRESS)
		((PxRigidDynamic*)m_pControllableSphere->GetRigidActor())->addTorque(PxVec3(-100.f, 0.f, 0.f), PxForceMode::eFORCE);
	if (glfwGetKey(GetWindow(), GLFW_KEY_UP) == GLFW_PRESS)
		((PxRigidDynamic*)m_pControllableSphere->GetRigidActor())->addTorque(PxVec3(100.f, 0.f, 0.f), PxForceMode::eFORCE);
	if (glfwGetKey(GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS && m_SpaceKeyStateLastFrame == GLFW_RELEASE)
		((PxRigidDynamic*)m_pControllableSphere->GetRigidActor())->addForce(PxVec3(0.f, 150.f, 0.f), PxForceMode::eIMPULSE);
	m_SpaceKeyStateLastFrame = glfwGetKey(GetWindow(), GLFW_KEY_SPACE);
}

void BouncingBallScene::SceneActivated()
{
	Debug::LogInfo(L"4BallScene: Every ball has a different PhysX bounciness and thus reacts different to collisions.");
	Debug::LogInfo(L"4BallScene: You can control the ball with the Arrow keys + SpaceBar.");

	//Reset Camera
	auto cam = GetCamera();
	static_cast<ManualCamera*>(cam)->SetPositionAndTarget(vec3(-2.0f, 25.69f, -30.0f), vec3(0, -10.0f, 40.0f));

	//Reset Spheres on SceneActivation
	auto yStart = 45.0f;
	m_pSphereNoBounce->Translate(-10,yStart,0);
	m_pSphereMidBounce->Translate(0,yStart,0);
	m_pSphereMaxBounce->Translate(10,yStart,0);

	//Reset Velocity
	auto rigidBody = (PxRigidDynamic*)m_pSphereNoBounce->GetRigidActor();
	rigidBody->setLinearVelocity(PxVec3(0,0,0));

	rigidBody = (PxRigidDynamic*)m_pSphereMidBounce->GetRigidActor();
	rigidBody->setLinearVelocity(PxVec3(0,0,0));

	rigidBody = (PxRigidDynamic*)m_pSphereMaxBounce->GetRigidActor();
	rigidBody->setLinearVelocity(PxVec3(0,0,0));

}

void BouncingBallScene::SceneDeactivated()
{

}
