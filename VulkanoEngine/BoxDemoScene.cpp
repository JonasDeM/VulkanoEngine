#include "stdafx.h"
#include "BoxDemoScene.h"
#include "Debug.h"
#include "CubePosColorTex.h"
#include "CubePosColorNorm.h"
#include "ManualCamera.h"


BoxDemoScene::BoxDemoScene(GameSettings* pGameSettings):
	GameScene(L"BoxDemoScene", pGameSettings),
	m_pVkCube0(nullptr),
	m_pVkCube1(nullptr),
	m_pVkCube2(nullptr)
{
}


BoxDemoScene::~BoxDemoScene(void)
{
}

void BoxDemoScene::Initialize()
{
	m_pVkCube0 = new CubePosColorTex(5.f, 5.f, 5.f);
	m_pVkCube1 = new CubePosColorTex(4.f, 4.f, 4.f);
	m_pVkCube1->Translate(6.f, 0.f, 0.f);
	m_pVkCube2 = new CubePosColorTex(2.f, 2.f, 2.f);
	m_pVkCube2->Translate(0.f, 4.f, 0.f);

	AddGameObject(m_pVkCube0);
	m_pVkCube0->AddChild(m_pVkCube1);
	m_pVkCube1->AddChild(m_pVkCube2);

	m_pVkCube0->Scale(1.5, 1.5, 1.5);

	m_pVkCube0->RotateEuler(0, 0, 90.0f);
}

void BoxDemoScene::Update()
{
	float totSec = (float)GetTimer()->GetTotalSecondsF();
	m_pVkCube0->RotateEuler(180.0f, totSec * 90.f, 0.f);
	m_pVkCube1->RotateEuler(totSec * 90.f, 0.f, 0.f);
	m_pVkCube2->RotateEuler(0.f, 0.f, totSec * 90.f);
}

void BoxDemoScene::SceneActivated()
{
	Debug::LogInfo(L"RotatingBoxScene: Showing the Parent-Child relation in the Transforms of the GameObjects.");
	auto cam = GetCamera();
	static_cast<ManualCamera*>(cam)->SetPositionAndTarget(vec3(0, 0, -20.f), vec3(0.f, 0.f, 0.f));
}
