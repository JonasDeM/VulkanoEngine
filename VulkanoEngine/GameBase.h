#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include "HandleUtilities.h"
#include <iostream>
#include <vector>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include "FileUtils.h"
#include <set>
#include <memory>
#include "VkSwapchainKHR_Ext.h"
#include "VkPhysicalDevice_Ext.h"
#include "VkDevice_Ext.h"
#include "VkInstance_Ext.h"
#include <vulkan/vulkan.h>
#include "string.h"
#include "GameSettings.h"

class VkDebugPipeline_Ext;
class VulkanDrawer;


class GameBase
{
public:
	GameBase(void);
	virtual ~GameBase(void);
	void RunGame();

protected:

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void OnPreparingGame() = 0;

	unique_ptr_del<GLFWwindow> m_pWindow = nullptr;
	bool m_IsActive = true;
	unique_ptr<GameSettings> m_pGameSettings = nullptr;
	unique_ptr<VulkanDrawer> m_pVulkanDrawer = nullptr;
	
private:
	//Functions
	void CreateGLFWWindow();

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	GameBase(const GameBase& t);
	GameBase& operator=(const GameBase& t);
};

