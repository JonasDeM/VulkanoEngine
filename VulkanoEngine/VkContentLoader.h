#pragma once
#include "stdafx.h"
#include <typeinfo>
#include <unordered_map>
#include <sys/types.h>
#include <sys/stat.h>
#include "Debug.h"
#include "BaseLoader.h"
#include <memory>


template <class T>
class VkContentLoader : public VkBaseLoader
{
public:
	VkContentLoader() {};
	virtual const type_info& GetType() const override { return typeid(T); }

	shared_ptr<T> GetContent(const wstring& assetFile, VulkanContext* vkContext)
	{
		//if file is already loaded
		auto it = m_WeakContentPtrs.find(assetFile);
		if (it != m_WeakContentPtrs.end() && !(it->second.expired()))
		{
			return it->second.lock();
		}

		//if file needs to be loaded
		struct _stat buff;
		int result = -1;
		result = _wstat(assetFile.c_str(), &buff);
		if (result != 0)
		{
			wstringstream ss;
			ss << "File not found!\nPath: ";
			ss << assetFile;
			Debug::LogError(ss.str());
		}

		shared_ptr<T> content = LoadContent(assetFile, vkContext);
		if (content != nullptr)
			m_WeakContentPtrs[assetFile] = content;

		return content;
	}
protected:
	virtual shared_ptr<T> LoadContent(const wstring& assetFile, VulkanContext* vkContext) const = 0;

private:
	static unordered_map<wstring, weak_ptr<T>> m_WeakContentPtrs;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	VkContentLoader(const VkContentLoader &obj);
	VkContentLoader& operator=(const VkContentLoader& obj);
};

template<class T>
unordered_map<wstring, weak_ptr<T>> VkContentLoader<T>::m_WeakContentPtrs = unordered_map<wstring, weak_ptr<T>>();
