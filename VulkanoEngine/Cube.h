#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include <glm\glm.hpp>
#include "VkPhysicalDevice_Ext.h"
#include "HandleUtilities.h"

#include "VertexStructs.h"
#include "ContentManager.h"
#include "VulkanUtils.h"
#include "HandleUtilities.h"
#include "PipelineManager.h"
#include "VulkanContext.h"
#include "GameScene.h"
#include "VkSwapchainKHR_Ext.h"
#include "VkDevice_Ext.h"


class VulkanContext;

// Not Performant practice gameobject
template<class Pipeline>
class Cube : public GameObject
{
	static_assert(std::is_base_of<VkPipeline_Ext, Pipeline>::value, "Template parameter Pipeline must derive from VkPipeline_Ext");
public:
	Cube(float width, float height, float depth) :
		m_Width(width),
		m_Height(height),
		m_Depth(depth)
	{
	}
	Cube(float width, float height, float depth, vec4 color) :
		Cube(width, height, depth)
	{
		m_UseCustomColor = true;
		m_CustomColor = color;
	}
	~Cube(void) = default;
	Cube(const Cube& t) = delete;
	Cube& operator=(const Cube& t) = delete;

	virtual void Initialize(VulkanContext* pVkContext) override;
	virtual void Update(VulkanContext* pVkContext) override
	{
		UpdateUniformVariables(pVkContext);
	};

private:
	void CreateVertexBuffer(VulkanContext* pVkContext);
	void CreateIndexBuffer(VulkanContext* pVkContext);
	void CreateUniformBuffer(VulkanContext* pVkContext);
	void UpdateUniformVariables(VulkanContext* pVkContext);
	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex) override;

	float m_Width, m_Height, m_Depth;
	static const int NUMVERTICES = 24;
	static const int NUMINDICES = 36;

	unique_ptr_del<VkBuffer> m_VertexBuffer; // unique ptr -> ownrship, which means every cube has it's own vertexbuffer (not optimal ofcourse)
	unique_ptr_del<VkDeviceMemory> m_VertexBufferMemory;
	unique_ptr_del<VkBuffer> m_IndexBuffer;
	unique_ptr_del<VkDeviceMemory> m_IndexBufferMemory;
	std::vector<unique_ptr_del<VkBuffer>> m_UniformBuffers;
	std::vector<unique_ptr_del<VkDeviceMemory>> m_UniformBuffersMemory;

	unique_ptr_del<VkDescriptorPool> m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets; //gets automatically cleaned up with pool

	bool m_UseCustomColor = false;
	vec4 m_CustomColor;
};

template<class Pipeline>
void Cube<Pipeline>::Initialize(VulkanContext* pVkContext)
{
	auto pipeline = PipelineManager::GetPipeline<Pipeline>();
	CreateUniformBuffer(pVkContext);
	CreateVertexBuffer(pVkContext);
	CreateIndexBuffer(pVkContext);
	int amountFrameBuffers = pVkContext->GetVkSwapChain()->GetAmountImages();
	m_DescriptorPool = pipeline->CreateDescriptorPool(*pVkContext->GetVkDevice(), amountFrameBuffers);
	m_DescriptorSets = pipeline->CreateAndWriteDescriptorSets(*pVkContext->GetVkDevice(), *m_DescriptorPool, m_UniformBuffers);
}

template<class Pipeline>
void Cube<Pipeline>::UpdateUniformVariables(VulkanContext* pVkContext)
{
	Pipeline::UniformBufferObject ubo; // this way you can get the UniformBufferObject declared in that graphics pipeline

	ubo.world = m_WorldMatrix;
	ubo.wvp = GetScene()->GetCamera()->GetViewProjection() * ubo.world;

	int i = pVkContext->GetCurrentFrameIndex();

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[i], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[i]);
}

template<class Pipeline>
void Cube<Pipeline>::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex)
{
	auto pipeline = PipelineManager::GetPipeline<Pipeline>();

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

	VkBuffer vertexBuffers[] = { *m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(cmdBuffer, *m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSets[frameBufferIndex], 0, nullptr);
	//draw
	vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(NUMINDICES), 1, 0, 0, 0);

}

//per scene object
template<class Pipeline>
void Cube<Pipeline>::CreateIndexBuffer(VulkanContext* pVkContext)
{
	//Indexbuffer
	uint16_t indices[NUMINDICES] = {
		0,1,2, 2,1,3,
		4,6,5, 5,6,7,
		8,10,9, 9,10,11,
		12,13,14, 14,13,15,
		16,18,17, 17,18,19,
		20,21,22, 22,21,23
	};
	VkDeviceSize bufferSize = sizeof(indices[0]) * NUMINDICES;

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

//per scene object
template<class Pipeline>
void Cube<Pipeline>::CreateVertexBuffer(VulkanContext* pVkContext)
{
	//Vertex Buffer
	VertexBase baseVertices[NUMVERTICES] = {};
	int index = 0;

	//front red
	vec4 color = m_UseCustomColor ? m_CustomColor : vec4(0.5f, 0.5f, 0.5f, 1);
	baseVertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, -m_Depth / 2);
	baseVertices[index].Normal = vec3(0, 0, -1);
	baseVertices[index].TexCoord = vec2(0, 0);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(-m_Width / 2, m_Height / 2, -m_Depth / 2);
	baseVertices[index].Normal = vec3(0, 0, -1);
	baseVertices[index].TexCoord = vec2(0, 1);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(m_Width / 2, -m_Height / 2, -m_Depth / 2);
	baseVertices[index].Normal = vec3(0, 0, -1);
	baseVertices[index].TexCoord = vec2(1, 0);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(m_Width / 2, m_Height / 2, -m_Depth / 2);
	baseVertices[index].Normal = vec3(0, 0, -1);
	baseVertices[index].TexCoord = vec2(1, 1);
	baseVertices[index++].Color = color;
	//back red
	baseVertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, m_Depth / 2);
	baseVertices[index].Normal = vec3(0, 0, 1);
	baseVertices[index].TexCoord = vec2(0, 0);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(-m_Width / 2, m_Height / 2, m_Depth / 2);
	baseVertices[index].Normal = vec3(0, 0, 1);
	baseVertices[index].TexCoord = vec2(0, 1);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(m_Width / 2, -m_Height / 2, m_Depth / 2);
	baseVertices[index].Normal = vec3(0, 0, 1);
	baseVertices[index].TexCoord = vec2(1, 0);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(m_Width / 2, m_Height / 2, m_Depth / 2);
	baseVertices[index].Normal = vec3(0, 0, 1);
	baseVertices[index].TexCoord = vec2(1, 1);
	baseVertices[index++].Color = color;

	color = m_UseCustomColor ? m_CustomColor : vec4(0, 1, 0, 1);
	//left green
	baseVertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, -m_Depth / 2);
	baseVertices[index].Normal = vec3(-1, 0, 0);
	baseVertices[index].TexCoord = vec2(0, 0);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(-m_Width / 2, m_Height / 2, -m_Depth / 2);
	baseVertices[index].Normal = vec3(-1, 0, 0);
	baseVertices[index].TexCoord = vec2(0, 1);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, m_Depth / 2);
	baseVertices[index].Normal = vec3(-1, 0, 0);
	baseVertices[index].TexCoord = vec2(1, 0);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(-m_Width / 2, m_Height / 2, m_Depth / 2);
	baseVertices[index].Normal = vec3(-1, 0, 0);
	baseVertices[index].TexCoord = vec2(1, 1);
	baseVertices[index++].Color = color;

	//right green
	baseVertices[index].Position = vec3(m_Width / 2, -m_Height / 2, -m_Depth / 2);
	baseVertices[index].Normal = vec3(1, 0, 0);
	baseVertices[index].TexCoord = vec2(0, 0);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(m_Width / 2, m_Height / 2, -m_Depth / 2);
	baseVertices[index].Normal = vec3(1, 0, 0);
	baseVertices[index].TexCoord = vec2(0, 1);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(m_Width / 2, -m_Height / 2, m_Depth / 2);
	baseVertices[index].Normal = vec3(1, 0, 0);
	baseVertices[index].TexCoord = vec2(1, 0);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(m_Width / 2, m_Height / 2, m_Depth / 2);
	baseVertices[index].Normal = vec3(1, 0, 0);
	baseVertices[index].TexCoord = vec2(1, 1);
	baseVertices[index++].Color = color;

	color = m_UseCustomColor ? m_CustomColor : vec4(0, 0, 1, 1);
	//top blue
	baseVertices[index].Position = vec3(-m_Width / 2, m_Height / 2, m_Depth / 2);
	baseVertices[index].Normal = vec3(0, 1, 0);
	baseVertices[index].TexCoord = vec2(0, 0);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(-m_Width / 2, m_Height / 2, -m_Depth / 2);
	baseVertices[index].Normal = vec3(0, 1, 0);
	baseVertices[index].TexCoord = vec2(0, 1);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(m_Width / 2, m_Height / 2, m_Depth / 2);
	baseVertices[index].Normal = vec3(0, 1, 0);
	baseVertices[index].TexCoord = vec2(1, 0);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(m_Width / 2, m_Height / 2, -m_Depth / 2);
	baseVertices[index].Normal = vec3(0, 1, 0);
	baseVertices[index].TexCoord = vec2(1, 1);
	baseVertices[index++].Color = color;

	//bottom blue
	baseVertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, m_Depth / 2);
	baseVertices[index].Normal = vec3(0, -1, 0);
	baseVertices[index].TexCoord = vec2(0, 0);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, -m_Depth / 2);
	baseVertices[index].Normal = vec3(0, -1, 0);
	baseVertices[index].TexCoord = vec2(0, 1);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(m_Width / 2, -m_Height / 2, m_Depth / 2);
	baseVertices[index].Normal = vec3(0, -1, 0);
	baseVertices[index].TexCoord = vec2(1, 0);
	baseVertices[index++].Color = color;
	baseVertices[index].Position = vec3(m_Width / 2, -m_Height / 2, -m_Depth / 2);
	baseVertices[index].Normal = vec3(0, -1, 0);
	baseVertices[index].TexCoord = vec2(1, 1);
	baseVertices[index++].Color = color;


	typename Pipeline::VertexType vertices[NUMVERTICES] = {};

	for (int i = 0; i < NUMVERTICES; i++)
	{
		vertices[i] = Pipeline::VertexType(baseVertices[i]);
	}

	//todo use my handles instead of manual cleanup
	VkDeviceSize bufferSize = sizeof(vertices[0]) * NUMVERTICES;

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

//per scene object
template<class Pipeline>
void Cube<Pipeline>::CreateUniformBuffer(VulkanContext* pVkContext)
{
	VkDeviceSize bufferSize = sizeof(Pipeline::UniformBufferObject);

	m_UniformBuffers.resize(pVkContext->GetVkSwapChain()->GetAmountImages());
	m_UniformBuffersMemory.resize(m_UniformBuffers.size());
	for (size_t i = 0; i < m_UniformBuffers.size(); i++)
	{
		m_UniformBuffers[i] = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
		m_UniformBuffersMemory[i] = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
		VulkanUtils::CreateBuffer(pVkContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i].get(), m_UniformBuffersMemory[i].get());
	}
}