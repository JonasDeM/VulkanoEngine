#pragma once
#include "Component.h"
#include <glm/gtc/quaternion.hpp>
using namespace glm;

class TransformComponent :public Component
{
public:
	enum Flags : char
	{
		PosChanged = 1<<0,
		RotChanged = 1<<1,
		ScaleChanged = 1<<2
	};

public:
	TransformComponent() = default;
	~TransformComponent() = default;
	TransformComponent(const TransformComponent& t) = delete;
	TransformComponent& operator=(const TransformComponent& t) = delete;

	vec3 GetPosition() const { return m_Position; }
	vec3 GetScale() const { return m_Scale; }
	quat GetRotation() const { return m_Rotation; }
	mat4 GetWorldMatrix() const { return m_WorldMatrix; }

	// worldmatrix only gets updated after everything else, so decomposing it is not a good idea
	// make this add all the pos or rot or scale from all the parents
	vec3 GetWorldPosition() const;
	quat GetWorldRotation() const;
	vec3 GetWorldScale() const;

	void Translate(float x, float y, float z);
	void Rotate(float x, float y, float z);
	void RotateQuat(float x, float y, float z, float w);
	void RotateEuler(float x, float y, float z);
	void Scale(float x, float y, float z);

	Flags GetFlags() { return m_Flags; }
private:
	friend class BaseGameObject; // because this is the standard component that every gameobject has.
	void Build(VulkanContext* vkContext) override;
	void Update(VulkanContext* vkContext) override;
	void CalculateWorldMatrix(const mat4 &parentWorld);

	// flags that show if the local position has changed since last update
	Flags m_Flags = (Flags)0;

	TransformComponent* m_pParentTransform = nullptr;

	mat4 m_WorldMatrix;
	vec3 m_Position;
	vec3 m_Scale = vec3(1, 1, 1);
	quat m_Rotation = vec4(0, 0, 0, 1);

	//vec3 m_WorldPosition;
	//vec3 m_WorldScale = vec3(1, 1, 1);
	//quat m_WorldRotation = vec4(0, 0, 0, 1);
};

