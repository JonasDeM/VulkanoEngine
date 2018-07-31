#pragma once
#include "stdafx.h"
#include "MainGame.h"
#include "SceneManager.h"
#include "Debug.h"
#include "ManualCamera.h"

#define DISPLAY_ENGINE
#define REST_OF_SCENES
#ifdef DISPLAY_ENGINE
#include "TexturedMeshScene.h"
#include "BouncingBallScene.h"
#include "BoxDemoScene.h"
#include "PhysXMeshTestScene_2.h"
#include "BallPoolScene.h"
#endif
#ifdef REST_OF_SCENES
#include "GimbalRigScene.h"
#include "CarouselScene.h"
#include "InputTestScene.h"
#endif

bool MainGame::m_PrintFPS = false;

void MainGame::OnPreparingGame()
{
	m_pGameSettings->SetVSync(true);
	m_pGameSettings->SetClearColor(VkClearColorValue{0.7f,0.7f,0.7f});
}

void MainGame::Update()
{
	auto pActiveScene = SceneManager::GetActiveGameScene();
	if (m_pPrevScene != pActiveScene)
		m_PrevFPSCheck = 0;

	if (pActiveScene && m_PrintFPS && pActiveScene->GetTimer()->GetTotalSeconds() >= m_PrevFPSCheck)
	{
		m_PrevFPSCheck = (float)pActiveScene->GetTimer()->GetTotalSeconds()+1.0f;
		wstringstream ss;
		ss << L"FPS: " << SceneManager::GetActiveGameScene()->GetTimer()->GetFPS() << "\n";
		Debug::LogInfo(ss.str());
	}
	m_pPrevScene = pActiveScene;
}

void MainGame::Initialize()
{
#ifdef DISPLAY_ENGINE
	SceneManager::AddGameScene(new BoxDemoScene(m_pGameSettings.get()));
	SceneManager::AddGameScene(new TexturedMeshScene(m_pGameSettings.get()));
	SceneManager::AddGameScene(new BallPoolScene(m_pGameSettings.get()));
	SceneManager::AddGameScene(new BouncingBallScene(m_pGameSettings.get()));
	SceneManager::AddGameScene(new PhysXMeshTestScene_2(m_pGameSettings.get()));
#endif
#ifdef REST_OF_SCENES
	SceneManager::AddGameScene(new CarouselScene(m_pGameSettings.get()));
	SceneManager::AddGameScene(new GimbalRigScene(m_pGameSettings.get()));
	SceneManager::AddGameScene(new InputTestScene(m_pGameSettings.get()));
#endif
	std::cout << "-----------------------------------------------" << '\n' ;
	Debug::LogInfo(L"Use WASD + Hold and move Mouse to navigate in the scene.");
	Debug::LogInfo(L"Have an AZERTY keyboard? Press \'P\' to toggle to ZQSD controls and back.");
	Debug::LogInfo(L"Press \'N\'/\'M\' to go the the next/previous scene.");
	Debug::LogInfo(L"Press \'G\' to TOGGLE Debug Visuals.");
	Debug::LogInfo(L"Press \'F\' to TOGGLE the FPS counter in the Console Window.");
	std::cout << "-----------------------------------------------" << '\n';

	//Global window callbacks
	glfwSetWindowSizeCallback(m_pWindow.get(), MainGame::OnWindowResized);
	glfwSetKeyCallback(m_pWindow.get(), MainGame::OnKeyPress);
	glfwSetWindowUserPointer(m_pWindow.get(), nullptr);
}

void MainGame::OnWindowResized(GLFWwindow * window, int width, int height)
{
	if (width == 0 || height == 0) return;

	void* p = glfwGetWindowUserPointer(window);
	if (p == nullptr) return;

	GameScene* scene = reinterpret_cast<GameScene*>(p);
	scene->GetSettings()->SetBackBufferExtent(VkExtent2D{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
}

void MainGame::OnKeyPress(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_M)
			SceneManager::NextScene();
		if (key == GLFW_KEY_N)
			SceneManager::PreviousScene();
		if (key == GLFW_KEY_G)
			Debug::ToggleDebugRenderer();
		if (key == GLFW_KEY_F)
			m_PrintFPS = !m_PrintFPS;
		if (key == GLFW_KEY_P)
			ManualCamera::ToggleQWERTY_AZERTY();
	}
}

