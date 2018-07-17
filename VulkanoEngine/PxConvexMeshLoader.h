#pragma once
#include "ContentLoader.h"
#include <memory>

class PxConvexMeshLoader : public ContentLoader<PxConvexMesh>
{
public:
	PxConvexMeshLoader() {};

protected:

	virtual std::shared_ptr<PxConvexMesh> LoadContent(const wstring& assetFile) const override;

private:
// -------------------------
// Disabling default copy constructor and default 
// assignment operator.
// -------------------------
PxConvexMeshLoader(const PxConvexMeshLoader& yRef);									
PxConvexMeshLoader& operator=(const PxConvexMeshLoader& yRef);
};

