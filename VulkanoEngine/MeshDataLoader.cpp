#include "stdafx.h"

#include "MeshDataLoader.h"
#include "BinaryReader.h"
#include <algorithm>

std::shared_ptr<MeshData> MeshDataLoader::LoadContent(const wstring& assetFile) const
{
	auto binReader = std::make_unique<BinaryReader>();
	binReader->Open(assetFile);

	if (!binReader->Exists())
		return nullptr;

	vector<VertexBase> vertices;
	vector<uint32_t> indices;

	//READ OVM FILE
	int versionMajor = binReader->Read<char>();
	int versionMinor = binReader->Read<char>();

	if (versionMajor != 1 || versionMinor != 1)
	{
		wstringstream ss;
		ss << L"MeshDataLoader::Load() > Wrong OVM version\nFile: ";
		ss << assetFile;
		ss << L"\nExpected version 1.1, not " << versionMajor << L"." << versionMinor;
		Debug::LogWarning(ss.str());

		return nullptr;
	}

	unsigned int vertexCount = 0;
	unsigned int indexCount = 0;

	while (true)
	{
		auto meshDataType = (MeshDataType)binReader->Read<char>();
		if (meshDataType == MeshDataType::END)
			break;

		auto dataOffset = binReader->Read<unsigned int>();

		switch (meshDataType)
		{
			//HEADER
		case MeshDataType::HEADER:
		{
			auto name = binReader->ReadString();
			vertexCount = binReader->Read<unsigned int>();
			indexCount = binReader->Read<unsigned int>();
		}
		break;
		case MeshDataType::POSITIONS:
		{
			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				VertexBase vertex;
				vertex.Position.x = binReader->Read<float>();
				vertex.Position.y = binReader->Read<float>();
				vertex.Position.z = binReader->Read<float>();
				vertices.push_back(vertex);
			}
		}
		break;
		case MeshDataType::INDICES:
		{
			for (unsigned int i = 0; i < indexCount; ++i)
			{
				indices.push_back(binReader->Read<uint32_t>());
			}
		}
		break;
		case MeshDataType::NORMALS:
		{
			for_each(vertices.begin(), vertices.end(), [&binReader](VertexBase& vertex)
			{
				vertex.Normal.x = binReader->Read<float>();
				vertex.Normal.y = binReader->Read<float>();
				vertex.Normal.z = binReader->Read<float>();
			});
		}
		break;
		case MeshDataType::TEXCOORDS:
		{
			for_each(vertices.begin(), vertices.end(), [&binReader](VertexBase& vertex)
			{
				vertex.TexCoord.x = binReader->Read<float>();
				vertex.TexCoord.y = binReader->Read<float>();
			});
		}
		break;
		case MeshDataType::COLORS:
		{
			for_each(vertices.begin(), vertices.end(), [&binReader](VertexBase& vertex)
			{
				vertex.Color.x = binReader->Read<float>();
				vertex.Color.y = binReader->Read<float>();
				vertex.Color.z = binReader->Read<float>();
				vertex.Color.w = binReader->Read<float>();
			});
		}
		break;
		default:
			binReader->MoveBufferPosition(dataOffset);
			break;
		}
	}

	return std::make_shared<MeshData>(vertices, indices);
}