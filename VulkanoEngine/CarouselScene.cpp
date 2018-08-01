#pragma once
#include "stdafx.h"
#include "CarouselScene.h"
#include "SceneManager.h"
#include "CubePosColorNorm.h"
#include "CylinderPosColorNorm.h"
#include "GameTimer.h"

CarouselScene::CarouselScene(GameSettings* pGameSettings):
	GameScene(L"CarouselScene", pGameSettings)
{
}


CarouselScene::~CarouselScene(void)
{
}

void CarouselScene::Initialize()
{

	float boxSize = 2.f;
	float discSize = 6.f;

	//vulkan objects
	m_pVkBase = new CylinderPosColorNorm(20, 20, 0.75f, glm::Yellow);

	for (unsigned int i = 0; i < 3; ++i)
	{
		float discStep = (glm::two_pi<float>() / 3.f)*i;
		auto disc = new CylinderPosColorNorm(discSize, 10, 1.25f, glm::Green);
		disc->Translate(12 * cos(discStep), 0.75f, 12 * sin(discStep));

		for (unsigned int j = 0; j < 3; ++j)
		{
			float step = (glm::two_pi<float>() / 3.f)*j;
			auto box = new CubePosColorNorm(boxSize, boxSize, boxSize);
			box->Translate((discSize - 2.5f) * cos(step), 1.25f, (discSize - 2.5f) * sin(step));
			disc->AddChild(box);
		}

		m_VkDiscs.push_back(disc);
		m_pVkBase->AddChild(disc);
	}
	AddGameObject(m_pVkBase);
}

void CarouselScene::Update()
{
	float totSec = GetTimer()->GetTotalSecondsF();

	m_pVkBase->RotateEuler(0, totSec * 90, 0);
	for (auto obj : m_VkDiscs)
	{
		obj->RotateEuler(0, -totSec * 90, 0);
	}
}
