#pragma once
#include "GameBase.h"

class MainGame: public GameBase
{
public:
	MainGame(void) {};
	~MainGame(void) {};

protected: 

	virtual void OnPreparingGame() override;
	virtual void Update() override;
	virtual void Initialize() override;

	//callbacks
	static void OnWindowResized(GLFWwindow* window, int width, int height);
	static void OnKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);

	float m_PrevFPSCheck = 0.0f;
	static bool m_PrintFPS; //static because of glfw callback

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	MainGame(const MainGame& t);
	MainGame& operator=(const MainGame& t);
};

