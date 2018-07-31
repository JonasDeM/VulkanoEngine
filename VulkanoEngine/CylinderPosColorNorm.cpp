#pragma once
#include "stdafx.h"
#include "CylinderPosColorNorm.h"
#include "VertexStructs.h"
#include "ContentManager.h"
#include "Debug.h"
#include "VulkanUtils.h"
#include "HandleUtilities.h"
#include "VkBasicSampler_Ext.h"
#include "VkImageView_Ext.h"
#include "VkTextureImage_Ext.h"
#include "PipelineManager.h"
#include "VkBasicGeometryPipeline_Ext.h"
#include "VulkanContext.h"
#include "GameScene.h"


CylinderPosColorNorm::CylinderPosColorNorm(float radius, uint16_t radiusSteps, float height, vec4 color):
		m_Radius(radius),
		m_RadiusSteps(radiusSteps),
		m_Height(height),
		m_Color(color)
{
}


CylinderPosColorNorm::~CylinderPosColorNorm(void)
{
}

void CylinderPosColorNorm::Initialize(VulkanContext* pVkContext)
{
	auto pipeline = PipelineManager::GetPipeline<VkBasicGeometryPipeline_Ext>();
	CreateUniformBuffer(pVkContext);
	CreateVertexBuffer(pVkContext);
	CreateIndexBuffer(pVkContext);
	int amountFrameBuffers = pVkContext->GetVkSwapChain()->GetAmountImages();
	m_DescriptorPool = pipeline->CreateDescriptorPool(*pVkContext->GetVkDevice(), amountFrameBuffers);
	m_DescriptorSets = pipeline->CreateAndWriteDescriptorSets(*pVkContext->GetVkDevice(), *m_DescriptorPool, m_UniformBuffers);
}

void CylinderPosColorNorm::Update(VulkanContext* pVkContext)
{
	UpdateUniformVariables(pVkContext);
}

void CylinderPosColorNorm::UpdateUniformVariables(VulkanContext* pVkContext)
{
	GET_CLASS_FROM_PTR(PipelineManager::GetPipeline<VkBasicGeometryPipeline_Ext>())::UniformBufferObject ubo; // this way you can get the UniformBufferObject declared in that hraphics pipeline

	ubo.world = m_WorldMatrix;
	ubo.wvp = GetScene()->GetCamera()->GetViewProjection() * ubo.world;

	int i = pVkContext->GetCurrentFrameIndex();

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[i], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[i]);
}

void CylinderPosColorNorm::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, int frameBufferIndex)
{
	auto pipeline = PipelineManager::GetPipeline<VkBasicGeometryPipeline_Ext>();

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

	VkBuffer vertexBuffers[] = { *m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(cmdBuffer, *m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSets[frameBufferIndex], 0, nullptr);
	//draw
	vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(m_NumIndices), 1, 0, 0, 0);

}


void CylinderPosColorNorm::CreateIndexBuffer(VulkanContext* pVkContext)
{
	//Indexbuffer

	vector<uint16_t> indices;
	for (uint16_t i = 0; i < m_NumVerts - 2; i += 4)
	{
		indices.push_back(m_NumVerts - 2);
		indices.push_back(i);
		indices.push_back((i + 4) % (m_NumVerts - 2));

		indices.push_back(i + 1);
		indices.push_back(i + 2);
		indices.push_back((i + 6) % (m_NumVerts - 2));

		indices.push_back((i + 6) % (m_NumVerts - 2));
		indices.push_back((i + 5) % (m_NumVerts - 2));
		indices.push_back(i + 1);

		indices.push_back(i + 3);
		indices.push_back(m_NumVerts - 1);
		indices.push_back((i + 7) % (m_NumVerts - 2));
	}

	if ((size_t)std::numeric_limits<uint32_t>::max() < indices.size())
		m_NumIndices = std::numeric_limits<uint32_t>::max();
	else
		m_NumIndices = (uint32_t)indices.size();


	VkDeviceSize bufferSize = sizeof(indices[0]) * m_NumIndices;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, &stagingBuffer, &stagingBufferMemory);

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, &indices[0], (size_t)bufferSize);
	vkUnmapMemory(*pVkContext->GetVkDevice(), stagingBufferMemory);


	m_IndexBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
	m_IndexBufferMemory = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		, m_IndexBuffer.get(), m_IndexBufferMemory.get());

	VulkanUtils::CopyBuffer(pVkContext, stagingBuffer, *m_IndexBuffer, bufferSize);

	vkDestroyBuffer(*pVkContext->GetVkDevice(), stagingBuffer, nullptr);
	vkFreeMemory(*pVkContext->GetVkDevice(), stagingBufferMemory, nullptr);
}


void CylinderPosColorNorm::CreateVertexBuffer(VulkanContext* pVkContext)
{

	//Vertices
	vector<VertexPosColNorm> vertices;
	float interval = glm::two_pi<float>() / m_RadiusSteps;
	for (uint16_t step = 0; step < m_RadiusSteps; ++step)
	{
		float circleRadius = step * interval;
		vec3 pos = vec3(m_Radius * cos(circleRadius), m_Height / 2.0f, m_Radius * sin(circleRadius));
		vec3 norm(pos.x, 0.0f, pos.z);
		glm::normalize(norm);

		vertices.push_back(VertexPosColNorm(pos, m_Color, vec3(0, 1, 0)));

		vertices.push_back(VertexPosColNorm(pos, m_Color, norm));
		pos.y *= -1;
		vertices.push_back(VertexPosColNorm(pos, m_Color, norm));
		vertices.push_back(VertexPosColNorm(pos, m_Color, vec3(0, -1, 0)));
	}

	vertices.push_back(VertexPosColNorm(vec3(0, m_Height / 2.0f, 0), m_Color, vec3(0, 1, 0)));
	vertices.push_back(VertexPosColNorm(vec3(0, -m_Height / 2.0f, 0), m_Color, vec3(0, -1, 0)));

	if ((size_t)std::numeric_limits<uint16_t>::max() < vertices.size())
		m_NumVerts = std::numeric_limits<uint16_t>::max();
	else
		m_NumVerts = (uint16_t)vertices.size();


	//todo use my handles instead of manual cleanup
	VkDeviceSize bufferSize = sizeof(vertices[0]) * m_NumVerts;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, &stagingBuffer, &stagingBufferMemory);

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, &vertices[0], (size_t)bufferSize);
	vkUnmapMemory(*pVkContext->GetVkDevice(), stagingBufferMemory);


	m_VertexBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
	m_VertexBufferMemory = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		, m_VertexBuffer.get(), m_VertexBufferMemory.get());

	VulkanUtils::CopyBuffer(pVkContext, stagingBuffer, *m_VertexBuffer, bufferSize);

	vkDestroyBuffer(*pVkContext->GetVkDevice(), stagingBuffer, nullptr);
	vkFreeMemory(*pVkContext->GetVkDevice(), stagingBufferMemory, nullptr);
}


void CylinderPosColorNorm::CreateUniformBuffer(VulkanContext* pVkContext)
{
	VkDeviceSize bufferSize = sizeof(GET_CLASS_FROM_PTR(PipelineManager::GetPipeline<VkBasicGeometryPipeline_Ext>())::UniformBufferObject);
	m_UniformBuffers.resize(pVkContext->GetVkSwapChain()->GetAmountImages());
	m_UniformBuffersMemory.resize(m_UniformBuffers.size());
	for (size_t i = 0; i < m_UniformBuffers.size(); i++)
	{
		m_UniformBuffers[i] = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
		m_UniformBuffersMemory[i] = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
		VulkanUtils::CreateBuffer(pVkContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i].get(), m_UniformBuffersMemory[i].get());
	}
}