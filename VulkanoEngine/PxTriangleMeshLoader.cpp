#include "stdafx.h"
#include "PxTriangleMeshLoader.h"
#include "PhysxManager.h"

std::shared_ptr<PxTriangleMesh> PxTriangleMeshLoader::LoadContent(const wstring& assetFile) const
{
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	string buffer = string(assetFile.begin(), assetFile.end());
	auto inputStream  = PxDefaultFileInputData(buffer.c_str());
	PxTriangleMesh* triangleMesh = physX->createTriangleMesh(inputStream);

	return std::shared_ptr<PxTriangleMesh>(triangleMesh, [](PxTriangleMesh* obj)
	{
		obj->release();
	});
}
