#pragma once
#include "stdafx.h"
#include "ContentManager.h"
#include "MeshDataLoader.h"
#include "PxConvexMeshLoader.h"
#include "PxTriangleMeshLoader.h"
#include "VkTextureLoader.h"
#include "BaseLoader.h"

std::vector<std::unique_ptr<BaseLoader>> ContentManager::m_Loaders{};

//std::unordered_map<std::type_index, std::unique_ptr<BaseLoader>> ContentManager::m_Loaders;

void ContentManager::Initialize(VulkanContext * pVkContext)
{
	//AddLoader(new MeshDataLoader());
	//AddLoader(new VkTextureLoader(pVkContext));
	//AddLoader(new PxConvexMeshLoader());
	//AddLoader(new PxTriangleMeshLoader());

	AddLoaderNew<MeshDataLoader>();
	AddLoaderNew<VkTextureLoader>(pVkContext);
	AddLoaderNew<PxConvexMeshLoader>();
	AddLoaderNew<PxTriangleMeshLoader>();
}

void ContentManager::CleanUp()
{
	m_Loaders.clear();
	//m_Loaders.clear();
}

//void ContentManager::AddLoader(BaseLoader * loader)
//{
//	//m_Loaders.insert_or_assign(loader->GetType(), std::unique_ptr<BaseLoader>(loader));
//}
