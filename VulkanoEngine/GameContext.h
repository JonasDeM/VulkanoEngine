#pragma once
#include "GameTimer.h"
#include "BaseCamera.h"
#include "VkDevice_Ext.h"
#include "VkPosColTexPipeline_Ext.h"
#include "VkDebugPipeline_Ext.h"
#include "glfw/glfw3.h"

class GameContext final
{
public:
	GameContext(GameSettings* pGameSettings);
	~GameContext(void);

	GameSettings* GetSettings() const { return m_pGameSettings; }

private:

	GameSettings* m_pGameSettings;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	GameContext(const GameContext& t);
	GameContext& operator=(const GameContext& t);
};

