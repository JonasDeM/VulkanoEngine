#pragma once
#include "stdafx.h"
#include "ManualCamera.h"
#include "SceneManager.h"
#include "GameScene.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "Debug.h"
#include "GameTimer.h"
#include "GameSettings.h"

int ManualCamera::m_UpKey = GLFW_KEY_W;
int ManualCamera::m_LeftKey = GLFW_KEY_A;
bool ManualCamera::m_IsQWERTY = true;

ManualCamera::ManualCamera(GameSettings* pGameSettings) :
	BaseCamera(pGameSettings),
	m_TotalYaw(0),
	m_TotalPitch(0),
	m_Speed(30.0f),
	m_MouseDownPreviousFrame(false),
	m_Direction(vec3(0, 0, 1.0f))
{
	auto ext = pGameSettings->GetWindowExtent(); 
	BuildProjection(glm::quarter_pi<float>(), (float)ext.width / (float)ext.height, 0.01f, 500.0f);
}

void ManualCamera::SetPositionAndTarget(vec3 pos, vec3 tar)
{
	m_Target = tar;
	m_Position = pos;

	m_Direction = normalize(tar - pos);
	m_Forward = m_Direction;

	m_TotalYaw = atan2(-m_Direction.x, m_Direction.z);
	m_TotalPitch = asin(m_Direction.y);
}

void ManualCamera::Update(const GameScene* scene)
{
	if (!SceneManager::IsInputEnabled())
		return;

	GLFWwindow* window = scene->GetWindow();
	bool mouseDownNow = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	float deltaTime = (float)scene->GetTimer()->GetElapsedSeconds();

	if (mouseDownNow && !m_MouseDownPreviousFrame)
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		m_PrevCursorPosition = vec2(x, y);
		m_MouseDownPreviousFrame = true;
	}
	else if (mouseDownNow)
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		vec2 currCursorPosition(x, y);

		//Debug::Log(Debug::LogLevel::Info, std::to_wstring(m_PrevCursorPosition.x) + L" - " + std::to_wstring(m_PrevCursorPosition.y));
		//Debug::Log(Debug::LogLevel::Info, std::to_wstring(currCursorPosition.x) + L" - " + std::to_wstring(currCursorPosition.y));

		float xDiff = -(currCursorPosition.x - m_PrevCursorPosition.x) / 200.0f;
		float yDiff = -(currCursorPosition.y - m_PrevCursorPosition.y) / 200.0f;

		//Debug::Log(Debug::LogLevel::Info, std::to_wstring(xDiff) + L" - " + std::to_wstring(yDiff));

		m_TotalYaw += xDiff * glm::radians(45.0f);
		m_TotalPitch += yDiff * glm::radians(45.0f);

		mat4 rotMat = glm::eulerAngleXY(m_TotalPitch, m_TotalYaw);

		vec4 forwardVec = { 0,0, 1.0f,0 };
		m_Direction = forwardVec * rotMat;

		m_PrevCursorPosition = currCursorPosition;
	}
	else m_MouseDownPreviousFrame = false;

	m_Right = glm::cross(m_Direction, vec3(0,-1,0));

	if (glfwGetKey(window, m_UpKey) == GLFW_PRESS)
		m_Position += m_Direction * m_Speed * deltaTime;
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_Position -= m_Direction * m_Speed * deltaTime;

	if (glfwGetKey(window, m_LeftKey) == GLFW_PRESS)
		m_Position -= m_Right * m_Speed * deltaTime;
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_Position += m_Right * m_Speed * deltaTime;

	m_Forward = m_Direction;
	m_Up = glm::cross(m_Right, m_Direction);

	m_Target = m_Position + m_Direction;
	m_View = glm::lookAt(m_Position, m_Target, m_Up);
	BuildViewProjection();
}

void ManualCamera::SetQWERTY(bool isQwerty)
{
	m_IsQWERTY = isQwerty;
	if (m_IsQWERTY)
		Debug::Log(Debug::LogLevel::Info, L"QWERTY controls Active!");
	else
		Debug::Log(Debug::LogLevel::Info, L"AZERTY controls Active!");
	m_LeftKey = m_IsQWERTY ? GLFW_KEY_A : GLFW_KEY_Q;
	m_UpKey = m_IsQWERTY ? GLFW_KEY_W : GLFW_KEY_Z;
}

void ManualCamera::ToggleQWERTY_AZERTY()
{
	SetQWERTY(!m_IsQWERTY);
}
