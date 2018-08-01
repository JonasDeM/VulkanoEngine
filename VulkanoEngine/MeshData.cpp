#pragma once
#include "stdafx.h"
#include "MeshData.h"
#include <algorithm>
#include "VulkanUtils.h"
#include "VulkanContext.h"
#include "HandleUtilities.h"
#include "VkDevice_Ext.h"

MeshData::MeshData(const vector<VertexBase>& vertices, const vector<uint32_t>& indices) :
	m_Vertices(vertices),
	m_Indices(indices),
	m_pVertexBuffer_PosNormCol(nullptr),
	m_pVertexBuffer_PosNormColTex(nullptr),
	m_pVertexBuffer_PosNormTex(nullptr),
	m_pVertexBufferMemory_PosNormCol(nullptr),
	m_pVertexBufferMemory_PosNormColTex(nullptr),
	m_pVertexBufferMemory_PosNormTex(nullptr),
	m_pIndexBuffer(nullptr)
{
}


MeshData::~MeshData(void)
{
}


VkBuffer* MeshData::GetIndexBuffer(const VulkanContext* pVkContext)
{
	if (m_pIndexBuffer)
		return m_pIndexBuffer.get();

	VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();

	auto stagingBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
	auto stagingBufferMemory = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());

	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, stagingBuffer.get(), stagingBufferMemory.get());

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, &m_Indices[0], (size_t)bufferSize);
	vkUnmapMemory(*pVkContext->GetVkDevice(), *stagingBufferMemory);


	m_pIndexBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
	m_pIndexBufferMemory = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		, m_pIndexBuffer.get(), m_pIndexBufferMemory.get());

	VulkanUtils::CopyBuffer(pVkContext, *stagingBuffer, *m_pIndexBuffer, bufferSize);
		
	return m_pIndexBuffer.get();
}

VkBuffer* MeshData::GetVertexBuffer_PosNormCol(const VulkanContext* pVkContext) 
{
	if (m_pVertexBuffer_PosNormCol)
		return m_pVertexBuffer_PosNormCol.get();

	vector<VertexPosColNorm> verts;
	for_each(m_Vertices.begin(), m_Vertices.end(), [&verts](VertexBase& vertex)
	{
		verts.push_back(VertexPosColNorm(vertex));
	});

	//todo use my handles instead of manual cleanup
	VkDeviceSize bufferSize = sizeof(verts[0]) * verts.size();

	auto stagingBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
	auto stagingBufferMemory = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());

	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, stagingBuffer.get(), stagingBufferMemory.get());

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, &verts[0], (size_t)bufferSize);
	vkUnmapMemory(*pVkContext->GetVkDevice(), *stagingBufferMemory);


	m_pVertexBuffer_PosNormCol = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
	m_pVertexBufferMemory_PosNormCol = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		, m_pVertexBuffer_PosNormCol.get(), m_pVertexBufferMemory_PosNormCol.get());

	VulkanUtils::CopyBuffer(pVkContext, *stagingBuffer, *m_pVertexBuffer_PosNormCol, bufferSize);


	return m_pVertexBuffer_PosNormCol.get();
}

VkBuffer* MeshData::GetVertexBuffer_PosNormTex(const VulkanContext* pVkContext) 
{

	if (m_pVertexBuffer_PosNormTex)
		return m_pVertexBuffer_PosNormTex.get();

	vector<VertexPosNormTex> verts;
	for_each(m_Vertices.begin(), m_Vertices.end(), [&verts](VertexBase& vertex)
	{
		verts.push_back(VertexPosNormTex(vertex));
	});

	//todo use my handles instead of manual cleanup
	VkDeviceSize bufferSize = sizeof(verts[0]) * verts.size();

	auto stagingBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
	auto stagingBufferMemory = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());

	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, stagingBuffer.get(), stagingBufferMemory.get());

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, &verts[0], (size_t)bufferSize);
	vkUnmapMemory(*pVkContext->GetVkDevice(), *stagingBufferMemory);


	m_pVertexBuffer_PosNormTex = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
	m_pVertexBufferMemory_PosNormTex = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		, m_pVertexBuffer_PosNormTex.get(), m_pVertexBufferMemory_PosNormTex.get());

	VulkanUtils::CopyBuffer(pVkContext, *stagingBuffer, *m_pVertexBuffer_PosNormTex, bufferSize);


	return m_pVertexBuffer_PosNormTex.get();
}

VkBuffer* MeshData::GetVertexBuffer_PosNormColTex(const VulkanContext* pVkContext) 
{

	if (m_pVertexBuffer_PosNormColTex)
		return m_pVertexBuffer_PosNormColTex.get();

	vector<VertexPosColNorm> verts;
	for_each(m_Vertices.begin(), m_Vertices.end(), [&verts](VertexBase& vertex)
	{
		verts.push_back(VertexPosColNorm(vertex));
	});

	//todo use my handles instead of manual cleanup
	VkDeviceSize bufferSize = sizeof(verts[0]) * verts.size();

	auto stagingBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
	auto stagingBufferMemory = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());

	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, stagingBuffer.get(), stagingBufferMemory.get());

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, &verts[0], (size_t)bufferSize);
	vkUnmapMemory(*pVkContext->GetVkDevice(), *stagingBufferMemory);


	m_pVertexBuffer_PosNormColTex = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
	m_pVertexBufferMemory_PosNormColTex = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		, m_pVertexBuffer_PosNormColTex.get(), m_pVertexBufferMemory_PosNormColTex.get());

	VulkanUtils::CopyBuffer(pVkContext, *stagingBuffer, *m_pVertexBuffer_PosNormColTex, bufferSize);


	return m_pVertexBuffer_PosNormColTex.get();
}