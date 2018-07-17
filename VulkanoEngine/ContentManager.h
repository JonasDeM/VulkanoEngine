#pragma once
#include "Singleton.h"
#include <unordered_map>
#include <memory>
#include "ContentLoader.h"
#include "VkContentLoader.h"
#include "Debug.h"

using namespace std;

class ContentManager : public Singleton<ContentManager>
{
public:
	void AddLoader(BaseLoader* loader);
	void AddVkLoader(VkBaseLoader* loader);

	template<class T> 
	shared_ptr<T> Load(const wstring& assetFile)
	{
		const size_t hash = typeid(T).hash_code();

		if (m_Loaders.find(hash) != m_Loaders.end())
		{
			ContentLoader<T>* pLoader = static_cast<ContentLoader<T>*>(m_Loaders[hash].get());
			return pLoader->GetContent(assetFile);
		}

		Debug::LogError(L"Loader not found!");
		return nullptr;
	}

	template<class T>
	shared_ptr<T> LoadVk(const wstring& assetFile, VulkanContext* vkContext)
	{
		const size_t hash = typeid(T).hash_code();

		if (m_VkLoaders.find(hash) != m_VkLoaders.end())
		{
			VkContentLoader<T>* pLoader = static_cast<VkContentLoader<T>*>(m_VkLoaders[hash].get());
			return pLoader->GetContent(assetFile, vkContext);
		}

		Debug::LogError(L"Loader not found!");
		return nullptr;
	}

private:
	friend class Singleton<ContentManager>;
	ContentManager();
	~ContentManager() {};
	unordered_map<size_t, std::unique_ptr<BaseLoader>> m_Loaders;
	unordered_map<size_t, std::unique_ptr<VkBaseLoader>> m_VkLoaders;

	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	ContentManager(const ContentManager& t);
	ContentManager& operator=(const ContentManager& t);
};
