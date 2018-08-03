#pragma once
#include <unordered_map>
#include <memory>
#include <type_traits>
#include "ContentLoader.h"
#include "Debug.h"
#include <typeindex>

// Static manager, it manages loaders for every type of content
class ContentManager final
{
public:
	//Explicitly deleting construction of an object of this class
	ContentManager() = delete;

	static void Initialize(VulkanContext * pVkContext);
	static void CleanUp();

	template<class T>
	static std::shared_ptr<T> Load(const wstring& assetFile);

private:
	using id_type = uint8_t;
	
	template <typename T>
	struct RttiContentWrap
	{
		static id_type id;
	};

	// >Register a Loader, so when Load<ContentType> is called, the manager has a Loader to load the ContentType.
	// >Template Parameter: Loader: a class derived from ContentLoader<T>
	// >Arguments: Loader constructor arguments.
	// >Notes: Only one Loader per ContentType!
	template<class Loader, typename... Args>
	static void AddLoaderNew(Args... as);

	static std::vector<std::unique_ptr<BaseLoader>> m_Loaders;
};

template <typename T>
ContentManager::id_type ContentManager::RttiContentWrap<T>::id = std::numeric_limits<id_type>::max();

template<class T>
std::shared_ptr<T> ContentManager::Load(const wstring & assetFile)
{
	if (RttiContentWrap<T>::id != std::numeric_limits<id_type>::max())
		return static_cast<ContentLoader<T>*>(m_Loaders[RttiContentWrap<T>::id].get())->GetContent(assetFile);
	Debug::LogError(L"There is no loader registered in the ContentManager that can load this type of content");
	return nullptr;
}

template<class Loader, typename ...Args>
void ContentManager::AddLoaderNew(Args ...as)
{
	//1. Determine the type of content Loader loads
	//*********************************************

	// c++11 option A
	//using ContentType = std::remove_pointer<decltype(decltype(Loader(as...).GetContent(L""))(nullptr).get())>;  

	// c++11 option B
	using ContentSmartPtrType = std::result_of_t<decltype(&Loader::GetContent)(Loader, const std::wstring&)>;
	using ContentType = std::remove_pointer_t<decltype(ContentSmartPtrType().get())>;

	// c++17 (deprecates result_of<T>)
	//using ContentSmartPtrType = std::invoke_result_t<decltype(&Loader::GetContent), Loader, const std::wstring&>;
	//using ContentType = std::remove_pointer_t<decltype(ContentSmartPtrType().get())>;

	//2. Check if Loader Derives from BaseLoader
	//******************************************
	static_assert(std::is_base_of<BaseLoader, Loader>::value, "template param should be derived from BaseLoader");

	//3. Use custom rtti wrapper to store a unique loader per ContentType 
	//*******************************************************************
	if (RttiContentWrap<ContentType>::id != std::numeric_limits<id_type>::max())
	{
		Debug::LogWarning(L"A Loader for that type of content is already added. Index=" + std::to_wstring(RttiContentWrap<ContentType>::id));
		return;
	}
	if (m_Loaders.size() >= std::numeric_limits<id_type>::max())
	{
		Debug::LogError(L"Maximum loaders reached! (change id_type in LoaderManager to a bigger data type)");
		return;
	}
	RttiContentWrap<ContentType>::id = (id_type)m_Loaders.size();
	m_Loaders.emplace_back(std::make_unique<Loader>(as...));
}