#pragma once
#include "GameObject.h"

class CylinderPosColorNorm;
class TorusPosColorNorm;
class CubePosColorNorm;

class GimbalRig : public GameObject
{
public:
	GimbalRig(void);
	~GimbalRig(void);
	GimbalRig(const GimbalRig& t) = delete;
	GimbalRig& operator=(const GimbalRig& t) = delete;

	virtual void Initialize(VulkanContext* pVkContext) override;
	virtual void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex) override {};
	virtual void Update(VulkanContext* pVkContext) override;

private:

	CylinderPosColorNorm *m_pBase, *m_pPillar0, *m_pPillar1;
	TorusPosColorNorm *m_pOuterTorus, *m_pMiddleTorus, *m_pInnerTorus;
	CubePosColorNorm *m_pJoint0, *m_pJoint1;
};