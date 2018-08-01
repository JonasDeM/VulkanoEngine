#pragma once

#include "HandleUtilities.h"
#include <memory>
#include "GameSettings.h"

class VkDebugPipeline_Ext;
class VulkanDrawer;
struct GLFWwindow;

class GameBase
{
public:
	GameBase(void);
	virtual ~GameBase(void);
	GameBase(const GameBase& t) = delete;
	GameBase& operator=(const GameBase& t) = delete;

	void RunGame();
protected:

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void OnPreparingGame() = 0;

	unique_ptr_del<GLFWwindow> m_pWindow = nullptr;
	bool m_IsActive = true;
	std::unique_ptr<GameSettings> m_pGameSettings = nullptr;
	std::unique_ptr<VulkanDrawer> m_pVulkanDrawer = nullptr;
	
private:
	//Functions
	void CreateGLFWWindow();
};

