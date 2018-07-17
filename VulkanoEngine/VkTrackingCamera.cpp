#include "stdafx.h"
#include "VkTrackingCamera.h"
#include <Windows.h>
#include <iostream>

namespace Vulkan {


	TrackingCamera::TrackingCamera(GameSettings* pGameSettings) :
		BaseCamera(pGameSettings),
		m_TotalYaw(0),
		m_TotalPitch(0),
		m_Speed(30.0f),
		m_MouseDownPreviousFrame(false)
	{
		BuildProjection(XM_PIDIV4, 1280.0f / 720.0f, 0.01f, 1000.0f);
	}


	TrackingCamera::~TrackingCamera(void)
	{
	}

	void TrackingCamera::Update(SceneContext* pContext)
	{
		XMVECTOR pos = XMLoadFloat3(&m_Position);
		XMVECTOR target = XMLoadFloat3(&m_Target);
		XMVECTOR vecForward = XMVector3Normalize(target - pos);
		XMStoreFloat3(&m_Forward, vecForward);

		XMVECTOR up = XMVectorSet(0, 1, 0, 0);
		XMVECTOR right = XMVector3Cross(vecForward, up);
		right = XMVector3Normalize(right);

		up = XMVector3Cross(right, vecForward);
		up = XMVector3Normalize(up);

		XMStoreFloat3(&m_Up, up);
		XMStoreFloat3(&m_Right, right);

		XMStoreFloat4x4(&m_View, XMMatrixLookAtLH(pos, target, XMLoadFloat3(&m_Up)));
		BuildViewProjection();
	}

	void TrackingCamera::SetPosition(XMFLOAT3 cameraPosition)
	{
		m_Position = cameraPosition;
	}

	void TrackingCamera::SetTarget(XMFLOAT3 targetPosition)
	{
		m_Target = targetPosition;
	}
}