#pragma once
#include "Component.h"
#include <glm\glm.hpp>
using namespace glm;

class TransformSystem;
struct TransformData;
enum TransformFlag : int16_t;

// Component that manages the hierarchy, position, rotation and scale of objects
// Every Gameobject has a Transform
// TODO: there's definetely room for optimizations (utilizing SIMD, more usage of matrix math, ...)
class Transform : public Component<TransformData, TransformSystem>
{
public:
	// Hierarchy
	Transform GetParent();
	void SetParent(Transform parent);
	Transform GetRootParent(); // Maybe keep rootparent handle in TransformData?
	Transform GetChild(size_t index);
	size_t GetChildCount();
	void AddChild(Transform child);
	void RemoveChild(Transform child);

	// Local 3D Position - Rotation - Scale
	// Setters: instant
	// Getters: instant
	void SetLocalPosition(const vec3& pos);
	vec3 GetLocalPosition();
	void SetLocalRotation(const quat& rot);
	quat GetLocalRotation();
	void SetLocalEulerRotation(const vec3& rot);
	vec3 GetLocalEulerRotation();
	void SetLocalEulerRotationInDegrees(const vec3& rot);
	vec3 GetLocalEulerRotationInDegrees();
	void SetLocalScale(const vec3& scale);
	vec3 GetLocalScale();

	// World 3D Position - Rotation - Scale 
	// More expensive than local equivalents 
	// Setters: Calculations need to be done to figure out new local equivalents
	// Getters: WorldVariables are lazily calculated, so they might need to get calculated when you Get them
	void SetWorldPosition(const vec3& pos);
	vec3 GetWorldPosition();
	void SetWorldRotation(const quat& rot);
	quat GetWorldRotation();
	void SetWorldEulerRotation(const vec3& rot);
	vec3 GetWorldEulerRotation();
	void SetWorldEulerRotationInDegrees(const vec3& rot);
	vec3 GetWorldEulerRotationInDegrees();
	//void SetWorldScale(const vec3& scale);
	vec3 GetWorldScale();

	void Translate(const vec3& delta);
	void Rotate(const quat& delta);
	void RotateEuler(const vec3& delta);
	void RotateEulerDegrees(const vec3& delta);
	void Scale(const vec3& multiplier);

	mat4 GetWorldMatrix();
private:
	void CalculateWorldMatrix();
	void DecomposeWorldMatrix();
	void SetFlagsOnChildren(TransformFlag flags);
	void SetFlagsOnSelfAndChildren(TransformFlag flags);
};