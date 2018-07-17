#pragma once

#include <vector>
#include "VertexStructs.h"
#include "Debug.h"
#include <unordered_map>
#include "HandleUtilities.h"

class VulkanContext;
	
class MeshData
{
public:
	MeshData(const vector<VertexBase>& vertices, const vector<unsigned int>& indices);
	~MeshData(void);

	size_t GetVertexCount() const { return m_Vertices.size(); }
	size_t GetIndexCount() const { return m_Indices.size(); }

	VkBuffer* GetIndexBuffer(const VulkanContext* pVkContext);

	template<typename T>
	VkBuffer* GetVertexBuffer(const VulkanContext* pVkContext)
	{
		const type_info &type = typeid(T);
		if (type == typeid(VertexPosColNorm))
			return GetVertexBuffer_PosNormCol(pVkContext);

		if (type == typeid(VertexPosColNormTex))
			return GetVertexBuffer_PosNormColTex(pVkContext);

		if (type == typeid(VertexPosNormTex))
			return GetVertexBuffer_PosNormTex(pVkContext);

		wstringstream ss;
		ss << L"MeshData::GetVertexBuffer<" << type.name() << L">(): No implementation found for \'" << type.name() << L"\'!";
		Debug::LogWarning(ss.str());
		return nullptr;
	}


private:
	unique_ptr_del<VkBuffer> m_pIndexBuffer;
	unique_ptr_del<VkDeviceMemory> m_pIndexBufferMemory;
		
	unique_ptr_del<VkBuffer> m_pVertexBuffer_PosNormCol;
	unique_ptr_del<VkDeviceMemory> m_pVertexBufferMemory_PosNormCol;
	VkBuffer* GetVertexBuffer_PosNormCol(const VulkanContext* pVkContext);

	unique_ptr_del<VkBuffer> m_pVertexBuffer_PosNormColTex;
	unique_ptr_del<VkDeviceMemory> m_pVertexBufferMemory_PosNormColTex;
	VkBuffer* GetVertexBuffer_PosNormColTex(const VulkanContext* pVkContext);

	unique_ptr_del<VkBuffer> m_pVertexBuffer_PosNormTex;
	unique_ptr_del<VkDeviceMemory> m_pVertexBufferMemory_PosNormTex;
	VkBuffer* GetVertexBuffer_PosNormTex(const VulkanContext* pVkContext);

	vector<VertexBase> m_Vertices;
	vector<unsigned int> m_Indices;

private:
	MeshData(const MeshData& obj);
	MeshData& operator=(const MeshData& obj);
};
