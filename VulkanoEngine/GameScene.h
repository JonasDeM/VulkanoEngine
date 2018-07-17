#pragma once

#include "SceneManager.h"
#include "GameContext.h"
#include "BaseCamera.h"

class GameObject;
class GameContext;
class BaseCamera;

class GameScene: public PxSimulationEventCallback
{
public:
	GameScene(wstring sceneName, GameSettings* settings);
	virtual ~GameScene(void);

	//GET
	PxScene* GetPhysxScene() const { return m_pPhysxScene; }
	BaseCamera* GetCamera() const { return m_pActiveVkCamera; }
	GameTimer* GetTimer() const { return m_pTimer.get(); }
	GLFWwindow* GetWindow() const { return m_pSceneWindow; }
	GameSettings* GetSettings() const { return m_pGameSettings; }

	//SET
	void SetVkCamera(BaseCamera * pCamera);
	void SetWindow(GLFWwindow* window) { m_pSceneWindow = window; }
	void EnablePhysxDebugRendering(bool enable) { m_DrawPhysx = enable; }

	//METHODS
	void AddGameObject(GameObject* pObject);
	void RemoveGameObject(GameObject* pObject);
	void RootInitializeSceneObject(GameObject* pObject);

	//Debug Methods
	static void EnablePhysXFrameStepping(bool enable) { m_PhysXFrameStepping = enable; }
	static void NextPhysXFrame(float time = 0.03f) { m_PhysXStepTime = time; }

protected:

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer) {};
	virtual void SceneActivated(){}
	virtual void SceneDeactivated(){}

	//PhysX SimulationEventCalbacks
	virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override {}
	virtual void onWake(PxActor **actors, PxU32 count) override {}
	virtual void onSleep(PxActor **actors, PxU32 count) override {}
	virtual void onContact(const PxContactPairHeader & pairHeader, const PxContactPair *pairs, PxU32 nbPairs) override {}
	virtual void onTrigger(PxTriggerPair *pairs, PxU32 count) override {}

	vector<std::unique_ptr<GameObject>> m_vecGameObjects;
	bool m_IsInitialized;

	//Physx
	PxScene *m_pPhysxScene;
	const int m_PhysXSimulationSpeed = 2; //fast way to get more realistic behaviour TODO: change

private:
	friend SceneManager;

	void RootInitialize(VulkanContext* vkContext);
	void RootUpdate();
	void RootRecordVulkanDrawCommands(VkCommandBuffer cmdBuffer);
	void RootSceneActivated();
	void RootSceneDeactivated();
	void RootWindowStateChanged(int state, bool active);

	BaseCamera* m_pActiveVkCamera;
	std::unique_ptr<GameTimer> m_pTimer;

	GLFWwindow* m_pSceneWindow = nullptr;
	VulkanContext* m_pVkContext;
	GameSettings* m_pGameSettings;
	std::unique_ptr<BaseCamera> m_pDefaultVkCamera;
	wstring m_SceneName;
	bool m_DrawPhysx;

	static bool m_PhysXFrameStepping;
	static float m_PhysXStepTime;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	GameScene(const GameScene& t) = delete;
	GameScene& operator=(const GameScene& t) = delete;
};

