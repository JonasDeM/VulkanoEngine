#pragma once
#include "GameScene.h"

class InputTestScene: public GameScene
{
public:
	InputTestScene(GameSettings* pSettings);
	~InputTestScene(void);

protected:

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override {};
	virtual void SceneActivated() override;
	virtual void SceneDeactivated() override;

private:

	GameObject* m_pSphere;

	int m_SpaceKeyStateLastFrame;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	InputTestScene(const InputTestScene& t);
	InputTestScene& operator=(const InputTestScene& t);
};

