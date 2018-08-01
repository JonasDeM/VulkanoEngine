#pragma once
using namespace std;

class GameScene;
struct GLFWwindow;
class VulkanContext;

class SceneManager final
{
public:
	//Explicitly deleting construction of an object of this class 
	SceneManager() = delete;

	static void AddGameScene(GameScene* scene); //Takes ownership
	static void RemoveGameScene(unsigned int sceneIndex);
	static void RemoveGameScene(const GameScene* scene);
	static void SetActiveGameScene(wstring sceneName, int windowNr=0);
	static GameScene* GetActiveGameScene() { return m_pActiveScene; }
	static void NextScene();
	static void PreviousScene();
	static bool IsInputEnabled() { return m_InputEnabled; }
	static void FlagDrawChanges();

	static const int MAX_REGISTERED_WINDOWS = 10;
	static const int RegisterWindow(GLFWwindow* window, int windowNr = -1); // -1 means to take the next available nr, returns windowNr

private:

	friend class GameBase;
	friend class VulkanDrawer;

	static void Initialize(VulkanContext* vkContext);
	static void Update();
	static void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex);
	static void CleanUp();
	static void WindowStateChanged(int state, bool active);
	static void UpdateIfNewActiveScene();

	static vector<std::unique_ptr<GameScene>> m_vecScenes;
	static bool m_IsInitialized;
	static GameScene* m_pActiveScene, *m_pNewActiveScene;

	static VulkanContext* m_pVulkanContext;
	static bool m_InputEnabled;

	static std::array<GLFWwindow*, MAX_REGISTERED_WINDOWS> m_RegisteredWindows;
	static int m_NewActiveSceneWindowNr;
};

