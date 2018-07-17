#pragma once
#include "GameScene.h"
#include "Debug.h"

class CylinderPosColorNorm; 
class GameSettings;

class CarouselScene: public GameScene
{
public:
	CarouselScene(GameSettings* pGameSettings);
	~CarouselScene(void);

protected:

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override {};
	virtual void SceneActivated() override { Debug::LogInfo(L"CarouselScene Activated!"); }
	virtual void SceneDeactivated() override { Debug::LogInfo(L"CarouselScene Deactivated!"); }

private:


	//vulkan objects
	CylinderPosColorNorm *m_pVkBase;
	vector<GameObject*> m_VkDiscs;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	CarouselScene(const CarouselScene& t);
	CarouselScene& operator=(const CarouselScene& t);
};

