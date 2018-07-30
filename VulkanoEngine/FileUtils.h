#pragma once

#include <vector>
#include <fstream>

namespace FileUtils
{
	std::vector<char> ReadFile(const char* filePath);
	std::vector<char> ReadFile(wstring filePath);

	string ToString(wstring wstr);
	wstring ToWString(string str);
};

