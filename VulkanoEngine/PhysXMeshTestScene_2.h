#pragma once
#include "GameScene.h"

class PhysXMeshTestScene_2: public GameScene
{
public:
	PhysXMeshTestScene_2(GameSettings* pGameSettings);
	~PhysXMeshTestScene_2(void);

protected:

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override {};
	virtual void SceneActivated() override;
	virtual void SceneDeactivated() override;

private:

	GameObject* m_pConvexChair;
	PxRigidDynamic* m_pKinematicActor;
	PxVec3 m_KinematicPosition;

	std::shared_ptr<PxTriangleMesh> m_pTriangleMesh;
	std::shared_ptr<PxConvexMesh> m_pConvexMesh;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	PhysXMeshTestScene_2(const PhysXMeshTestScene_2& t);
	PhysXMeshTestScene_2& operator=(const PhysXMeshTestScene_2& t);
};

