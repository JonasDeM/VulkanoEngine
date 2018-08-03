#pragma once
#include "BaseLoader.h"

template <class T>
class ContentLoader : public BaseLoader
{
public:
	ContentLoader() = default;
	virtual ~ContentLoader() = default;
	ContentLoader(const ContentLoader &obj) = delete;
	ContentLoader& operator=(const ContentLoader& obj) = delete;
	virtual shared_ptr<T> GetContent(const wstring& assetFile) = 0;
};