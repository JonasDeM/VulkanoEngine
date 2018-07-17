#pragma once

#include "Singleton.h"
#include "HandleUtilities.h"

class PhysxAllocator;
class PhysxErrorCallback;
class GameScene;

class PhysxManager : public Singleton<PhysxManager>, public PxVisualDebuggerConnectionHandler
{
public:
	friend class Singleton<PhysxManager>;

	//GETTERS
	PxPhysics* GetPhysics() const { return m_pPhysics.get(); }
	
	//Methods
	PxScene* CreateScene(GameScene* pScene);

	//PVD
	virtual void onPvdSendClassDescriptions(PxVisualDebuggerConnection& connection) {};
	virtual void onPvdConnected(PxVisualDebuggerConnection& connection);
	virtual void onPvdDisconnected(PxVisualDebuggerConnection& connection) {};

	bool ToggleVisualDebuggerConnection();

private:
	PhysxManager(void);
	~PhysxManager(void);

	void Init();

	std::unique_ptr<PhysxAllocator> m_pDefaultAllocator;
	std::unique_ptr<PhysxErrorCallback> m_pDefaultErrorCallback;
	unique_ptr_del<PxFoundation> m_pFoundation;
	unique_ptr_del<PxProfileZoneManager> m_pProfileZoneManager;
	unique_ptr_del<PxPhysics> m_pPhysics;
	unique_ptr_del<PxDefaultCpuDispatcher> m_pDefaultCpuDispatcher;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	PhysxManager( const PhysxManager &obj) = delete;	
	PhysxManager& operator=( const PhysxManager& obj );
};

