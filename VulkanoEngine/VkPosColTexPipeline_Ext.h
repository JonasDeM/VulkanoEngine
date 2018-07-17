#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include "DeriveableHandle.h"
#include "HandleUtilities.h"
#include "VkPipeline_Ext.h"


class VulkanContext;
//contains the pipeline, the pipelinelayout 

class VkPosColTexPipeline_Ext : public VkPipeline_Ext
{
public:
	VkPosColTexPipeline_Ext(VulkanContext* pVkContext);

	void Destroy(VkDevice device) const override
	{
		vkDestroyPipeline(device, *this, nullptr);
	}


	unique_ptr_del<VkDescriptorPool> CreateDescriptorPool(VkDevice device) override;
	void CreateDescriptorSetLayout(VkDevice device) override;
	VkDescriptorSet CreateAndWriteDescriptorSet(VkDevice device, VkDescriptorPool descPool, VkBuffer uniformBuffer, VkImageView texImageView, VkSampler texSampler);

	struct UniformBufferObject {
		glm::mat4 world;
		glm::mat4 wvp;
	};

};
