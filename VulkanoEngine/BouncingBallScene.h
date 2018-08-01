#pragma once
#include "GameScene.h"

class MeshObject;

class BouncingBallScene: public GameScene
{
public:
	BouncingBallScene(GameSettings* pSettings);
	~BouncingBallScene(void);

protected:

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override {};
	virtual void SceneActivated() override;
	virtual void SceneDeactivated() override;

private:

	MeshObject *m_pSphereNoBounce, *m_pSphereMidBounce, *m_pSphereMaxBounce;
	GameObject* m_pControllableSphere;

	int m_SpaceKeyStateLastFrame;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	BouncingBallScene(const BouncingBallScene& t);
	BouncingBallScene& operator=(const BouncingBallScene& t);
};

