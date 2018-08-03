#pragma once

#include "GameScene.h"
#include "Debug.h"

class GameSettings;
class GameObject;


class BoxDemoScene: public GameScene
{
public:
	BoxDemoScene(GameSettings* pGameSettings);
	~BoxDemoScene(void);

protected:

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override {};
	virtual void SceneActivated() override;
	virtual void SceneDeactivated() override { }

private:

	GameObject *m_pVkCube0, *m_pVkCube1, *m_pVkCube2;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	BoxDemoScene(const BoxDemoScene& t);
	BoxDemoScene& operator=(const BoxDemoScene& t);
};

