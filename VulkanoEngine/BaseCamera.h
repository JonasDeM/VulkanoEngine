#pragma once
class GameSettings;
class GameContext;
class GameScene;
using namespace glm;

class BaseCamera
{
public:
	BaseCamera(GameSettings* pGameSettings);
	~BaseCamera(void) {};
	void SetQWERTY(bool isQuerty = true);

	mat4 GetView() const { return m_View; }
	mat4 GetProjection() const { return m_Projection; }
	mat4 GetViewProjection() const { return m_ViewProjection; }

	vec3 GetPosition() const { return m_Position; }
	vec3 GetUp() const { return m_Up; }
	vec3 GetRight() const { return m_Right; }
	vec3 GetForward() const { return m_Forward; }
	vec3 GetTarget() const { return m_Target; }

	virtual void BuildView();
	virtual void BuildProjection(float fov, float aspectRatio, float nearZ, float farZ);
	virtual void Update(const GameScene* scene) {};

	void SetAspectRatio(float aspectRatio);

protected:
	void BuildViewProjection();
	bool m_IsQWERTY;

	vec3 m_Up, m_Right, m_Forward, m_Position, m_Target;
	mat4 m_View, m_Projection, m_ViewProjection;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	BaseCamera(const BaseCamera& t);
	BaseCamera& operator=(const BaseCamera& t);
};