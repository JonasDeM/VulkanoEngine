#include "stdafx.h"
#include "PhysxManager.h"
#include "PhysxAllocator.h"
#include "PhysxErrorCallback.h"
#include "GameScene.h"
#include "Debug.h"
#include "CustomSimulationFilterShader.h"

static const char* DEFAULT_PVD_HOST = "127.0.0.1";
static const PxU32 DEFAULT_PVD_PORT = 5425;

PhysxManager::PhysxManager(void):
	m_pDefaultAllocator(nullptr),
	m_pFoundation(nullptr),
	m_pDefaultErrorCallback(nullptr),
	m_pProfileZoneManager(nullptr),
	m_pPhysics(nullptr),
	m_pDefaultCpuDispatcher(nullptr)
{
	Init();
}

PhysxManager::~PhysxManager(void)
{
}

void PhysxManager::Init()
{
	//Recording memory allocations is necessary if you want to 
	//use the memory facilities in PVD effectively.  Since PVD isn't necessarily connected
	//right away, we add a mechanism that records all outstanding memory allocations and
	//forwards them to PVD when it does connect.

	//This certainly has a performance and memory profile effect and thus should be used
	//only in non-production builds.
	bool recordMemoryAllocations = false;
#if defined(DEBUG) || defined(_DEBUG)
	recordMemoryAllocations = true;
#endif

	//WIN32 Allocator Implementation
	m_pDefaultAllocator = std::make_unique<PhysxAllocator>();
	m_pDefaultErrorCallback = std::make_unique<PhysxErrorCallback>();
	//Create Foundation
	m_pFoundation = unique_ptr_del<PxFoundation>(PxCreateFoundation(PX_PHYSICS_VERSION, *m_pDefaultAllocator, *m_pDefaultErrorCallback), &PxFoundation::release);
	if(!m_pFoundation)
	{
		Debug::LogError(L"Physx: Foundation creation failed!");
	}

	//Create ProfileZoneManager
	m_pProfileZoneManager = unique_ptr_del<PxProfileZoneManager>(&PxProfileZoneManager::createProfileZoneManager(m_pFoundation.get()), &PxProfileZoneManager::release);
	if(!m_pProfileZoneManager)
	{
		Debug::LogError(L"Physx: Profile Zone Manager creation failed!");
	}

	//Cude context manager?

	//Create Main Physics Object
	m_pPhysics = unique_ptr_del<PxPhysics>(PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), recordMemoryAllocations, m_pProfileZoneManager.get()), &PxPhysics::release);
	if(!m_pPhysics)
	{
		Debug::LogError(L"Physx: Physics Object creation failed!");
	}

	//TODO: ADD ProfileZone

	//Create Default CpuDispatcher (only 1 thread)
	m_pDefaultCpuDispatcher = unique_ptr_del<PxDefaultCpuDispatcher>(PxDefaultCpuDispatcherCreate(1), &PxDefaultCpuDispatcher::release);

	//Try to connect with the PVD (Todo: link profileZone)
	ToggleVisualDebuggerConnection();
	if(m_pPhysics->getPvdConnectionManager())
		m_pPhysics->getPvdConnectionManager()->addHandler(*this);
	
}

PxScene* PhysxManager::CreateScene(GameScene* pScene)
{
	auto sceneDesc = PxSceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.setToDefault(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = m_pDefaultCpuDispatcher.get();
	sceneDesc.filterShader = GP1SimulationFilterShader;
	sceneDesc.userData = pScene;
	//Link GpuDispatcher;

	auto physxScene = m_pPhysics->createScene(sceneDesc);
	if(!physxScene)
	{
		Debug::LogError(L"Physx: Scene creation failed!");
	}

	return physxScene;
}

bool PhysxManager::ToggleVisualDebuggerConnection()
{
	if(!m_pPhysics || !m_pPhysics->getPvdConnectionManager()) return false;

	auto pvdConnectionManager = m_pPhysics->getPvdConnectionManager();
	if(pvdConnectionManager->isConnected())
	{
		//DISCONNECT
		pvdConnectionManager->disconnect();
		return false;
	}
	else
	{
		//CONNECT

		//Use these flags for a clean profile trace with minimal overhead
		PxVisualDebuggerConnectionFlags connectionFlags( PxVisualDebuggerExt::getAllConnectionFlags() );
		//if (!mUseFullPvdConnection)
		//	theConnectionFlags = PxVisualDebuggerConnectionFlag::Profile; 
		//TODO: ADD PROFILE FIRST

		//The normal way to connect to pvd.  PVD needs to be running at the time this function is called.
		//We don't worry about the return value because we are already registered as a listener for connections
		//and thus our onPvdConnected call will take care of setting up our basic connection state.
		PxVisualDebuggerConnection* conn = PxVisualDebuggerExt::createConnection(pvdConnectionManager, DEFAULT_PVD_HOST, DEFAULT_PVD_PORT, 4000, connectionFlags);

		if (conn)
		{
			conn->release();
			return true;
		}
	}

	return false;
}

void PhysxManager::onPvdConnected(PxVisualDebuggerConnection& connection)
{
	m_pPhysics->getVisualDebugger()->setVisualizeConstraints(true);
	//m_pPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlags::eTRANSMIT_CONTACTS, true);
	//m_pPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlags::eTRANSMIT_SCENEQUERIES, true);
}