#include "stdafx.h"
#include "VoxelTerrain.h"
#include "VulkanContext.h"
#include "VkDevice_Ext.h"
#include "VulkanUtils.h"
#include "VertexStructs.h"
#include "PipelineManager.h"
#include "VkBasicGeometryPipeline_Ext.h"
#include "VkSwapchainKHR_Ext.h"
#include "GameScene.h"

VoxelTerrain::VoxelTerrain(float sizeVoxel) :
	m_SizeVoxel(sizeVoxel)
{
}

void VoxelTerrain::Initialize(VulkanContext * pVkContext)
{
	CreateUniformBuffer(pVkContext);

	for (size_t i = 0; i < AMOUNT_1_AXIS*AMOUNT_1_AXIS*AMOUNT_1_AXIS; i++)
	{
		float y = IndexToXYZ(i).y;
		if (y != 0)
		{
			m_Normals[i] = vec3();
		}
		else
		{
			m_Normals[i] = vec3(0, (float)i/(float)(AMOUNT_1_AXIS*AMOUNT_1_AXIS*AMOUNT_1_AXIS)*0.9f + 0.001f,0);
		}
	}

	NormalsToVertices();
	UpdateVertexBufferSize(pVkContext);
}

void VoxelTerrain::Update(VulkanContext * pVkContext)
{
	UpdateUniformVariables(pVkContext);
	NormalsToVertices();
	UpdateVertexBufferSize(pVkContext);
	//update vertexdata
	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_pVertexBufferMem, 0, (VkDeviceSize)(sizeof(VertexPosColNorm)*m_Vertices.size()), 0, &data);
	memcpy(data, m_Vertices.data(), sizeof(VertexPosColNorm)*m_Vertices.size());
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_pVertexBufferMem);
}

void VoxelTerrain::UpdateVertexBufferSize(VulkanContext * pVkContext)
{	
	if (m_Vertices.size() > m_BufferSize)
	{
		VkDevice device = *pVkContext->GetVkDevice();
		vkDeviceWaitIdle(device); // TODO CHANGE
		m_pVertexBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, device);
		m_pVertexBufferMem = CreateHandle<VkDeviceMemory>(vkFreeMemory, device);
		VulkanUtils::CreateBuffer(pVkContext, m_Vertices.size()
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, m_pVertexBuffer.get(), m_pVertexBufferMem.get());

		m_BufferSize = m_Vertices.size();
		pVkContext->SetFlags(VkContextFlags::InvalidDrawCommandBuffers);
	}
}

void VoxelTerrain::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, int frameBufferIndex)
{
	auto pipeline = PipelineManager::GetPipeline<VkBasicGeometryPipeline_Ext>();
	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

	VkBuffer vertexBuffers[] = { *m_pVertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSets[frameBufferIndex], 0, nullptr);
	//draw
	vkCmdDraw(cmdBuffer, m_Vertices.size(), 1, 0, 0);

}

void VoxelTerrain::CreateUniformBuffer(VulkanContext * pVkContext)
{
	auto device = *pVkContext->GetVkDevice();
	VkDeviceSize bufferSize = sizeof(GET_CLASS_FROM_PTR(PipelineManager::GetPipeline<VkBasicGeometryPipeline_Ext>())::UniformBufferObject);

	m_UniformBuffers.resize(pVkContext->GetVkSwapChain()->GetAmountImages());
	m_UniformBuffersMemory.resize(m_UniformBuffers.size());
	for (size_t i = 0; i < m_UniformBuffers.size(); i++)
	{
		m_UniformBuffers[i] = CreateHandle<VkBuffer>(vkDestroyBuffer, device);
		m_UniformBuffersMemory[i] = CreateHandle<VkDeviceMemory>(vkFreeMemory, device);
		VulkanUtils::CreateBuffer(pVkContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i].get(), m_UniformBuffersMemory[i].get());
	}
}

void VoxelTerrain::UpdateUniformVariables(VulkanContext * pVkContext)
{
	//with camera from context
	GET_CLASS_FROM_PTR(PipelineManager::GetPipeline<VkBasicGeometryPipeline_Ext>())::UniformBufferObject ubo; // this way you can get the UniformBufferObject declared in that graphics pipeline

	ubo.wvp = mat4() * GetScene()->GetCamera()->GetViewProjection();

	int index = pVkContext->GetCurrentFrameIndex();
	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[index], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[index]);
}

void VoxelTerrain::NormalsToVertices()
{
	m_Vertices.clear();
	for (size_t z = 1; z < AMOUNT_1_AXIS-1; z++)
	{
		for (size_t y = 1; y < AMOUNT_1_AXIS - 1; y++)
		{
			for (size_t x = 1; x < AMOUNT_1_AXIS - 1; x++)
			{
				int i = x + y * AMOUNT_1_AXIS + z * AMOUNT_1_AXIS*AMOUNT_1_AXIS;
				vec3 v = m_Normals[i];

				if (v != vec3())
				{
					std::array<int, 6> neighbours{ 
						i + 1 
						,i - 1
						,i + AMOUNT_1_AXIS
						,i - AMOUNT_1_AXIS
						,i + AMOUNT_1_AXIS * AMOUNT_1_AXIS
						,i - AMOUNT_1_AXIS * AMOUNT_1_AXIS
					
					};
					std::array<vec3, 6> offsets{
						vec3(0,0,-m_SizeVoxel / 2.0f)
						,vec3(0,0,m_SizeVoxel / 2.0f)
						,vec3(-m_SizeVoxel / 2.0f, 0, 0)
						,vec3(m_SizeVoxel / 2.0f, 0, 0)
						,vec3(0,-m_SizeVoxel / 2.0f,0)
						,vec3(0,m_SizeVoxel / 2.0f,0)

					};
					for (size_t j = 0; j < 6; i++)
					{
						if (m_Normals[neighbours[j]] != vec3())
						{
							m_Vertices.push_back(VertexPosColNorm(IndexToXYZ(i) + v, vec4(0.4, 0.4, 0.4, 1), v));
							m_Vertices.push_back(VertexPosColNorm((IndexToXYZ(i) + v + IndexToXYZ(neighbours[j]) + m_Normals[neighbours[j]]/2.0f)/2.0f + offsets[j], vec4(0.4, 0.4, 0.4, 1), v));
							m_Vertices.push_back(VertexPosColNorm((IndexToXYZ(i) + v + IndexToXYZ(neighbours[j]) + m_Normals[neighbours[j]] / 2.0f) / 2.0f - offsets[j], vec4(0.4, 0.4, 0.4, 1), v));
						}
					}
				}

			}
		}
	}
}

vec3 VoxelTerrain::IndexToXYZ(int index)
{
	return vec3(m_SizeVoxel * (index%AMOUNT_1_AXIS), m_SizeVoxel *((index/AMOUNT_1_AXIS)% AMOUNT_1_AXIS), m_SizeVoxel *(index / (AMOUNT_1_AXIS*AMOUNT_1_AXIS)));
}
