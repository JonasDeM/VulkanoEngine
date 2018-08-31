#pragma once
#include "stdafx.h"
#include "TransformComponent.h"
#include "CompObj.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>


//vec3 TransformComponent::GetWorldPosition() const
//{
//	return vec3();
//}
//
//quat TransformComponent::GetWorldRotation() const
//{
//	return quat();
//}
//
//vec3 TransformComponent::GetWorldScale() const
//{
//	return vec3();
//}

void TransformComponent::Translate(float x, float y, float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;

	m_Flags = Flags(m_Flags | Flags::PosChanged);
}

void TransformComponent::Rotate(float x, float y, float z)
{
	m_Rotation = quat(glm::eulerAngleYXZ(y, x, z)); // i force radians with a GLM preprocessor directive
	m_Flags = Flags(m_Flags | Flags::RotChanged);
}

void TransformComponent::RotateQuat(float x, float y, float z, float w)
{
	m_Rotation.x = x;
	m_Rotation.y = y;
	m_Rotation.z = z;
	m_Rotation.w = w;
	m_Flags = Flags(m_Flags | Flags::RotChanged);
}

void TransformComponent::RotateEuler(float x, float y, float z)
{
	m_Rotation = quat(glm::eulerAngleYXZ(y / 180.0f*glm::pi<float>(), x / 180.0f*glm::pi<float>(), z / 180.0f*glm::pi<float>())); // i force radians with a GLM preprocessor directive
	m_Flags = Flags(m_Flags | Flags::RotChanged);
}

void TransformComponent::Scale(float x, float y, float z)
{
	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;
	m_Flags = Flags(m_Flags | Flags::ScaleChanged);
}

void TransformComponent::Build(VulkanContext * vkContext)
{
	if (m_pParentTransform == nullptr)
		CalculateWorldMatrix(mat4());
	else
		CalculateWorldMatrix(m_pParentTransform->GetWorldMatrix());
}

void TransformComponent::Update(VulkanContext * vkContext)
{
	if (m_pParentTransform == nullptr)
		CalculateWorldMatrix(mat4()); 
	else
		CalculateWorldMatrix(m_pParentTransform->GetWorldMatrix());
}

void TransformComponent::CalculateWorldMatrix(const mat4 & parentWorld)
{
	//make the ParentTranslateRotateScale (PTRS) matrix => this scales, rotates, translates and then applies parent transform (order: SRTP)
	m_WorldMatrix = mat4(1.0f);
	m_WorldMatrix *= parentWorld;
	m_WorldMatrix = glm::translate(m_WorldMatrix, m_Position);
	m_WorldMatrix *= glm::toMat4(m_Rotation);
	m_WorldMatrix = glm::scale(m_WorldMatrix, m_Scale);
}