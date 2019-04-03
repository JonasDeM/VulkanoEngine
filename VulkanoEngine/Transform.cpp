#include "stdafx.h"
#include "Transform.h"
#include "glm/gtx/euler_angles.hpp"

Transform Transform::GetParent()
{
	return GetData().m_Parent;
}

Transform Transform::GetScene()
{
	Transform t = *this;
	Transform parent = t.GetParent();

	while (parent.IsValid())
	{
		t = parent;
		parent = parent.GetParent();
	}
	return t;
}

void Transform::AddChild(Transform child)
{
}

void Transform::RemoveChild(Transform child)
{
}

void Transform::SetPosition(const glm::vec3 & pos)
{
	GetData().m_Position = pos;
}

void Transform::GetPosition(glm::vec3 & pos)
{
	pos = GetData().m_Position;
}

void Transform::SetRotation(const quat & rot)
{
	GetData().m_Rotation = rot;
}

void Transform::GetRotation(quat & rot)
{
	rot = GetData().m_Rotation;
}

void Transform::SetEulerRotation(const vec3 & rot)
{
	GetData().m_Rotation = quat(glm::eulerAngleYXZ(rot.y, rot.x, rot.z));
}

void Transform::GetEulerRotation(vec3 & rot)
{
	rot = eulerAngles(GetData().m_Rotation);
}

void Transform::SetEulerRotationInDegrees(const vec3 & rot)
{
	GetData().m_Rotation = quat(glm::eulerAngleYXZ((rot.y / 180.0f)*glm::pi<float>(), (rot.x / 180.0f)*glm::pi<float>(), (rot.z / 180.0f)*glm::pi<float>()));
}

void Transform::GetEulerRotationInDegrees(vec3 & rot)
{
	rot = eulerAngles(GetData().m_Rotation);
	rot = (rot * 180.0f) / glm::pi<float>();
}

void Transform::SetScale(const vec3 & scale)
{
	GetData().m_Scale = scale;
}

void Transform::GetScale(vec3 & scale)
{
	scale = GetData().m_Scale;
}

void Transform::Translate(const vec3 & delta)
{
	GetData().m_Position += delta;
}

void Transform::Rotate(const quat& delta)
{
	GetData().m_Rotation *= delta;
}

void Transform::RotateEuler(const vec3 & delta)
{
	GetData().m_Rotation *= quat(glm::eulerAngleYXZ(delta.y, delta.x, delta.z));
}

void Transform::RotateEulerDegrees(const vec3 & delta)
{
	GetData().m_Rotation *= quat(glm::eulerAngleYXZ((delta.y / 180.0f)*glm::pi<float>(), (delta.x / 180.0f)*glm::pi<float>(), (delta.z / 180.0f)*glm::pi<float>()));
}

void Transform::Scale(const vec3 & delta)
{
	GetData().m_Scale *= delta;
}
