#include "stdafx.h"
#include "GameScene.h"
#include "GameObject.h"
#include "Debug.h"
#include "PhysxManager.h"
#include "PhysxHelpers.h"
#include "GameBase.h"
#include "ManualCamera.h"

bool GameScene::m_PhysXFrameStepping = false;
float GameScene::m_PhysXStepTime = 0.f;

GameScene::GameScene(wstring sceneName, GameSettings* settings):
	m_vecGameObjects(vector<std::unique_ptr<GameObject>>()),
	m_IsInitialized(false),
	m_pPhysxScene(nullptr),
	m_SceneName(sceneName),
	m_DrawPhysx(false),
	m_pGameSettings(settings)
{
	//create Timer
	m_pTimer = std::make_unique<GameTimer>();
	m_pTimer->Reset();
	m_pTimer->Stop();
}


GameScene::~GameScene(void)
{
}

void GameScene::AddGameObject(GameObject* pObject)
{
	auto it = find_if(m_vecGameObjects.begin(), m_vecGameObjects.end(), [pObject](std::unique_ptr<GameObject> &pChildObject) {return pObject == pChildObject.get(); });
	if(it == m_vecGameObjects.end())
	{
		m_vecGameObjects.emplace_back(std::unique_ptr<GameObject>(pObject));
		pObject->m_pScene = this;
		pObject->m_pParent = nullptr;

		RootInitializeSceneObject(pObject);

		SceneManager::UpdateChanges();
	}
}

void GameScene::RootInitializeSceneObject(GameObject * pObject)
{
	if (m_IsInitialized)
		pObject->RootInitialize(m_pVkContext);
}

void GameScene::RemoveGameObject(GameObject* pObject)
{
	auto it = find_if(m_vecGameObjects.begin(), m_vecGameObjects.end(), [pObject](std::unique_ptr<GameObject> &pChildObject) {return pObject == pChildObject.get(); });
	if(it != m_vecGameObjects.end())
	{
		m_vecGameObjects.erase(it);
		pObject->m_pScene = nullptr;
		pObject->m_pParent = nullptr;

		SceneManager::UpdateChanges();
	}
}

void GameScene::SetVkCamera(BaseCamera* pCamera)
{
	if (!pCamera)
		pCamera = m_pDefaultVkCamera.get();

	m_pActiveVkCamera = pCamera;
}

void GameScene::RootInitialize(VulkanContext* vkContext)
{
	if(m_IsInitialized)
		return;

	m_pVkContext = vkContext;

	//Initialize PhysX Scene
	m_pPhysxScene = PhysxManager::GetInstance()->CreateScene(this);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	m_pPhysxScene->setSimulationEventCallback(this);

	//Default VkCamera
	m_pDefaultVkCamera = std::make_unique<ManualCamera>(m_pGameSettings);
	SetVkCamera(m_pDefaultVkCamera.get());

#if defined(DEBUG) || defined(_DEBUG)
	EnablePhysxDebugRendering(true);
#endif

	Initialize();

	for(auto& pObject: m_vecGameObjects)
	{
		pObject->RootInitialize(m_pVkContext);
	}

	m_IsInitialized = true;
}

void GameScene::RootUpdate()
{
	m_pTimer->Update();

	if (m_pActiveVkCamera)
		m_pActiveVkCamera->Update(this);

	Update();

	for(auto& pObject:m_vecGameObjects)
	{
		pObject->RootUpdate(m_pVkContext);
	}

	for(auto& pObject:m_vecGameObjects)
	{
		pObject->CalculateWorldMatrix(mat4()); //or inside GameObject itself??
	}

	if(m_pTimer->IsRunning())
	{
		if(m_PhysXFrameStepping)
		{
			if(m_PhysXStepTime > 0.f)
			{
				m_pPhysxScene->simulate(m_PhysXStepTime);
				m_pPhysxScene->fetchResults(true);
				m_PhysXStepTime = 0.f;
			}
			else if(m_PhysXStepTime < 0.f)
			{
				m_pPhysxScene->simulate(GetTimer()->GetElapsedSecondsF());
				m_pPhysxScene->fetchResults(true);
			}
		}
		else
		{
			m_pPhysxScene->simulate(GetTimer()->GetElapsedSecondsF() * m_PhysXSimulationSpeed); //fast way to get more realistic behaviour TODO: change
			m_pPhysxScene->fetchResults(true);
		}
	}


	//Send Camera to PVD
	if(m_pPhysxScene->getPhysics().getPvdConnectionManager())
	{
		m_pPhysxScene->getPhysics().getPvdConnectionManager()->setCamera("SceneCam"
			, ToPxVec3(m_pActiveVkCamera->GetPosition())
			, ToPxVec3(m_pActiveVkCamera->GetUp())
			, ToPxVec3(m_pActiveVkCamera->GetTarget()));
	}
}

void GameScene::RootRecordVulkanDrawCommands(VkCommandBuffer cmdBuffer)
{
	RecordVulkanDrawCommands(cmdBuffer);

	for (auto& pObject : m_vecGameObjects)
	{
		pObject->RootRecordVulkanDrawCommands(cmdBuffer);
	}
}

void GameScene::RootRecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex)
{
	RecordVulkanDrawCommands(cmdBuffer, frameBufferIndex);

	for (auto& pObject : m_vecGameObjects)
	{
		pObject->RootRecordVulkanDrawCommands(cmdBuffer, frameBufferIndex);
	}
}

void GameScene::RootWindowStateChanged(int state, bool active)
{
	//TIMER
	if(state == 0)
	{
		if(active)m_pTimer->Start();
		else m_pTimer->Stop();
	}
}

void GameScene::RootSceneActivated()
{
	//Stop Timer
	m_pTimer->Start();

	SceneActivated();
}

void GameScene::RootSceneDeactivated()
{
	//Start Timer
	m_pTimer->Stop();

	SceneDeactivated();
}
