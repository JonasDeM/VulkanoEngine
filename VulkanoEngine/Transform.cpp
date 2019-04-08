#include "stdafx.h"
#include "Transform.h"
#include "TransformData.h"
#include "TransformFlags.h"
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

void Transform::SetParent(Transform newParent)
{
	Transform oldParent = GetParent();

	if (oldParent.IsValid())
	{
		auto& oldParentChildren = oldParent.m_pData->m_Children;
		oldParentChildren.erase(std::find(oldParentChildren.begin(), oldParentChildren.end(), *this));
	}
	if (newParent.IsValid())
	{	
		// updating local transform variables, no need to flag changes as nothing really has changed
		// there might be a more optimized way of doing this with matrices + SIMD
		m_pData->m_LocalPosition = GetWorldPosition() - newParent.GetWorldPosition();
		m_pData->m_LocalRotation = GetWorldRotation() * inverse(newParent.GetWorldRotation());
		m_pData->m_LocalScale = GetWorldScale() / newParent.GetWorldScale();

		auto& newParentChildren = newParent.m_pData->m_Children;
		newParentChildren.push_back(*this);
	}
	else
	{
		m_pData->m_LocalPosition = GetWorldPosition();
		m_pData->m_LocalRotation = GetWorldRotation();
		m_pData->m_LocalScale = GetWorldScale();
	}

	m_pData->m_Parent = newParent;
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
	child.SetParent(*this);
}

void Transform::RemoveChild(Transform child)
{
	child.SetParent(Transform());
}

void Transform::SetLocalPosition(const glm::vec3 & pos)
{
	m_pData->m_LocalPosition = pos;
	m_pData->m_Flags.SetFlags(TransformFlag::PHYSICS_POSITION_OUTOFDATE | TransformFlag::WORLD_POSITION_OUTOFDATE | TransformFlag::WORLD_MATRIX_OUTOFDATE);
	SetFlagsOnChildren(TransformFlag::PHYSICS_POSITION_OUTOFDATE | TransformFlag::WORLD_POSITION_OUTOFDATE | TransformFlag::WORLD_MATRIX_OUTOFDATE);
}

vec3 Transform::GetLocalPosition()
{
	return m_pData->m_LocalPosition;
}

void Transform::SetLocalRotation(const quat & rot)
{
	m_pData->m_LocalRotation = rot;
	m_pData->m_Flags.SetFlags(TransformFlag::PHYSICS_ROTATION_OUTOFDATE | TransformFlag::WORLD_ROTATION_OUTOFDATE | TransformFlag::WORLD_MATRIX_OUTOFDATE);
	SetFlagsOnChildren(TransformFlag::PHYSICS_ROTATION_OUTOFDATE | TransformFlag::WORLD_ROTATION_OUTOFDATE
		| TransformFlag::WORLD_MATRIX_OUTOFDATE | TransformFlag::PHYSICS_POSITION_OUTOFDATE | TransformFlag::WORLD_POSITION_OUTOFDATE );
}

quat Transform::GetLocalRotation()
{
	return m_pData->m_LocalRotation;
}

void Transform::SetLocalEulerRotation(const vec3 & rot)
{
	SetLocalRotation(quat(glm::eulerAngleYXZ(rot.y, rot.x, rot.z)));
}

vec3 Transform::GetLocalEulerRotation()
{
	return eulerAngles(GetLocalRotation());
}

void Transform::SetLocalEulerRotationInDegrees(const vec3 & rot)
{
	SetLocalEulerRotation(rot * (glm::pi<float>() / 180.0f));
}

vec3 Transform::GetLocalEulerRotationInDegrees()
{
	return GetLocalEulerRotation() * (180.0f / glm::pi<float>());
}

void Transform::SetLocalScale(const vec3 & scale)
{
	m_pData->m_LocalScale = scale;

	m_pData->m_Flags.SetFlags(TransformFlag::PHYSICS_ROTATION_OUTOFDATE | TransformFlag::WORLD_ROTATION_OUTOFDATE | TransformFlag::WORLD_MATRIX_OUTOFDATE);
	SetFlagsOnChildren(TransformFlag::PHYSICS_SCALE_OUTOFDATE | TransformFlag::WORLD_SCALE_OUTOFDATE 
		| TransformFlag::WORLD_MATRIX_OUTOFDATE | TransformFlag::PHYSICS_POSITION_OUTOFDATE | TransformFlag::WORLD_POSITION_OUTOFDATE);
}

vec3 Transform::GetLocalScale()
{
	return m_pData->m_LocalScale;
}

void Transform::SetWorldPosition(const vec3 & pos)
{
	assert(false); // NotImplemented
	//SetLocalPos(); set local pos so that the object ends up at the new worldposition
	m_pData->m_WorldPosition = pos;
	m_pData->m_Flags.ClearFlags(TransformFlag::WORLD_POSITION_OUTOFDATE);
}

vec3 Transform::GetWorldPosition()
{
	if (m_pData->m_Flags.HasFlags(TransformFlag::WORLD_POSITION_OUTOFDATE))
	{
		DecomposeWorldMatrix(); // maybe just multiply
	}
	return m_pData->m_WorldPosition;
}

void Transform::SetWorldRotation(const quat & rot)
{
	assert(false); // NotImplemented	
	//SetLocalRot(); set local rot so that the object ends up at the new worldrotation
	m_pData->m_WorldRotation = rot;
	m_pData->m_Flags.ClearFlags(TransformFlag::WORLD_POSITION_OUTOFDATE);
}

quat Transform::GetWorldRotation()
{
	if (m_pData->m_Flags.HasFlags(TransformFlag::WORLD_ROTATION_OUTOFDATE))
	{
		DecomposeWorldMatrix();
	}
	return m_pData->m_WorldRotation;
}

void Transform::SetWorldEulerRotation(const vec3 & rot)
{
	SetWorldRotation(quat(glm::eulerAngleYXZ(rot.y, rot.x, rot.z)));
}

vec3 Transform::GetWorldEulerRotation()
{
	return eulerAngles(GetWorldRotation());
}

void Transform::SetWorldEulerRotationInDegrees(const vec3 & rot)
{
	SetWorldEulerRotation(rot * (glm::pi<float>() / 180.0f));
}

vec3 Transform::GetWorldEulerRotationInDegrees()
{
	return GetWorldEulerRotation() * (180.0f / glm::pi<float>());
}

vec3 Transform::GetWorldScale()
{
	if (m_pData->m_Flags.HasFlags(TransformFlag::WORLD_SCALE_OUTOFDATE))
	{
		DecomposeWorldMatrix();
	}
	return m_pData->m_WorldScale;
}

void Transform::Translate(const vec3 & delta)
{
	SetLocalPosition(GetLocalPosition() + delta);
}

void Transform::Rotate(const quat& delta)
{
	SetLocalRotation(GetLocalRotation()*delta);
}

void Transform::RotateEuler(const vec3 & delta)
{
	Rotate(glm::eulerAngleYXZ(delta.y, delta.x, delta.z));
}

void Transform::RotateEulerDegrees(const vec3 & delta)
{
	RotateEuler(delta * (glm::pi<float>() / 180.0f));
}

void Transform::Scale(const vec3 & multiplier)
{
	SetLocalScale(GetLocalScale()*multiplier);
}

mat4 Transform::GetWorldMatrix()
{
	if (m_pData->m_Flags.HasFlags(TransformFlag::WORLD_MATRIX_OUTOFDATE))
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
	m_WorldMatrix = glm::translate(m_WorldMatrix, GetLocalPosition());
	m_WorldMatrix *= glm::toMat4(GetLocalRotation());
	m_WorldMatrix = glm::scale(m_WorldMatrix, GetLocalScale());

	m_pData->m_Flags.ClearFlags(TransformFlag::WORLD_MATRIX_OUTOFDATE);
}

void Transform::DecomposeWorldMatrix()
{
	// TODO optimize, we only need 3/5 variables calculated
	decompose(GetWorldMatrix()/*<- ensures we have the latest */, m_pData->m_WorldScale, m_pData->m_WorldRotation, m_pData->m_WorldPosition, vec3(), vec4());
	m_pData->m_Flags.ClearFlags(TransformFlag::WORLD_POSITION_OUTOFDATE | TransformFlag::WORLD_ROTATION_OUTOFDATE | TransformFlag::WORLD_SCALE_OUTOFDATE);
}

void Transform::SetFlagsOnChildren(TransformFlag flags)
{
	//for (Transform& t : m_pData->m_Children)
	//{
	//	t.SetFlagsOnSelfAndChildren(flags);
	//}
}

void Transform::SetFlagsOnSelfAndChildren(TransformFlag flags)
{
	//m_pData->m_Flags.SetFlags(flags);
	//for (Transform& t : m_pData->m_Children)
	//{
	//	t.SetFlagsOnSelfAndChildren(flags);
	//}
}
