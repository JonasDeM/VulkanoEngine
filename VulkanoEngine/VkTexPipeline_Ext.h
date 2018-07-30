#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include "DeriveableHandle.h"
#include "HandleUtilities.h"
#include "VkPipeline_Ext.h"
#include <vector>


class VulkanContext;
//contains the pipeline, the pipelinelayout 

class VkTexPipeline_Ext: public VkPipeline_Ext
{
public:

	unique_ptr_del<VkDescriptorPool> CreateDescriptorPool(VkDevice device, const int uboCount) const final override;
	std::vector<VkDescriptorSet> CreateAndWriteDescriptorSets(VkDevice device, VkDescriptorPool descPool, const std::vector<unique_ptr_del<VkBuffer>>& uniformBuffers, VkImageView texImageView, VkSampler texSampler) const;

	struct UniformBufferObject {
		glm::mat4 world;
		glm::mat4 wvp;
	};

protected:
	VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice device) const final override;
};
