#pragma once
#include "Debug.h"
#include <typeindex>
#include <type_traits>
#include "HandleUtilities.h"
class VkPipeline_Ext;
class VulkanContext;

class PipelineManager 
{
public:
	//Explicitly deleting construction of an object of this class
	PipelineManager() = delete;

	static void Initialize(VulkanContext* pVkContext);
	static void CleanUp();

	template<class T>
	static const T* GetPipeline()
	{
		if (m_Pipelines.find(typeid(T)) != m_Pipelines.end())
			return static_cast<T*>(m_Pipelines[typeid(T)].get());
		Debug::LogError(L"This pipeline isn't managed by the PipelineManager (use PipelineManager::AddPipeline in Initialize)");
		return nullptr;
	}

protected:
	// Only pipelines derived from VkPipeline_Ext are accepted;
	// Only pipelines with a default constructor work (If needed, Implement an AddPipeline method that takes a raw pointer and takes ownership)
	template<class T>
	static void AddPipeline(VkDevice device)
	{
		static_assert(std::is_base_of<VkPipeline_Ext, T>::value, "Pipeline must derive from VkPipeline_Ext");
		if (m_Pipelines.find(typeid(T)) != m_Pipelines.end())
		{
			Debug::LogError(L"Pipeline is already added.");
			return;
		}
		const std::type_index typeIndex(typeid(T));
		m_Pipelines[typeIndex] = CreateExtendedHandle<VkPipeline_Ext>(new T(), device);
	}

	static unordered_map<std::type_index, unique_ptr_del<VkPipeline_Ext>> m_Pipelines;
};
