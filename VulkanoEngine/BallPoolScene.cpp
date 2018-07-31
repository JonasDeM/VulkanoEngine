#pragma once
#include "stdafx.h"

#include "BallPoolScene.h"
#include "MeshObject.h"
#include "Debug.h"
#include "ManualCamera.h"
#include "PhysxHelpers.h"


BallPoolScene::BallPoolScene(GameSettings* pSettings):
	GameScene(L"BallPoolScene", pSettings)
{
}


BallPoolScene::~BallPoolScene(void)
{

}

void BallPoolScene::Initialize()
{
	EnablePhysxDebugRendering(true);
	auto physX = &m_pPhysxScene->getPhysics();

	auto defaultMaterial = physX->createMaterial(.5f,.5f,.1f);

	//GROUND PLANE
	auto groundPlaneActor = physX->createRigidStatic(PxTransform::createIdentity());
	groundPlaneActor->createShape(PxPlaneGeometry(),*defaultMaterial,PxTransform(PxQuat(glm::half_pi<float>(),PxVec3(0,0,1))));
	m_pPhysxScene->addActor(*groundPlaneActor);

	//DYN SPHERES
	auto sphereMaterial = physX->createMaterial(.5f,.5f,.7f);
	const auto sphereScale = 1.f;
	for (size_t i = 0; i < AMOUNT_BALLS; i++)
	{
		auto sphere = new MeshObject(L"Meshes/Sphere.ovm");
		m_PhysXBalls[i] = sphere;
		auto sphereActor = physX->createRigidDynamic(PxTransform::createIdentity());
		sphereActor->createShape(PxSphereGeometry(1.f), *sphereMaterial);
		sphereActor->setLinearDamping(1.5f);
		sphere->AttachRigidActor(sphereActor);
		sphere->Scale(sphereScale, sphereScale, sphereScale);
		AddGameObject(sphere);
	}

	//KINEMATIC ACTOR
	m_pControllableBall = new MeshObject(L"Meshes/Sphere.ovm");
	m_pKinematicActor = physX->createRigidDynamic(PxTransform::createIdentity());
	m_pKinematicActor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
	m_pKinematicActor->createShape(PxSphereGeometry(1.f), *defaultMaterial);
	m_KinematicPosition = PxVec3(0.f, 1.f, 0.f);
	m_pControllableBall->Scale(15.f, 15.f, 15.f);
	m_pControllableBall->AttachRigidActor(m_pKinematicActor);
	m_pControllableBall->Translate(m_KinematicPosition.x, m_KinematicPosition.y, m_KinematicPosition.z);
	AddGameObject(m_pControllableBall);

	//Disable Gravity
	GetPhysxScene()->setGravity(PxVec3(0, 0, 0));
}

void BallPoolScene::Update()
{
	if(glfwGetKey(GetWindow(), GLFW_KEY_R) == GLFW_PRESS)
		ResetScene();

	//CONTROLS
	if (glfwGetKey(GetWindow(), GLFW_KEY_LEFT) == GLFW_PRESS)
		m_KinematicPosition.x -= m_KinematicSpeed * (float)GetTimer()->GetElapsedSeconds();
	if (glfwGetKey(GetWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS)
		m_KinematicPosition.x += m_KinematicSpeed * (float)GetTimer()->GetElapsedSeconds();
	if (glfwGetKey(GetWindow(), GLFW_KEY_DOWN) == GLFW_PRESS)
		m_KinematicPosition.y -= m_KinematicSpeed * (float)GetTimer()->GetElapsedSeconds();
	if (glfwGetKey(GetWindow(), GLFW_KEY_UP) == GLFW_PRESS)
		m_KinematicPosition.y += m_KinematicSpeed * (float)GetTimer()->GetElapsedSeconds();
	m_pKinematicActor->setKinematicTarget(PxTransform(m_KinematicPosition));

	// BOOM + FORCE
	const auto controlPos = m_pControllableBall->GetPosition();
	bool explode = glfwGetKey(GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS && m_SpaceKeyStateLastFrame == GLFW_RELEASE;
	for (size_t i = 0; i < AMOUNT_BALLS; i++)
	{
		auto vec = controlPos - m_PhysXBalls[i]->GetPosition();
		vec = glm::normalize(vec)*50.0f;
		auto rDyn = ((PxRigidDynamic*)m_PhysXBalls[i]->GetRigidActor());
		rDyn->addForce(ToPxVec3(vec), PxForceMode::eFORCE);
		if (explode)
			rDyn->addForce(ToPxVec3(-vec*2.0f), PxForceMode::eIMPULSE);
	}
	m_SpaceKeyStateLastFrame = glfwGetKey(GetWindow(), GLFW_KEY_SPACE);
}

void BallPoolScene::SceneActivated()
{
#if defined(DEBUG) | defined(_DEBUG) //this is very heavy in debugmode, so i deactivate it
	EnablePhysxDebugRendering(false);
#endif
	Debug::LogInfo(L"4BallScene: You can control the Big ball with the Arrow keys + SpaceBar.");

	ResetScene();

	//Reset Camera
	auto cam = GetCamera();
	static_cast<ManualCamera*>(cam)->SetPositionAndTarget(vec3(-2.0f, 25.69f, -50.0f), vec3(0, -10.0f, 40.0f));
}

void BallPoolScene::SceneDeactivated()
{

}

void BallPoolScene::ResetScene()
{
	//Reset Velocity
	const float space = 5.0f, startY = 30.0f;
	for (int x = 0; x < X_AMOUNT; x++)
	{
		for (int y = 0; y < Y_AMOUNT; y++)
		{
			for (int z = 0; z < Z_AMOUNT; z++)
			{
				int index = z + y * Z_AMOUNT + x * Y_AMOUNT* Z_AMOUNT;
				m_PhysXBalls[index]->Translate(x*space, startY+(float)y*space, z*space);
				((PxRigidDynamic*)m_PhysXBalls[index]->GetRigidActor())->setLinearVelocity(PxVec3(0, 0, 0));
			}
		}
	}

	// Reset m_KinematicPosition
	m_KinematicPosition = PxVec3(0.f, 1.f, 0.f);
}
