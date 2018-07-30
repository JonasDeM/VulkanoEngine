#pragma once
#include "Singleton.h"
#include <unordered_map>
#include <memory>
#include "ContentLoader.h"
#include "Debug.h"
#include <typeindex>

using namespace std;

// Static manager, it manages loaders for every type of content
class ContentManager final
{
public:
	//Explicitly deleting construction of an object of this class
	ContentManager() = delete;

	static void Initialize(VulkanContext * pVkContext);
	static void CleanUp();

	// Pass raw pointer to a loader you created, ContentManager takes ownership!
	static void AddLoader(BaseLoader* loader);

	template<class T> 
	static shared_ptr<T> Load(const wstring& assetFile)
	{
		if (m_Loaders.find(typeid(T)) != m_Loaders.end())
		{
			ContentLoader<T>* pLoader = static_cast<ContentLoader<T>*>(m_Loaders[typeid(T)].get());
			return pLoader->GetContent(assetFile);
		} 

		Debug::LogError(L"Loader not found!");
		return nullptr;
	}

private:

	static unordered_map<std::type_index, std::unique_ptr<BaseLoader>> m_Loaders;
};
