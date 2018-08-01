#pragma once
#include "stdafx.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "GameTimer.h"
#include "Debug.h"
#include "GameBase.h"
#include "VulkanContext.h"
#include <GLFW/glfw3.h>


vector<std::unique_ptr<GameScene>> SceneManager::m_vecScenes = vector<std::unique_ptr<GameScene>>();
bool SceneManager::m_IsInitialized = false;
GameScene* SceneManager::m_pActiveScene = nullptr;
GameScene* SceneManager::m_pNewActiveScene = nullptr;
bool SceneManager::m_InputEnabled = true;
VulkanContext* SceneManager::m_pVulkanContext = nullptr;
int SceneManager::m_NewActiveSceneWindowNr = 0;
std::array<GLFWwindow*, SceneManager::MAX_REGISTERED_WINDOWS> SceneManager::m_RegisteredWindows = std::array<GLFWwindow*, MAX_REGISTERED_WINDOWS>{};


void SceneManager::CleanUp()
{
	m_vecScenes.clear();
}

void SceneManager::AddGameScene(GameScene* scene)
{
	auto it = find_if(m_vecScenes.begin(), m_vecScenes.end(), [scene](std::unique_ptr<GameScene> &managedScene) {return scene == managedScene.get(); });

	if (it == m_vecScenes.end())
	{
		if (m_IsInitialized)
			scene->RootInitialize(m_pVulkanContext);

		if (m_pActiveScene == nullptr && m_pNewActiveScene == nullptr)
			m_pNewActiveScene = scene;

		m_vecScenes.emplace_back(std::unique_ptr<GameScene>(scene));
	}
}

void SceneManager::RemoveGameScene(unsigned int sceneIndex)
{
	if (m_vecScenes.size() < sceneIndex)
		m_vecScenes.erase(m_vecScenes.begin() + sceneIndex);
	else
		Debug::LogError(L"Attempt to remove scene by index, but index was not in range.");
}

void SceneManager::RemoveGameScene(const GameScene * scene)
{
	auto it = find_if(m_vecScenes.begin(), m_vecScenes.end(), [scene](const std::unique_ptr<GameScene> &sceneToFind) {
		return scene == sceneToFind.get();
	});

	if (it != m_vecScenes.end())
		m_vecScenes.erase(it);
}


void SceneManager::NextScene()
{
	for (size_t i = 0; i < m_vecScenes.size(); ++i)
	{
		if (m_vecScenes[i].get() == m_pActiveScene)
		{
			auto nextScene = (++i) % m_vecScenes.size();
			m_pNewActiveScene = m_vecScenes[nextScene].get();
			break;
		}
	}
}

void SceneManager::PreviousScene()
{
	for (size_t i = 0; i < m_vecScenes.size(); ++i)
	{
		if (m_vecScenes[i].get() == m_pActiveScene)
		{
			if (i == 0) i = m_vecScenes.size();
			--i;
			m_pNewActiveScene = m_vecScenes[i].get();
			break;
		}
	}
}

void SceneManager::FlagDrawChanges()
{
	m_pVulkanContext->SetFlags(VkContextFlags::InvalidDrawCommandBuffers);
}

const int SceneManager::RegisterWindow(GLFWwindow* window, int windowNr)
{
	if (windowNr = -1)
	{
		int nr = 0;
		while (m_RegisteredWindows[nr] != nullptr && nr < MAX_REGISTERED_WINDOWS)
		{
			++nr;
		}
		if (nr == MAX_REGISTERED_WINDOWS)
			throw new runtime_error("No available slots remaining. If you want to overwrite, specify a number.");
		m_RegisteredWindows[nr] = window;
		return nr;
	}
	if (windowNr >= 0 && windowNr < MAX_REGISTERED_WINDOWS)
	{
		m_RegisteredWindows[windowNr] = window;
		return windowNr;
	}

	throw new runtime_error("invalid window number");
}

void SceneManager::SetActiveGameScene(wstring sceneName, int windowNr)
{
	auto it = find_if(m_vecScenes.begin(), m_vecScenes.end(), [sceneName](const std::unique_ptr<GameScene> &scene)
	{
		return (wcscmp(scene->m_SceneName.c_str(), sceneName.c_str()) == 0);
	});

	if (it != m_vecScenes.end() && m_RegisteredWindows[windowNr] != nullptr)
	{
		m_pNewActiveScene = it->get();
		m_NewActiveSceneWindowNr = windowNr;
	}
}

void SceneManager::Initialize(VulkanContext* vkContext)
{
	m_pVulkanContext = vkContext;

	for (auto&  scene : m_vecScenes)
	{
		scene->RootInitialize(vkContext);
	}
	UpdateIfNewActiveScene();
}

void SceneManager::WindowStateChanged(int state, bool active)
{
	if (state == 0 && m_pActiveScene)
	{
		m_pActiveScene->RootWindowStateChanged(state, active);
	}
}

void SceneManager::UpdateIfNewActiveScene()
{
	if (m_pNewActiveScene != nullptr)
	{
		FlagDrawChanges();

		//Deactivate the current active scene
		if (m_pActiveScene != nullptr)
		{
			m_pActiveScene->RootSceneDeactivated();
			m_pActiveScene->SetWindow(nullptr);
		}

		//Set New Scene
		m_pActiveScene = m_pNewActiveScene;
		m_pNewActiveScene = nullptr;

		//Activate the new scene
		m_pActiveScene->RootSceneActivated();
		m_pActiveScene->SetWindow(m_RegisteredWindows[m_NewActiveSceneWindowNr]);
		glfwSetWindowUserPointer(m_RegisteredWindows[m_NewActiveSceneWindowNr], m_pActiveScene);
	}
}

void SceneManager::Update()
{
	UpdateIfNewActiveScene();

	if (m_pActiveScene != nullptr)
	{
		m_pActiveScene->RootUpdate();

		//Update DebugRenderdata
		if (m_pActiveScene->m_DrawPhysx)
			Debug::DrawPhysX(m_pActiveScene->GetPhysxScene());
		Debug::UpdateRenderData(m_pVulkanContext, m_pActiveScene);
	}
}

void SceneManager::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex)
{
	if (m_pActiveScene != nullptr)
		m_pActiveScene->RootRecordVulkanDrawCommands(cmdBuffer, frameBufferIndex);
}

