#pragma once
#include "LowMemoryContentLoader.h"
#include <memory>

class PxTriangleMeshLoader final: public LowMemoryContentLoader<PxTriangleMesh>
{
public:
	PxTriangleMeshLoader() = default;

protected:
	virtual std::shared_ptr<PxTriangleMesh> LoadContent(const wstring& assetFile) const override;
};

