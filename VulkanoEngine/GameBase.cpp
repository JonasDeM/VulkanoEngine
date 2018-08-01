#pragma once
#include "stdafx.h"

#include "GameBase.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "ContentManager.h"
#include "Debug.h"
#include "PhysxManager.h"
#include "PipelineManager.h"
#include "VulkanDrawer.h"
#include <GLFW/glfw3.h>
#include "vkDevice_Ext.h"


GameBase::GameBase(void){
	m_pGameSettings = std::make_unique<GameSettings>();
}

GameBase::~GameBase(void)
{
	//CLEAN-UP STATIC OR SINGLETON MANAGERS
	SceneManager::CleanUp();
	ContentManager::CleanUp();
	PipelineManager::CleanUp();
	PhysxManager::DestroyInstance();
	Debug::CleanUp();
}

void GameBase::RunGame()
{
#pragma region
	//PREPARE GAME
	Debug::StartProfileTimer();
	OnPreparingGame();

	//1. Create the Window
	CreateGLFWWindow(); 

	//2.Initialize Vulkan
	m_pVulkanDrawer = std::make_unique<VulkanDrawer>(m_pWindow.get());
	m_pGameSettings->AttachVkContextObserver(m_pVulkanDrawer.get());

	Debug::StartProfileTimer();
	//4. Initialize Managers
	PipelineManager::Initialize(m_pVulkanDrawer.get());
	ContentManager::Initialize(m_pVulkanDrawer.get());
	Debug::Initialize(m_pVulkanDrawer.get()); //Only possible after DeviceCreation & ContentManager Init
	PhysxManager::GetInstance();

	//5. Initialize the GameBase
	Initialize();

	//6. Register all windows to scenemanager and the initialize
	SceneManager::RegisterWindow(m_pWindow.get());
	SceneManager::Initialize(m_pVulkanDrawer.get());

	m_pVulkanDrawer->CreateDrawCommandBuffers(m_pGameSettings.get());
	Debug::PrintProfileInterval(L"Initialization");
#pragma endregion INITIALIZATION

#pragma region
	while (!glfwWindowShouldClose(m_pWindow.get())) {
		//Debug::LogInfo(L"New Frame");
		glfwPollEvents();
		//Debug::PrintProfileInterval(L"GLFW");
		//GAME LOOP
		Update();
		SceneManager::Update();
		//Debug::PrintProfileInterval(L"Update");
		m_pVulkanDrawer->VkDrawFrame(m_pGameSettings.get());
		//Debug::PrintProfileInterval(L"Draw");
	}
	vkDeviceWaitIdle(*m_pVulkanDrawer->GetVkDevice()); //wait for all async processes to complete

	glfwTerminate();

#pragma endregion GAMELOOP

#pragma region
	//Clean-up is handled by the Destructor :)
	//See ~GameBase()

	//6. Finished
	Debug::LogInfo(L"Finished");
#pragma endregion CLEAN-UP
}

void GameBase::CreateGLFWWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	auto extent = m_pGameSettings->GetWindowExtent();
	m_pWindow = unique_ptr_del<GLFWwindow>(glfwCreateWindow(extent.width, extent.height, "Window - Vulkan", nullptr, nullptr), [](GLFWwindow* ptr) {glfwDestroyWindow(ptr); });
}

