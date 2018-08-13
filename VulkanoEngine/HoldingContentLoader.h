#pragma once
#include "ContentLoader.h"
#include <typeinfo>
#include <unordered_map>
#include <sys/types.h>
#include <sys/stat.h>
#include "Debug.h"
#include <memory>
#include <string>

// Keeps the content in memory as long as loader exists (even when nothing else currently has ownership)
// Gives you the option to manually clean up content.
// Never loads content while it is already in memory.
template <typename T>
class HoldingContentLoader : public ContentLoader<T>
{
public:
	HoldingContentLoader() = default;
	virtual ~HoldingContentLoader() = default;
	shared_ptr<T> GetContent(const wstring& assetFile) override
	{
		//if content is still used somewhere
		auto it = m_SharedContentPtrs.find(assetFile);
		if (it != m_SharedContentPtrs.end())
		{
			return it->second;
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
			m_SharedContentPtrs[assetFile] = content;

		
		return content;
	};

protected:
	virtual std::shared_ptr<T> LoadContent(const wstring& fileName) const = 0;
private:
	std::unordered_map<std::wstring, std::shared_ptr<T>> m_SharedContentPtrs;
};
