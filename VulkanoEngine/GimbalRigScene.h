#pragma once
#include "GameScene.h"
#include "Debug.h"

class GimbalRig; 
class GameSettings;


class GimbalRigScene: public GameScene
{
public:
	GimbalRigScene(GameSettings* pGameSettings);
	~GimbalRigScene(void);

protected:

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override {};
	virtual void SceneActivated() override { Debug::LogInfo(L"GimbalRigScene Activated!"); }
	virtual void SceneDeactivated() override { Debug::LogInfo(L"GimbalRigScene Deactivated!"); }

private:

	GimbalRig *m_pVkGimbalRig;


private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	GimbalRigScene(const GimbalRigScene& t) = delete;
	GimbalRigScene& operator=(const GimbalRigScene& t) = delete;
};

