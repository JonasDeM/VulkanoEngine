#include "stdafx.h"
#include "ContentManager.h"
#include "MeshDataLoader.h"
#include "PxConvexMeshLoader.h"
#include "PxTriangleMeshLoader.h"
#include "VkTextureLoader.h"
#include "BaseLoader.h"
#include <memory>

ContentManager::ContentManager()
{
	AddLoader(new MeshDataLoader());
	AddVkLoader(new VkTextureLoader());
	AddLoader(new PxConvexMeshLoader());
	AddLoader(new PxTriangleMeshLoader());
}

void ContentManager::AddLoader(BaseLoader* loader)
{
	m_Loaders.insert_or_assign(loader->GetType().hash_code(), std::unique_ptr<BaseLoader>(loader));
}

void ContentManager::AddVkLoader(VkBaseLoader* loader)
{
	m_VkLoaders.insert_or_assign(loader->GetType().hash_code(), std::unique_ptr<VkBaseLoader>(loader));
}
