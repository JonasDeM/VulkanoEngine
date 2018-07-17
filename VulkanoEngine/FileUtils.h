#pragma once

#include <vector>
#include <fstream>

namespace FileUtils
{
	std::vector<char> ReadFile(const char* filePath);

	string ToString(wstring wstr);
	wstring ToWString(string str);
};

