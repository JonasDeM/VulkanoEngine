#pragma once
#include "stdafx.h"
#include "ContentManager.h"
#include "MeshDataLoader.h"
#include "PxConvexMeshLoader.h"
#include "PxTriangleMeshLoader.h"
#include "VkTextureLoader.h"
#include "BaseLoader.h"

std::vector<std::unique_ptr<BaseLoader>> ContentManager::m_Loaders{};

void ContentManager::Initialize(VulkanContext * pVkContext)
{
	AddLoaderNew<MeshDataLoader>();
	AddLoaderNew<VkTextureLoader>(pVkContext);
	AddLoaderNew<PxConvexMeshLoader>();
	AddLoaderNew<PxTriangleMeshLoader>();
}

void ContentManager::CleanUp()
{
	m_Loaders.clear();
}
