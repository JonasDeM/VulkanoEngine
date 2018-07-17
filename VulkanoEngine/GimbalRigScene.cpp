#include "stdafx.h"
#include "GimbalRigScene.h"
#include "GimbalRig.h"

GimbalRigScene::GimbalRigScene(GameSettings* pGameSettings):
	GameScene(L"GimbalRigScene", pGameSettings)
{
}


GimbalRigScene::~GimbalRigScene(void)
{
}

void GimbalRigScene::Initialize()
{
	m_pVkGimbalRig = new GimbalRig();
	AddGameObject(m_pVkGimbalRig);
}

void GimbalRigScene::Update()
{
}
