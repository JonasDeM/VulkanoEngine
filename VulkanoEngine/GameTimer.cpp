#pragma once
#include "stdafx.h"
#include "GameTimer.h"
#include "glfw/glfw3.h"
#include "Debug.h"

GameTimer::GameTimer(void) :
	m_ElapsedSeconds(0.0f),
	m_TotalSeconds(0.0f),
	m_BaseTime(0),
	m_PausedTime(0),
	m_StopTime(0),
	m_PrevTime(0),
	m_CurrTime(0),
	m_IsStopped(true),
	m_FPS(0),
	m_FpsTimer(0.0f),
	m_FpsCount(0),
	m_ElapsedUpperBound(0.03f),
	m_ForceElapsedUpperBound(false)
{
}

GameTimer::~GameTimer(void)
{

}

void GameTimer::Update()
{
	if (m_IsStopped)
	{
		m_FPS = 0;
		m_ElapsedSeconds = 0.0f;
		m_TotalSeconds = (float)(((m_StopTime - m_PausedTime) - m_BaseTime) * m_BaseTime);
		return;
	}

	double currTime = glfwGetTime();
	m_CurrTime = currTime;

	m_ElapsedSeconds = m_CurrTime - m_PrevTime;
	m_PrevTime = m_CurrTime;

	if (m_ElapsedSeconds < 0.0f)
		m_ElapsedSeconds = 0.0f;

	if (m_ForceElapsedUpperBound && m_ElapsedSeconds > m_ElapsedUpperBound)
	{
		m_ElapsedSeconds = m_ElapsedUpperBound;
	}

	m_TotalSeconds = (m_CurrTime - m_PausedTime) - m_BaseTime;

	//FPS LOGIC
	m_FpsTimer += m_ElapsedSeconds;
	++m_FpsCount;
	if (m_FpsTimer >= 1.0f)
	{
		m_FPS = m_FpsCount;
		m_FpsCount = 0;
		m_FpsTimer -= 1.0f;
	}
}

void GameTimer::Reset()
{
	double currTime = glfwGetTime();

	m_BaseTime = currTime;
	m_PrevTime = currTime;
	m_StopTime = 0;
	m_FpsTimer = 0.0f;
	m_FpsCount = 0;
	m_IsStopped = false;
}

void GameTimer::Start()
{
	double startTime = glfwGetTime();

	if (m_IsStopped)
	{
		m_PausedTime += (startTime - m_StopTime);

		m_PrevTime = startTime;
		m_StopTime = 0;
		m_IsStopped = false;
	}
}

void GameTimer::Stop()
{
	if (!m_IsStopped)
	{
		double currTime = glfwGetTime();

		m_StopTime = currTime;
		m_IsStopped = true;
	}
}
