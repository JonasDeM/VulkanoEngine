#pragma once
#include "BaseCamera.h"

class ManualCamera : public BaseCamera
{
public:
	ManualCamera(GameSettings* pGameSettings);
	~ManualCamera(void) {};

	virtual void Update(const GameScene* scene) override;
	static void SetQWERTY(bool isQuerty = true);
	static void ToggleQWERTY_AZERTY();
	void SetPositionAndTarget(vec3 pos, vec3 tar);

private:
	vec2 m_PrevCursorPosition;
		
	float m_TotalYaw, m_TotalPitch;
	float m_Speed;
	bool m_MouseDownPreviousFrame;
	vec3 m_Direction;

	//holds acii wasd or zqsd
	static bool m_IsQWERTY;
	static int m_UpKey, m_LeftKey;
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	ManualCamera(const ManualCamera& t);
	ManualCamera& operator=(const ManualCamera& t);
};