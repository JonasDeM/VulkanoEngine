#pragma once
#include <memory>
#include <chrono>
class VulkanContext;
struct VertexPosCol;
class VulkanDebugRenderer;
class GameScene;
using namespace glm;

class Debug
{
public:

	static void Initialize(VulkanContext * vulkanContext, unsigned int bufferSize = 100);
	static void ToggleDebugRenderer();
	static void CleanUp();

	//TIMING
	static void StartProfileTimer();
	// Prints time between last time this function has been called and now
	static void PrintProfileInterval(const wstring& info);

	//LOGGING
	enum class LogLevel
	{
		Info,
		Warning,
		Error,
		Profile,
		Vulkan
	};

	static void LogInfo(const wstring& msg) { Log(LogLevel::Info, msg); }
	static void LogWarning(const wstring& msg) { Log(LogLevel::Warning, msg); }
	static void LogError(const wstring& msg) { Log(LogLevel::Error, msg); }
	static void LogProfile(const wstring& msg) { Log(LogLevel::Profile, msg); }
	static void Log(LogLevel level, const wstring& msg);
	static void Log(LogLevel level, const string& msg);
	static void LogFormat(LogLevel level, const wchar_t* format, ...);

	//RENDERING
	static void UpdateRenderData(VulkanContext* pVulkanContext, const GameScene* pCurrentScene);
	static void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex);

	static void DrawLine(vec3 start, vec3 end, vec4 color = vec4(1,0,0,1));
	static void DrawLine(vec3 start, vec4 colorStart, vec3 end, vec4 colorEnd);
	static void DrawPhysX(PxScene* pScene);

	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	Debug(const Debug& t) = delete;
	Debug& operator=(const Debug& t) = delete;

private:
	Debug(void);
	~Debug(void);

	//LOGGING
	static wchar_t* m_pConvertBuffer;
	static const unsigned int m_ConvertBufferSize = 1024;

	//RENDERING
	static unique_ptr<VulkanDebugRenderer> m_VulkanDebugRenderer; 
	static void CreateFixedLineList();
	static vec4 ConvertPxColor(PxU32 color);
	static unsigned int m_BufferSize, m_FixedBufferSize, m_DrawVertexCount;
	static vector<VertexPosCol> m_LineList;
	static vector<VertexPosCol> m_FixedLineList;
	static bool m_RendererEnabled;

	//TIMING
	static std::chrono::high_resolution_clock::time_point m_LastProfileCheck;
};

