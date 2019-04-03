#pragma once
#include "Component.h"
#include <glm\glm.hpp>
using namespace glm;

struct TransformData;

// Component that manages the position rotation and scale of objects
// Every Gameobject has a Transform
class Transform : public Component<TransformData>
{
public:
	// Hierarchy
	Transform GetParent();
	Transform GetScene(); // Maybe keep scene handle in TransformData?
	void AddChild(Transform child);
	void RemoveChild(Transform child);

	void SetPosition(const vec3& pos);
	void GetPosition(vec3& pos);
	void SetRotation(const quat& rot);
	void GetRotation(quat& rot);
	void SetEulerRotation(const vec3& rot);
	void GetEulerRotation(vec3& rot);
	void SetEulerRotationInDegrees(const vec3& rot);
	void GetEulerRotationInDegrees(vec3& rot);
	void SetScale(const vec3& scale);
	void GetScale(vec3& scale);

	void Translate(const vec3& delta);
	void Rotate(const quat& delta);
	void RotateEuler(const vec3& delta);
	void RotateEulerDegrees(const vec3& delta);
	void Scale(const vec3& delta);

private:
	void Build() {};
	void Start() {};
	void Update() {};
	void RecordVulkanDrawCommands() {};
	void Stop() {};
	void Destroy() {};
};

struct TransformData {
	Transform m_Parent;
	std::vector<Transform> m_Children;

	mat4 m_WorldMatrix;
	vec3 m_Position, m_Scale;
	quat m_Rotation;

	vec3 m_WorldPosition, m_WorldScale;
	quat m_WorldRotation;
};
