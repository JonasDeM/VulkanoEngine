#pragma once
#include "stdafx.h"
#include "Debug.h"
#include "ContentManager.h"
#include "VertexStructs.h"
#include "GameContext.h"
#include "VulkanDebugRenderer.h"
#include "GameContext.h"
#include "VulkanContext.h"
#include "Colors.h"
#include "GameScene.h"
#include <limits>
#include "FileUtils.h"

#include <fcntl.h>
//#include <stdio.h>
//#include <stdlib.h>


unsigned int Debug::m_BufferSize = 100;
unsigned int Debug::m_DrawVertexCount = 0;
unique_ptr<VulkanDebugRenderer> Debug::m_VulkanDebugRenderer = nullptr;
vector<VertexPosCol> Debug::m_LineList = vector<VertexPosCol>();
vector<VertexPosCol> Debug::m_FixedLineList = vector<VertexPosCol>();
unsigned int Debug::m_FixedBufferSize = 0;
bool Debug::m_RendererEnabled = true;
wchar_t* Debug::m_pConvertBuffer = new wchar_t[m_ConvertBufferSize];
std::chrono::high_resolution_clock::time_point Debug::m_LastProfileCheck;

Debug::Debug(void)
{
}


Debug::~Debug(void)
{
}

void Debug::CleanUp()
{	
	m_VulkanDebugRenderer.reset();

	delete[] m_pConvertBuffer;
	m_pConvertBuffer = nullptr;
}

void Debug::StartProfileTimer()
{
	m_LastProfileCheck = std::chrono::high_resolution_clock::now();
}

//void Debug::PrintProfileInterval(const wstring& info){}

void Debug::PrintProfileInterval(const wstring& info)
{
	auto t = std::chrono::high_resolution_clock::now();

	auto durationNanoSec = t - m_LastProfileCheck;
	double durationMiliSec = durationNanoSec.count()/1000000.0;
	auto durationSec = std::chrono::duration_cast<std::chrono::duration<double>>(t - m_LastProfileCheck);

	LogProfile(L"s:" + to_wstring(durationSec.count())+ L"\t ms:" + to_wstring(durationMiliSec) + L"\t ns:" + to_wstring(durationNanoSec.count()) + L"\t  -" + info);

	m_LastProfileCheck = std::chrono::high_resolution_clock::now();
}

void Debug::Initialize(VulkanContext *vulkanContext, unsigned int bufferSize)
{
	m_BufferSize = bufferSize;
	CreateFixedLineList();

	m_VulkanDebugRenderer = std::make_unique<VulkanDebugRenderer>(vulkanContext, m_FixedLineList, m_BufferSize, m_FixedBufferSize);
	m_DrawVertexCount = m_FixedBufferSize;
}

void Debug::CreateFixedLineList()
{
	//*GRID*
	unsigned int numGridLines = 20;
	float gridSpacing = 1.0f;

	float startOffset = -((int)numGridLines / 2)*gridSpacing;
	float size = (numGridLines - 1) * gridSpacing;
	auto gridColor = glm::LightGrey;
	for (unsigned int i = 0; i < numGridLines; ++i)
	{
		//VERTICAL
		float lineOffset = startOffset + gridSpacing * i;
		auto vertStart = vec3(startOffset, 0, lineOffset);
		m_FixedLineList.push_back(VertexPosCol(vertStart, gridColor));
		vertStart.x += size;
		m_FixedLineList.push_back(VertexPosCol(vertStart, gridColor));

		//HORIZONTAL
		vertStart = vec3(lineOffset, 0, startOffset);
		m_FixedLineList.push_back(VertexPosCol(vertStart, gridColor));
		vertStart.z += size;
		m_FixedLineList.push_back(VertexPosCol(vertStart, gridColor));
	}

	//*AXIS
	m_FixedLineList.push_back(VertexPosCol(vec3(0, 0, 0), glm::DarkRed));
	m_FixedLineList.push_back(VertexPosCol(vec3(30, 0, 0), glm::DarkRed));
	m_FixedLineList.push_back(VertexPosCol(vec3(0, 0, 0), glm::DarkGreen));
	m_FixedLineList.push_back(VertexPosCol(vec3(0, 30, 0), glm::DarkGreen));
	m_FixedLineList.push_back(VertexPosCol(vec3(0, 0, 0), glm::DarkBlue));
	m_FixedLineList.push_back(VertexPosCol(vec3(0, 0, 30), glm::DarkBlue));

	//@END!
	m_FixedBufferSize = (unsigned int)m_FixedLineList.size();
}

#pragma region
void Debug::LogFormat(LogLevel level, const wchar_t* format, ...)
{
	va_list ap;

	va_start(ap, format);
	_vsnwprintf_s(&m_pConvertBuffer[0], m_ConvertBufferSize, m_ConvertBufferSize, format, ap);
	va_end(ap);
	Log(level, wstring(&m_pConvertBuffer[0]));
}

void Debug::Log(LogLevel level, const wstring& msg)
{
	switch (level)
	{
	case LogLevel::Info:
		wcout << L"[INFO]    >>";
		break;
	case LogLevel::Warning:
		wcout << L"[WARNING] >>";
		break;
	case LogLevel::Vulkan:
		wcout << L"[VULKAN] >>";
		break;
	case LogLevel::Error:
		wcout << L"[ERROR]   >>";
		throw new std::runtime_error(FileUtils::ToString(msg));
	case LogLevel::Profile:
		wcout << L"[PROFILE]   >>";
		break;
	}
	wcout << msg << endl;
}
void Debug::Log(LogLevel level, const string& msg)
{
	switch (level)
	{
	case LogLevel::Info:
		cout << "[INFO]    >>";
		break;
	case LogLevel::Warning:
		cout << "[WARNING] >>";
		break;
	case LogLevel::Vulkan:
		cout << "[VULKAN] >>";
		break;
	case LogLevel::Error:
		cout << "[ERROR]   >>";
		throw new std::runtime_error(msg);
	case LogLevel::Profile:
		cout << "[PROFILE]   >>";
		break;
	}
	cout << msg << endl;
}
#pragma endregion Logger Methods
#pragma region
void Debug::ToggleDebugRenderer()
{
	m_RendererEnabled = !m_RendererEnabled;
	SceneManager::FlagDrawChanges();
}

void Debug::DrawLine(vec3 start, vec3 end, vec4 color)
{
	if (!m_RendererEnabled)
		return;

	m_LineList.push_back(VertexPosCol(start, color));
	m_LineList.push_back(VertexPosCol(end, color));
}

void Debug::DrawLine(vec3 start, vec4 colorStart, vec3 end, vec4 colorEnd)
{
	if (!m_RendererEnabled)
		return;

	m_LineList.push_back(VertexPosCol(start, colorStart));
	m_LineList.push_back(VertexPosCol(end, colorEnd));
}

void Debug::DrawPhysX(PxScene* pScene)
{
	if (!m_RendererEnabled)
		return;

	auto pxDebugRenderer = &pScene->getRenderBuffer();
	auto debugLines = pxDebugRenderer->getLines();

	for (unsigned int i = 0; i < pxDebugRenderer->getNbLines(); ++i)
	{
		auto line = debugLines[i];
		DrawLine(vec3(line.pos0.x, line.pos0.y, line.pos0.z), ConvertPxColor(line.color0), vec3(line.pos1.x, line.pos1.y, line.pos1.z), ConvertPxColor(line.color1));
	}
}

glm::vec4 Debug::ConvertPxColor(PxU32 color)
{
	//TODO: Check performance, Bitshift+divide vs switch
	switch (color)
	{
	case 0xFF000000:
		return glm::Black;
	case 0xFFFF0000:
		return glm::Red;
	case 0xFF00FF00:
		return glm::Green;
	case 0xFF0000FF:
		return glm::Blue;
	case 0xFFFFFF00:
		return glm::Yellow;
	case 0xFFFF00FF:
		return glm::Magenta;
	case 0xFF00FFFF:
		return glm::Cyan;
	case 0xFFFFFFFF:
		return glm::White;
	case 0xFF808080:
		return glm::Grey;
	case 0x88880000:
		return glm::DarkRed;
	case 0x88008800:
		return glm::DarkGreen;
	case 0x88000088:
		return glm::DarkBlue;
	default:
		return glm::Black;
	}
}

void Debug::UpdateRenderData(VulkanContext* pVulkanContext, const GameScene* pCurrentScene)
{
	if (!m_RendererEnabled || m_VulkanDebugRenderer == nullptr)
		return;

	unsigned int size = (unsigned int)m_LineList.size();
	if ((size_t)std::numeric_limits<unsigned int>::max() < m_LineList.size())
	{
		size = std::numeric_limits<unsigned int>::max();
		LogError(L"Max VertexCount reached for DebugRenderer!");
	}
	else if ((size + m_FixedBufferSize) <= 0)
		return;
	else if (size > m_BufferSize)
	{
		//LogInfo(L"Debug Renderer: Increasing Vertexbuffer Size!");
		m_BufferSize = size;
		m_VulkanDebugRenderer->CreateVertexBuffer(pVulkanContext, m_FixedLineList, m_BufferSize, m_FixedBufferSize);
	}
	
	if (size>0)
		m_VulkanDebugRenderer->UpdateVertexData(pVulkanContext, m_LineList, m_FixedBufferSize);
	m_VulkanDebugRenderer->UpdateUniformVariables(pVulkanContext, pCurrentScene);

	if (m_DrawVertexCount != m_FixedBufferSize + size)
	{
		SceneManager::FlagDrawChanges();
		m_DrawVertexCount = m_FixedBufferSize + size;
	}
	m_LineList.clear();
}

void Debug::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex)
{
	if (m_RendererEnabled)
		m_VulkanDebugRenderer->RecordVulkanDrawCommands(cmdBuffer, m_DrawVertexCount, frameBufferIndex);
}
#pragma endregion Renderer Methods