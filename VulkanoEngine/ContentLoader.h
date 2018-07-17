#pragma once
#include "stdafx.h"
#include "BaseLoader.h"
#include <typeinfo>
#include <unordered_map>
#include <sys/types.h>
#include <sys/stat.h>
#include "Debug.h"
#include <memory>

template <class T>
class ContentLoader : public BaseLoader
{
public:
	ContentLoader() {};
	virtual const type_info& GetType() const override { return typeid(T); }
	shared_ptr<T> GetContent(const wstring& assetFile)
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

		shared_ptr<T> content = LoadContent(assetFile);
		if (content != nullptr) 
			m_WeakContentPtrs[assetFile] = content;

		return content;
	}

protected:
	virtual shared_ptr<T> LoadContent(const wstring& assetFile) const = 0;

private:
	static unordered_map<wstring, weak_ptr<T>> m_WeakContentPtrs;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	ContentLoader( const ContentLoader &obj);
	ContentLoader& operator=( const ContentLoader& obj );
};

template<class T>
unordered_map<wstring, weak_ptr<T>> ContentLoader<T>::m_WeakContentPtrs = unordered_map<wstring, weak_ptr<T>>();
