#pragma once
#include "Debug.h"
#include <typeindex>
#include <type_traits>
#include <limits>
#include "HandleUtilities.h"
class VkPipeline_Ext;
class VulkanContext;

// Static manager, it manages all the pipelines, so they aren't created more than once.
class PipelineManager 
{
public:
	//Explicitly deleting construction of an object of this class
	PipelineManager() = delete;

	static void Initialize(VulkanContext* pVkContext);
	static void CleanUp();

	//Template parameter: A class that derives from VkPipeline_Ext
	// >Note: class must've been added with PipelineManager::AddPipeline<T>
	template<class T>
	static const T* GetPipeline()
	{
		if (RttiPipelineWrap<T>::id != std::numeric_limits<id_type>::max())
			return static_cast<T*>(m_Pipelines[RttiPipelineWrap<T>::id].get());
		Debug::LogError(L"This pipeline isn't managed by the PipelineManager (use PipelineManager::AddPipeline in Initialize)");
		return nullptr;
	}
private:
	using id_type = uint8_t;

	template <typename T>
	struct RttiPipelineWrap
	{
		static id_type id;
	};

	// Only pipelines derived from VkPipeline_Ext are accepted
	template<class T, typename... Args>
	static void AddPipeline(VkDevice device, Args... as)
	{
		static_assert(std::is_base_of<VkPipeline_Ext, T>::value, "Pipeline must derive from VkPipeline_Ext");
		if (RttiPipelineWrap<T>::id != std::numeric_limits<id_type>::max())
		{
			Debug::LogError(L"Pipeline is already added. Index=" + std::to_wstring(RttiPipelineWrap<T>::id));
			return;
		}
		if (m_Pipelines.size() >= std::numeric_limits<id_type>::max())
		{
			Debug::LogError(L"Maximum pipelines reached! (change id_type in ContentManager to a bigger data type)");
			return;
		}
		RttiPipelineWrap<T>::id = (id_type)m_Pipelines.size();
		m_Pipelines.push_back(CreateExtendedHandle<VkPipeline_Ext>(new T(as...), device));
		m_PipelinesRttiIndices.emplace_back(&RttiPipelineWrap<T>::id);
	}

	static void ClearPipelines()
	{
		m_Pipelines.clear();
		for (auto p : m_PipelinesRttiIndices)
		{
			*p = std::numeric_limits<id_type>::max();
		}
	}
	 
	static std::vector<unique_ptr_del<VkPipeline_Ext>> m_Pipelines;
	static std::vector<id_type*> m_PipelinesRttiIndices; // to make clearing possible
};

template <typename T>
PipelineManager::id_type PipelineManager::RttiPipelineWrap<T>::id = std::numeric_limits<id_type>::max(); //maximum pipelines: 256-1 (need to change? -> change the using id_type to e.g. uint32_t)