#pragma once
#include "VkBaseCamera.h"


namespace Vulkan {

	class TrackingCamera : public BaseCamera
	{
	public:
		TrackingCamera(GameSettings* pGameSettings);
		~TrackingCamera(void);

		virtual void Update(SceneContext* pContext);
		void SetPosition(XMFLOAT3 cameraPosition);
		void SetTarget(XMFLOAT3 targetPosition);
	protected:

	private:
		POINT m_PrevCursorPosition;
		float m_TotalYaw, m_TotalPitch;
		float m_Speed;
		bool m_MouseDownPreviousFrame;
		XMFLOAT3 m_Direction;

		// -------------------------
		// Disabling default copy constructor and default 
		// assignment operator.
		// -------------------------
		TrackingCamera(const TrackingCamera& t);
		TrackingCamera& operator=(const TrackingCamera& t);
	};
}
