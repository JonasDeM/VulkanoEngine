#include "stdafx.h"
#include "Transform.h"
#include <algorithm>
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

Transform Transform::GetParent()
{
	return m_pData->m_Parent;
}

void Transform::SetParent(Transform parent)
{
	GetParent().RemoveChild(*this);
	parent.AddChild(*this);
}

Transform Transform::GetRootParent()
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

Transform Transform::GetChild(size_t index)
{
	assert(index < m_pData->m_Children.size());
	return m_pData->m_Children[index];
}

size_t Transform::GetChildCount()
{
	return m_pData->m_Children.size();
}

void Transform::AddChild(Transform child)
{
	m_pData->m_Children.push_back(child);
}

void Transform::RemoveChild(Transform child)
{
	auto& children = m_pData->m_Children;
	children.erase(std::find(children.begin(), children.end(), child));
}

void Transform::SetPosition(const glm::vec3 & pos)
{
	m_pData->m_Position = pos;
	m_pData->m_Flags.PositionChanged();
}

vec3 Transform::GetPosition()
{
	return m_pData->m_Position;
}

void Transform::SetRotation(const quat & rot)
{
	m_pData->m_Rotation = rot;
	m_pData->m_Flags.RotationChanged();
}

quat Transform::GetRotation()
{
	return m_pData->m_Rotation;
}

void Transform::SetEulerRotation(const vec3 & rot)
{
	m_pData->m_Rotation = quat(glm::eulerAngleYXZ(rot.y, rot.x, rot.z));
	m_pData->m_Flags.RotationChanged();
}

vec3 Transform::GetEulerRotation()
{
	return eulerAngles(m_pData->m_Rotation);
}

void Transform::SetEulerRotationInDegrees(const vec3 & rot)
{
	m_pData->m_Rotation = quat(glm::eulerAngleYXZ((rot.y / 180.0f)*glm::pi<float>(), (rot.x / 180.0f)*glm::pi<float>(), (rot.z / 180.0f)*glm::pi<float>()));
	m_pData->m_Flags.RotationChanged();
}

vec3 Transform::GetEulerRotationInDegrees()
{
	return (eulerAngles(m_pData->m_Rotation) * 180.0f) / glm::pi<float>();
}

void Transform::SetScale(const vec3 & scale)
{
	m_pData->m_Scale = scale;
	m_pData->m_Flags.ScaleChanged();
}

vec3 Transform::GetScale()
{
	return m_pData->m_Scale;
}

void Transform::Translate(const vec3 & delta)
{
	m_pData->m_Position += delta;
	m_pData->m_Flags.PositionChanged();
}

void Transform::Rotate(const quat& delta)
{
	m_pData->m_Rotation *= delta;
	m_pData->m_Flags.RotationChanged();
}

void Transform::RotateEuler(const vec3 & delta)
{
	m_pData->m_Rotation *= quat(glm::eulerAngleYXZ(delta.y, delta.x, delta.z));
	m_pData->m_Flags.RotationChanged();
}

void Transform::RotateEulerDegrees(const vec3 & delta)
{
	m_pData->m_Rotation *= quat(glm::eulerAngleYXZ((delta.y / 180.0f)*glm::pi<float>(), (delta.x / 180.0f)*glm::pi<float>(), (delta.z / 180.0f)*glm::pi<float>()));
	m_pData->m_Flags.RotationChanged();
}

void Transform::Scale(const vec3 & delta)
{
	m_pData->m_Scale *= delta;
	m_pData->m_Flags.ScaleChanged();
}

mat4 Transform::GetWorldMatrix()
{
	if (m_pData->m_Flags.HasTransformChanged())
	{
		CalculateWorldMatrix();
	}
	return m_pData->m_WorldMatrix;
}

void Transform::CalculateWorldMatrix()
{
	//make the ParentTranslateRotateScale (PTRS) matrix => this scales, rotates, translates and then applies parent transform (order: SRTP)
	mat4& m_WorldMatrix = m_pData->m_WorldMatrix;
	m_WorldMatrix = mat4(1.0f);
	m_WorldMatrix *= GetParent().GetWorldMatrix();
	m_WorldMatrix = glm::translate(m_WorldMatrix, GetPosition());
	m_WorldMatrix *= glm::toMat4(GetRotation());
	m_WorldMatrix = glm::scale(m_WorldMatrix, GetScale());
}
