#pragma once
#include "stdafx.h"
#include "GimbalRig.h"
#include "CylinderPosColorNorm.h"
#include "TorusPosColorNorm.h"
#include "CubePosColorNorm.h"
#include "GameScene.h"
#include "GameTimer.h"

GimbalRig::GimbalRig(void) :
	m_pBase(nullptr),
	m_pPillar0(nullptr),
	m_pPillar1(nullptr),
	m_pOuterTorus(nullptr),
	m_pMiddleTorus(nullptr),
	m_pInnerTorus(nullptr),
	m_pJoint0(nullptr),
	m_pJoint1(nullptr)
{
}

GimbalRig::~GimbalRig(void)
{
}

void GimbalRig::Initialize(VulkanContext* pVkContext)
{
	m_pBase = new CylinderPosColorNorm(8, 40, 0.5, glm::DarkGrey);
	m_pPillar0 = new CylinderPosColorNorm(0.5f, 10, 15, glm::LightGrey);
	m_pPillar0->Translate(-6.5f, 7.5f, 0.f);
	m_pPillar1 = new CylinderPosColorNorm(0.5f, 10, 15, glm::LightGrey);
	m_pPillar1->Translate(6.5f, 7.5f, 0.f);

	m_pOuterTorus = new TorusPosColorNorm(5.75f, 30, 0.25f, 10, glm::Red);
	m_pOuterTorus->Translate(0.f, 15.f, 0.f);
	m_pMiddleTorus = new TorusPosColorNorm(5.25f, 30, 0.25f, 10, glm::Green);
	m_pInnerTorus = new TorusPosColorNorm(4.75f, 30, 0.25f, 10, glm::Blue);

	float size = 1.1f;
	m_pJoint0 = new CubePosColorNorm(size, size, size);
	m_pJoint0->Translate(0.f, 7.5f, 0.f);
	m_pJoint1 = new CubePosColorNorm(size, size, size);
	m_pJoint1->Translate(0.f, 7.5f, 0.f);

	AddChild(m_pBase);
	m_pBase->AddChild(m_pPillar0);
	m_pPillar0->AddChild(m_pJoint0);
	m_pBase->AddChild(m_pPillar1);
	m_pPillar1->AddChild(m_pJoint1);
	m_pBase->AddChild(m_pOuterTorus);
	m_pOuterTorus->AddChild(m_pMiddleTorus);
	m_pMiddleTorus->AddChild(m_pInnerTorus);
}

void GimbalRig::Update(VulkanContext* pVkContext)
{
	float totSec = GetScene()->GetTimer()->GetTotalSecondsF();

	m_pOuterTorus->RotateEuler(totSec * 90, 0, 0);
	m_pMiddleTorus->RotateEuler(0, totSec * 90, 0);
	m_pInnerTorus->RotateEuler(totSec * 90, 0, 0);
}
