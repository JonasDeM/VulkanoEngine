#pragma once
#include "LowMemoryContentLoader.h"
#include <memory>

class PxConvexMeshLoader final: public LowMemoryContentLoader<PxConvexMesh>
{
public:
	PxConvexMeshLoader() = default;

protected:
	virtual std::shared_ptr<PxConvexMesh> LoadContent(const wstring& assetFile) const override;
};

