#pragma once
#include "stdafx.h"
#include "InputTestScene.h"
#include "CubePosColorNorm.h"
#include "MeshObject.h"
#include "Debug.h"
#include "ContentManager.h"
#include "MeshObject.h"
#include "ManualCamera.h"
#include <GLFW/glfw3.h>

InputTestScene::InputTestScene(GameSettings* pSettings):
	GameScene(L"InputTestScene", pSettings),
	m_pSphere(nullptr),
	m_SpaceKeyStateLastFrame(GLFW_RELEASE)
{
}


InputTestScene::~InputTestScene(void)
{

}

void InputTestScene::Initialize()
{
	auto physX = &m_pPhysxScene->getPhysics();

	auto defaultMaterial = physX->createMaterial(.5f,.5f,.1f);
	EnablePhysxDebugRendering(true);

	//GROUND PLANE
	auto groundPlaneActor = physX->createRigidStatic(PxTransform::createIdentity());
	auto groundShape = groundPlaneActor->createShape(PxPlaneGeometry(),*defaultMaterial,PxTransform(PxQuat(glm::half_pi<float>(),PxVec3(0,0,1))));
	m_pPhysxScene->addActor(*groundPlaneActor);

	//Sphere
	m_pSphere = new MeshObject(L"Meshes/Sphere.ovm");
	auto actor = physX->createRigidDynamic(PxTransform::createIdentity());
	auto shape = actor->createShape(PxSphereGeometry(1.f),*defaultMaterial);

	m_pSphere->AttachRigidActor(actor);
	m_pSphere->Translate(0.f, 3.f, 0.f);
	AddGameObject(m_pSphere);

	auto d6Joint = PxD6JointCreate(*physX, nullptr, PxTransform::createIdentity(), actor, PxTransform::createIdentity());
	d6Joint->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
	d6Joint->setMotion(PxD6Axis::eY, PxD6Motion::eFREE);
	d6Joint->setMotion(PxD6Axis::eZ, PxD6Motion::eFREE);
	d6Joint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
	d6Joint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
	d6Joint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
}

void InputTestScene::Update()
{
	if(glfwGetKey(GetWindow(), GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		((PxRigidDynamic*)m_pSphere->GetRigidActor())->addTorque(PxVec3(0.f,0.f,50.f), PxForceMode::eFORCE);
	}

	if(glfwGetKey(GetWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		((PxRigidDynamic*)m_pSphere->GetRigidActor())->addTorque(PxVec3(0.f,0.f,-50.f), PxForceMode::eFORCE);
	}

	if(glfwGetKey(GetWindow(), GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		((PxRigidDynamic*)m_pSphere->GetRigidActor())->addTorque(PxVec3(-50.f,0.f,0.f), PxForceMode::eFORCE);
	}

	if(glfwGetKey(GetWindow(), GLFW_KEY_UP) == GLFW_PRESS)
	{
		((PxRigidDynamic*)m_pSphere->GetRigidActor())->addTorque(PxVec3(50.f,0.f,0.f), PxForceMode::eFORCE);
	}

	if(glfwGetKey(GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS && m_SpaceKeyStateLastFrame == GLFW_RELEASE)
	{
		((PxRigidDynamic*)m_pSphere->GetRigidActor())->addForce(PxVec3(0.f,5.f,0.f), PxForceMode::eIMPULSE);
	}
	m_SpaceKeyStateLastFrame = glfwGetKey(GetWindow(), GLFW_KEY_SPACE);
}

void InputTestScene::SceneActivated()
{
	Debug::LogInfo(L".::InputTestScene::.");
	Debug::LogInfo(L"KeyboardInput: \'Q\',\'E\', \'Space\'");
	Debug::LogInfo(L"GamepadInput: \'DPAD_LEFT\',\'DPAD_RIGHT\', \'A\'");

	//Reset Camera
	auto cam = GetCamera();
	auto pos = vec3(-1.10f, 12.16f, -36.67f);
	auto tar = vec3(-1.33f, 10.95f, -26.74f);
	static_cast<ManualCamera*>(cam)->SetPositionAndTarget(pos, tar);
}

void InputTestScene::SceneDeactivated()
{

}
