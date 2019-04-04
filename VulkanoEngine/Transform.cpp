#include "stdafx.h"
#include "Transform.h"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

Transform Transform::GetParent()
{
	return GetData().m_Parent;
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

void Transform::AddChild(Transform child)
{

}

void Transform::RemoveChild(Transform child)
{

}

void Transform::SetPosition(const glm::vec3 & pos)
{
	GetData().m_Position = pos;
	GetData().m_Flags.PositionChanged();
}

vec3 Transform::GetPosition()
{
	return GetData().m_Position;
}

void Transform::SetRotation(const quat & rot)
{
	GetData().m_Rotation = rot;
	GetData().m_Flags.RotationChanged();
}

quat Transform::GetRotation()
{
	return GetData().m_Rotation;
}

void Transform::SetEulerRotation(const vec3 & rot)
{
	GetData().m_Rotation = quat(glm::eulerAngleYXZ(rot.y, rot.x, rot.z));
	GetData().m_Flags.RotationChanged();
}

vec3 Transform::GetEulerRotation()
{
	return eulerAngles(GetData().m_Rotation);
}

void Transform::SetEulerRotationInDegrees(const vec3 & rot)
{
	GetData().m_Rotation = quat(glm::eulerAngleYXZ((rot.y / 180.0f)*glm::pi<float>(), (rot.x / 180.0f)*glm::pi<float>(), (rot.z / 180.0f)*glm::pi<float>()));
	GetData().m_Flags.RotationChanged();
}

vec3 Transform::GetEulerRotationInDegrees()
{
	return (eulerAngles(GetData().m_Rotation) * 180.0f) / glm::pi<float>();
}

void Transform::SetScale(const vec3 & scale)
{
	GetData().m_Scale = scale;
	GetData().m_Flags.ScaleChanged();
}

vec3 Transform::GetScale()
{
	return GetData().m_Scale;
}

void Transform::Translate(const vec3 & delta)
{
	GetData().m_Position += delta;
	GetData().m_Flags.PositionChanged();
}

void Transform::Rotate(const quat& delta)
{
	GetData().m_Rotation *= delta;
	GetData().m_Flags.RotationChanged();
}

void Transform::RotateEuler(const vec3 & delta)
{
	GetData().m_Rotation *= quat(glm::eulerAngleYXZ(delta.y, delta.x, delta.z));
	GetData().m_Flags.RotationChanged();
}

void Transform::RotateEulerDegrees(const vec3 & delta)
{
	GetData().m_Rotation *= quat(glm::eulerAngleYXZ((delta.y / 180.0f)*glm::pi<float>(), (delta.x / 180.0f)*glm::pi<float>(), (delta.z / 180.0f)*glm::pi<float>()));
	GetData().m_Flags.RotationChanged();
}

void Transform::Scale(const vec3 & delta)
{
	GetData().m_Scale *= delta;
	GetData().m_Flags.ScaleChanged();
}

mat4 Transform::GetWorldMatrix()
{
	if (GetData().m_Flags.HasTransformChanged())
	{
		CalculateWorldMatrix();
	}
	return GetData().m_WorldMatrix;
}

void Transform::CalculateWorldMatrix()
{
	//make the ParentTranslateRotateScale (PTRS) matrix => this scales, rotates, translates and then applies parent transform (order: SRTP)
	TransformData& data = GetData();
	mat4& m_WorldMatrix = data.m_WorldMatrix;
	m_WorldMatrix = mat4(1.0f);
	m_WorldMatrix *= data.m_Parent.GetWorldMatrix();
	m_WorldMatrix = glm::translate(m_WorldMatrix, GetPosition());
	m_WorldMatrix *= glm::toMat4(GetRotation());
	m_WorldMatrix = glm::scale(m_WorldMatrix, GetScale());
}
