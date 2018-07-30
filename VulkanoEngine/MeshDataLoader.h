#pragma once
#include "LowMemoryContentLoader.h"
#include "MeshData.h"

class MeshDataLoader final: public LowMemoryContentLoader<MeshData>
{
public:
	MeshDataLoader() = default;

protected:
	std::shared_ptr<MeshData> LoadContent(const wstring& fileName) const override;

private:
	enum MeshDataType :int
	{
		END = 0,
		HEADER = 1,
		POSITIONS = 2,
		INDICES = 3,
		NORMALS = 4,
		COLORS = 7,
		TEXCOORDS = 8
	};
};