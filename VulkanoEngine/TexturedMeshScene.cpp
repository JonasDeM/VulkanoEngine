#pragma once
#include "stdafx.h"
#include "TexturedMeshScene.h"
#include "MeshObject.h"
#include "MeshObjectTex.h"
#include "MeshObjectTranspTex.h"
#include "ManualCamera.h"
#include "GameTimer.h"
#include <random>

TexturedMeshScene::TexturedMeshScene(GameSettings* pGameSettings):
	GameScene(L"MeshTestScene", pGameSettings)
{
}


TexturedMeshScene::~TexturedMeshScene(void)
{

}

void TexturedMeshScene::Initialize()
{
#if defined(DEBUG) | defined(_DEBUG)
	const int sqrtAmount = 7; // showing 7*7*4=196 GameObjects in debug
#else
	const int sqrtAmount = 20; // showing 20*20*4=1600 GameObjects in release
#endif

	const float space = 20.f;

	std::array<MeshObjectTranspTex*, sqrtAmount*sqrtAmount> transpObjectsPtrArr;
	for (int x = 0; x < sqrtAmount; x++)
	{
		for (int z = 0; z < sqrtAmount; z++)
		{
			int randRot = (rand() % 50) - 50;
			const bool isStatic = false;

			auto pedestal2 = new MeshObjectTex(L"Meshes/pedestal.ovm", L"Textures/Pedestal.png", isStatic);
			auto hut2 = new MeshObjectTex(L"Meshes/hut.ovm", L"Textures/Hut.png", isStatic);
			pedestal2->AddChild(hut2);
			auto floor2 = new MeshObjectTex(L"Meshes/floor.ovm", L"Textures/Floor.png", isStatic);
			pedestal2->AddChild(floor2);
			pedestal2->Scale(0.01f, 0.01f, 0.01f);
			pedestal2->RotateEuler(0, (float)randRot, 0);
			AddGameObject(pedestal2);
			pedestal2->Translate(x * space - (space*(float)sqrtAmount /2.0f), 0, z * space - (space*(float)sqrtAmount /2.0f));

			auto leaves2 = new MeshObjectTranspTex(L"Meshes/leaves.ovm", L"Textures/LeavesT.png", isStatic);
			leaves2->Scale(0.01f, 0.01f, 0.01f);
			leaves2->Translate(x * space - (space*(float)sqrtAmount / 2.0f), 0, z*space - (space*(float)sqrtAmount / 2.0f));
			leaves2->RotateEuler(0, (float)randRot,0);
			int index = z + x* sqrtAmount;
			transpObjectsPtrArr[index] = leaves2;
		}
	}

	//Temporary way of getting better results with just alpha blending: adding the semi-tranparent objects last
	for (size_t i = 0; i < sqrtAmount*sqrtAmount; i++)
	{
		AddGameObject(transpObjectsPtrArr[i]);
	}
}

void TexturedMeshScene::Update()
{
	auto totSec = GetTimer()->GetTotalSecondsF();
}

void TexturedMeshScene::SceneActivated()
{
	Debug::LogInfo(L"MeshScene: Every Hut is made of 22.000 Triangles");

	//Reset Camera
	auto cam = GetCamera();
	static_cast<ManualCamera*>(cam)->SetPositionAndTarget(vec3(47.7872734f, 18.5104141f, -64.9715805f), vec3(47.0625305f, 18.0765991f, -64.4362793f));
}
