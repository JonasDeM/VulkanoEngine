#pragma once
#include "GameScene.h"
#include "Debug.h"

class GameSettings;

class TexturedMeshScene: public GameScene
{
public:
	TexturedMeshScene(GameSettings* pGameSettings);
	~TexturedMeshScene(void);

protected:

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override {};
	virtual void SceneActivated() override;
	virtual void SceneDeactivated() override { };

private:

	GameObject* m_pVkMesh;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	TexturedMeshScene(const TexturedMeshScene& t);
	TexturedMeshScene& operator=(const TexturedMeshScene& t);
};

