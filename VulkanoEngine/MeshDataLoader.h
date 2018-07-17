#pragma once
#include "ContentLoader.h"
#include "MeshData.h"

class MeshDataLoader : public ContentLoader<MeshData>
{
public:
	MeshDataLoader() {};
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