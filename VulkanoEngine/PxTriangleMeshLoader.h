#pragma once
#include "ContentLoader.h"
#include <memory>

class PxTriangleMeshLoader : public ContentLoader<PxTriangleMesh>
{
public:
	PxTriangleMeshLoader() {};

protected:

	virtual std::shared_ptr<PxTriangleMesh> LoadContent(const wstring& assetFile) const override;

private:
// -------------------------
// Disabling default copy constructor and default 
// assignment operator.
// -------------------------
PxTriangleMeshLoader(const PxTriangleMeshLoader& yRef);									
PxTriangleMeshLoader& operator=(const PxTriangleMeshLoader& yRef);
};

