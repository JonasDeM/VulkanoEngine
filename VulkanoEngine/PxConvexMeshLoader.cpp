#pragma once
#include "stdafx.h"
#include "PxConvexMeshLoader.h"
#include "PhysxManager.h"

std::shared_ptr<PxConvexMesh> PxConvexMeshLoader::LoadContent(const wstring& assetFile) const
{
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	string buffer = string(assetFile.begin(), assetFile.end());
	auto inputStream  = PxDefaultFileInputData(buffer.c_str());
	PxConvexMesh* convexMesh = physX->createConvexMesh(inputStream);

	return std::shared_ptr<PxConvexMesh>(convexMesh, [](PxConvexMesh* obj)
	{
		obj->release();
	});
}
