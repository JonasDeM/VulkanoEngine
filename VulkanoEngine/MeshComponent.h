#pragma once
#include "Component.h"
#include "HandleUtilities.h"
#include "MeshData.h"
#include <type_traits>
#include <memory>
#include <string>
#include <vector>
#include "VkBasicGeometryPipeline_Ext.h"

class TransformComponent;

class MeshComponent :public Component
{
public:
	MeshComponent(wstring assetFile);
	~MeshComponent() = default;
	MeshComponent(const MeshComponent& t) = delete;
	MeshComponent& operator=(const MeshComponent& t) = delete;

private:
	void Build(VulkanContext* vkContext) override;
	void Update(VulkanContext* vkContext) override;
	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, int frameBufferIndex) override;

	void CreateUniformBuffer(VulkanContext* pVkContext);
	void UpdateUniformVariables(VulkanContext* pVkContext);

	TransformComponent* m_pTransform;

	std::shared_ptr<MeshData> m_pMeshData;
	std::wstring m_AssetFile;
	using Pipeline = VkBasicGeometryPipeline_Ext;
	Pipeline* m_MeshPipeline;

	VkBuffer *m_pVertexBuffer, *m_pIndexBuffer; //normal ptr -> no ownership
	std::vector<unique_ptr_del<VkBuffer>> m_UniformBuffers;
	std::vector<unique_ptr_del<VkDeviceMemory>> m_UniformBuffersMemory;

	unique_ptr_del<VkDescriptorPool> m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets; //gets automatically cleaned up with pool
};
