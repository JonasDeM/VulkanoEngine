#include "stdafx.h"
#include "BaseCamera.h"

#include "GameSettings.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

BaseCamera::BaseCamera(GameSettings* pGameSettings) :
	m_View(mat4(1.0f)),
	m_Projection(mat4(1.0f)),
	m_ViewProjection(mat4(1.0f)),
	m_Up(vec3(0, 1, 0)),
	m_Right(vec3(1, 0, 0)),
	m_Forward(vec3(0, 0, 1)),
	m_Position(vec3(0, 2, -5)),
	m_Target(vec3(0, 0, 0)),
	m_IsQWERTY(true)
{
	pGameSettings->AttachCameraObserver(this);
}

void BaseCamera::BuildView()
{
	m_Target = m_Position+m_Forward;
	m_View = glm::lookAtRH(m_Position, m_Target, m_Up);
	BuildViewProjection();
}

void BaseCamera::BuildProjection(float fov, float aspectRatio, float nearZ, float farZ)
{
	m_Projection = glm::perspectiveRH(fov, aspectRatio, nearZ, farZ);
	BuildViewProjection();
}

void BaseCamera::SetAspectRatio(float aspectRatio)
{
	BuildProjection(glm::quarter_pi<float>(), aspectRatio, 0.01f, 500.0f);
}

inline void BaseCamera::BuildViewProjection()
{
	m_ViewProjection = m_Projection * m_View;
}

void BaseCamera::SetQWERTY(bool isQwerty)
{
	m_IsQWERTY = isQwerty;
}