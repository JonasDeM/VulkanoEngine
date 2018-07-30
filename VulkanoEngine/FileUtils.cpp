
#include "stdafx.h"
#include "FileUtils.h"
#include <locale>
#include <codecvt>

namespace FileUtils {
	string ToString(wstring wstr)
	{
		//setup converter
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;

		//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
		return converter.to_bytes(wstr);
	}

	wstring ToWString(string str)
	{
		//setup converter
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;

		//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
		return converter.from_bytes(str);
	}

	std::vector<char> ReadFile(const char* filePath)
	{
		std::ifstream file(filePath, std::ios::ate | std::ios::binary);

		if (!file.is_open())
			throw std::runtime_error("failed to open file!");

		auto fileSize = file.tellg();
		std::vector<char> buffer(static_cast<size_t>(fileSize)); //we started at the end so we can know the filesize by asking the position of the current character in the input stream.

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}

	std::vector<char> ReadFile(wstring filePath)
	{
		return ReadFile(ToString(filePath).c_str());
	}
}