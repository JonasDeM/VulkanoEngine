#pragma once
#include <vector>
#include <type_traits>
#include "VertexStructs.h"
#include "Debug.h"
#include "HandleUtilities.h"

class VulkanContext;
	
class MeshData
{
public:
	MeshData(const vector<VertexBase>& vertices, const vector<uint32_t>& indices);
	~MeshData(void);

	size_t GetVertexCount() const { return m_Vertices.size(); }
	size_t GetIndexCount() const { return m_Indices.size(); }

	VkBuffer* GetIndexBuffer(const VulkanContext* pVkContext);

	template<typename T>
	VkBuffer* GetVertexBuffer(const VulkanContext* pVkContext)
	{
		if (std::is_same_v<T, VertexPosColNorm>)
			return GetVertexBuffer_PosNormCol(pVkContext);

		if (std::is_same_v<T, VertexPosColNormTex>)
			return GetVertexBuffer_PosNormColTex(pVkContext);

		if (std::is_same_v<T, VertexPosNormTex>)
			return GetVertexBuffer_PosNormTex(pVkContext);

		constexpr bool valid = std::is_same_v<T, VertexPosColNorm> || std::is_same_v<T, VertexPosColNormTex> || std::is_same_v<T, VertexPosNormTex>;
		static_assert(valid, "No implementation found for this Vertex type.");
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
	vector<uint32_t> m_Indices;

private:
	MeshData(const MeshData& obj);
	MeshData& operator=(const MeshData& obj);
};
