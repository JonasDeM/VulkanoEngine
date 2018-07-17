#pragma once
#include "HandleUtilities.h"
#include "Singleton.h"
class VkPosColTexPipeline_Ext;
class VkDebugPipeline_Ext;
class VkPosColNormPipeline_Ext;
class VkPosNormTexPipeline_Ext;
class VkPosNormTex2SPipeline_Ext;
class VulkanContext;

class VkPipelineManager : public Singleton<VkPipelineManager>
{
public:
	void Initialize(VulkanContext* pVkContext);
	
	VkPosColTexPipeline_Ext* GetPosColTexPipeline() const { return m_pVkPosColTexPipeline.get(); }
	VkPosColNormPipeline_Ext* GetPosColNormPipeline() const { return m_pVkPosColNormPipeline.get(); }
	VkPosNormTexPipeline_Ext* GetPosNormTexPipeline() const { return m_pVkPosNormTexPipeline.get(); }
	VkPosNormTex2SPipeline_Ext* GetPosNormTex2SPipeline() const { return m_pVkPosNormTex2SPipeline.get(); }
	VkDebugPipeline_Ext* GetDebugPipeline() const { return m_pVkDebugPipeline.get(); }

private:
	friend class Singleton<VkPipelineManager>;

	VkPipelineManager() {};
	~VkPipelineManager() {};

	unique_ptr_del<VkPosColTexPipeline_Ext> m_pVkPosColTexPipeline = nullptr;
	unique_ptr_del<VkPosColNormPipeline_Ext> m_pVkPosColNormPipeline = nullptr;
	unique_ptr_del<VkPosNormTexPipeline_Ext> m_pVkPosNormTexPipeline = nullptr;
	unique_ptr_del<VkPosNormTex2SPipeline_Ext> m_pVkPosNormTex2SPipeline = nullptr;
	unique_ptr_del<VkDebugPipeline_Ext> m_pVkDebugPipeline = nullptr;

};

