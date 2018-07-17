#include "stdafx.h"
#include "BinaryReader.h"


BinaryReader::BinaryReader(void):
	m_Exists(false)
{
}

BinaryReader::~BinaryReader()
{
}

wstring BinaryReader::ReadLongString() const
{
	if(m_pReader==nullptr)
		Debug::LogError(L"BinaryReader doesn't exist!\nUnable to read binary data...");

	auto stringLength = Read<unsigned int>();
	
	wstringstream ss;
	for(unsigned int i=0; i<stringLength; ++i)
	{
		ss<<Read<wchar_t>();
	}

	return (wstring)ss.str();
}

wstring BinaryReader::ReadNullString() const
{
	if(m_pReader==nullptr)
		Debug::LogError(L"BinaryReader doesn't exist!\nUnable to read binary data...");

	string buff;
	getline(*m_pReader, buff, '\0');

	return wstring(buff.begin(),buff.end());
}


wstring BinaryReader::ReadString() const
{
	if(m_pReader==nullptr)
		Debug::LogError(L"BinaryReader doesn't exist!\nUnable to read binary data...");

	int stringLength = (int)Read<char>();
	
	wstringstream ss;
	for(int i=0; i<stringLength; ++i)
	{
		ss<<Read<char>();
	}

	return (wstring)ss.str();
}

void BinaryReader::Open(wstring binaryFile) 
{
	auto temp = new ifstream();
	temp->open(binaryFile, ios::in|ios::binary);
	if(temp)
	{
		m_pReader.reset(temp);
		m_Exists = true;
	}
	else
	{
		wstringstream ss;
		ss<<L"BinaryReader::Open> Failed to open the file!\nBinaryFile: ";
		ss<<binaryFile;
		Debug::LogWarning(ss.str());
		Close();
	}
}

void BinaryReader::Open(char* s, unsigned int size)
{
	string data(s, size);
	m_pReader.reset(new istringstream(data));
	m_Exists = true;
}

void BinaryReader::Close()
{
	m_pReader.reset(nullptr);
	m_Exists = false;
}

int BinaryReader::GetBufferPosition() const
{
	if(m_pReader)
	{
		return static_cast<int>(m_pReader->tellg());
	}

	Debug::LogWarning(L"BinaryReader::GetBufferPosition> m_pReader doesn't exist");
	return -1;
}

bool BinaryReader::SetBufferPosition(int pos)
{
	if(m_pReader)
	{
		m_pReader->seekg(pos);
		return true;
	}

	Debug::LogWarning(L"BinaryReader::SetBufferPosition> m_pReader doesn't exist");
	return false;
}

bool BinaryReader::MoveBufferPosition(int move)
{
	auto currPos = GetBufferPosition();
	if(currPos>0)
	{
		return SetBufferPosition(currPos + move);
	}

	return false;
}
