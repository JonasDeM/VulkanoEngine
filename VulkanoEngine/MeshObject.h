#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include <glm\glm.hpp>
#include "VkPhysicalDevice_Ext.h"
#include "VkBasicGeometryPipeline_Ext.h"
#include "HandleUtilities.h"
#include "VkImageView_Ext.h"


class MeshData;
class VulkanContext;

class MeshObject : public GameObject
{
public:
	MeshObject(wstring assetFile, bool isStatic=false);
	~MeshObject(void);

	virtual void Initialize(VulkanContext* pVkContext) override;
	virtual void Update(VulkanContext* pVkContext) override;

private:
	void CreateUniformBuffer(VulkanContext* pVkContext);
	void UpdateUniformVariables(VulkanContext* pVkContext);
	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, int frameBufferIndex) override;

	shared_ptr<MeshData> m_pMeshData;
	wstring m_AssetFile;

	VkBuffer *m_pVertexBuffer, *m_pIndexBuffer; //normal ptr -> no ownership
	std::vector<unique_ptr_del<VkBuffer>> m_UniformBuffers;
	std::vector<unique_ptr_del<VkDeviceMemory>> m_UniformBuffersMemory;

	unique_ptr_del<VkDescriptorPool> m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets; //gets automatically cleaned up with pool

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	MeshObject(const MeshObject& t);
	MeshObject& operator=(const MeshObject& t);
};
