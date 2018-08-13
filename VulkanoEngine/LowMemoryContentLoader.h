#pragma once
#include "ContentLoader.h"
#include <typeinfo>
#include <unordered_map>
#include <sys/types.h>
#include <sys/stat.h>
#include "Debug.h"
#include <memory>
#include <string>

// Never loads content while it is already in memory.
// Unloads the content as soon as there are no owners(have a sharedptr to it) of the content anymore.
template <typename T>
class LowMemoryContentLoader : public ContentLoader<T>
{
public:
	LowMemoryContentLoader() = default;
	virtual ~LowMemoryContentLoader() = default;
	shared_ptr<T> GetContent(const wstring& assetFile) override
	{
		//if content is still used somewhere
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

		shared_ptr<T> content = LoadContent(assetFile);
		if (content != nullptr) 
			m_WeakContentPtrs[assetFile] = content;

		return content;
	};
protected:
	virtual std::shared_ptr<T> LoadContent(const wstring& fileName) const = 0;
private:
	std::unordered_map<std::wstring, std::weak_ptr<T>> m_WeakContentPtrs;
};
