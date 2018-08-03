#pragma once
#include "GameScene.h"


class BallPoolScene: public GameScene
{
public:
	BallPoolScene(GameSettings* pSettings);
	~BallPoolScene(void);

protected:

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override {};
	virtual void SceneActivated() override;
	virtual void SceneDeactivated() override;

private:
	void ResetScene();

#if defined(DEBUG) | defined(_DEBUG)
	static const int X_AMOUNT = 8, Y_AMOUNT = 8, Z_AMOUNT = 2;
#else
	static const int X_AMOUNT = 15, Y_AMOUNT = 15, Z_AMOUNT = 3;
#endif

	static const int AMOUNT_BALLS = X_AMOUNT* Y_AMOUNT*Z_AMOUNT;
	std::array<GameObject*, AMOUNT_BALLS> m_PhysXBalls;
	GameObject* m_pControllableBall = nullptr;

	PxRigidDynamic* m_pKinematicActor;
	PxVec3 m_KinematicPosition;
	const float m_KinematicSpeed = 17.f;

	int m_SpaceKeyStateLastFrame;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	BallPoolScene(const BallPoolScene& t);
	BallPoolScene& operator=(const BallPoolScene& t);
};

