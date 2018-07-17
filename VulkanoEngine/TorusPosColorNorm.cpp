#pragma once
#include "stdafx.h"
#include "TorusPosColorNorm.h"
#include "VertexStructs.h"
#include "ContentManager.h"
#include "Debug.h"
#include <vector>
#include "VulkanUtils.h"
#include "HandleUtilities.h"
#include "VkPipelineManager.h"
#include "VkPosColNormPipeline_Ext.h"
#include "VulkanContext.h"
#include "GameScene.h"


TorusPosColorNorm::TorusPosColorNorm(float majorRadius, uint16_t majorRadiusSteps, float minorRadius, uint16_t minorRadiusSteps, vec4 color) :
	m_MajorRadius(majorRadius),
	m_MinorRadius(minorRadius),
	m_MajorRadiusSteps(majorRadiusSteps),
	m_MinorRadiusSteps(minorRadiusSteps),
	m_Color(color),
	m_NumVerts(0),
	m_NumIndices(0)
{
}


TorusPosColorNorm::~TorusPosColorNorm(void)
{
}

void TorusPosColorNorm::Initialize(VulkanContext* pVkContext)
{
	auto pipeline = VkPipelineManager::GetInstance()->GetPosColNormPipeline();
	CreateUniformBuffer(pVkContext);
	CreateVertexBuffer(pVkContext);
	CreateIndexBuffer(pVkContext);
	m_DescriptorPool = pipeline->CreateDescriptorPool(*pVkContext->GetVkDevice());
	m_DescriptorSet = pipeline->CreateAndWriteDescriptorSet(*pVkContext->GetVkDevice(), *m_DescriptorPool, *m_UniformBuffer);

}

void TorusPosColorNorm::Update(VulkanContext* pVkContext)
{
	UpdateUniformVariables(pVkContext);
}


void TorusPosColorNorm::UpdateUniformVariables(VulkanContext* pVkContext)
{
	GET_CLASS_FROM_PTR(VkPipelineManager::GetInstance()->GetPosColNormPipeline())::UniformBufferObject ubo; // this way you can get the UniformBufferObject declared in that hraphics pipeline

	ubo.world = m_WorldMatrix;
	ubo.wvp = GetScene()->GetCamera()->GetViewProjection() * ubo.world;

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_UniformBufferMemory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_UniformBufferMemory);
}

void TorusPosColorNorm::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer)
{
	auto pipeline = VkPipelineManager::GetInstance()->GetPosColNormPipeline();

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

	VkBuffer vertexBuffers[] = { *m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(cmdBuffer, *m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSet, 0, nullptr);
	//draw
	vkCmdDrawIndexed(cmdBuffer, m_NumIndices, 1, 0, 0, 0);

}


void TorusPosColorNorm::CreateIndexBuffer(VulkanContext* pVkContext)
{
	//Indices
	vector<uint16_t> indices;
	for (uint16_t i = 0; i < (uint16_t)m_NumVerts; ++i)
	{
		uint16_t v0, v1, v2, v3;
		v0 = i;
		v1 = (v0 + m_MinorRadiusSteps) % m_NumVerts;
		v2 = v1 + 1;
		v3 = i + 1;

		if ((i + 1) % m_MinorRadiusSteps == 0)
		{
			v2 -= m_MinorRadiusSteps;
			v3 -= m_MinorRadiusSteps;
		}

		indices.push_back(v2);
		indices.push_back(v1);
		indices.push_back(v0);

		indices.push_back(v0);
		indices.push_back(v3);
		indices.push_back(v2);
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


void TorusPosColorNorm::CreateVertexBuffer(VulkanContext* pVkContext)
{
	//Vertices
	vector<VertexPosColNorm> vertices;
	float majorInterval = glm::two_pi<float>() / m_MajorRadiusSteps;
	float minorInterval = glm::two_pi<float>() / m_MinorRadiusSteps;
	for (unsigned int majorStep = 0; majorStep < m_MajorRadiusSteps; ++majorStep)
	{
		float torusRadius = majorStep * majorInterval;

		for (unsigned int minorStep = 0; minorStep < m_MinorRadiusSteps; ++minorStep)
		{
			float circleRadius = minorStep * minorInterval;
			vec3 pos;
			pos.x = (m_MajorRadius + (m_MinorRadius*cos(circleRadius))) * cos(torusRadius);
			pos.y = (m_MajorRadius + (m_MinorRadius*cos(circleRadius))) * sin(torusRadius);
			pos.z = m_MinorRadius*sin(circleRadius);

			//Normal
			vec3 middle;
			middle.x = m_MajorRadius * cos(torusRadius);
			middle.y = m_MajorRadius * sin(torusRadius);
			middle.z = 0;

			vec3 normal = glm::normalize(pos - middle);

			vertices.push_back(VertexPosColNorm(pos, m_Color, normal));
		}
	}
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


void TorusPosColorNorm::CreateUniformBuffer(VulkanContext* pVkContext)
{
	VkDeviceSize bufferSize = sizeof(GET_CLASS_FROM_PTR(VkPipelineManager::GetInstance()->GetPosColNormPipeline())::UniformBufferObject);
	m_UniformBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
	m_UniformBufferMemory = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
	VulkanUtils::CreateBuffer(pVkContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffer.get(), m_UniformBufferMemory.get());
}
