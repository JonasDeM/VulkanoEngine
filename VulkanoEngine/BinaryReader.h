#pragma once

#include "Debug.h"
#include <memory>

class BinaryReader
{
public:
	BinaryReader(void);
	~BinaryReader();


	template<class T>
	T Read() const
	{
		if(m_pReader==nullptr)
			Debug::LogError(L"BinaryReader doesn't exist!\nUnable to read binary data...");

		T value;
		m_pReader->read((char*)&value, sizeof(T));
		return value;
	}

	wstring ReadString() const;
	wstring ReadLongString() const;
	wstring ReadNullString() const;

	int GetBufferPosition() const;
	bool SetBufferPosition(int pos);
	bool MoveBufferPosition(int move);
	bool Exists() const { return m_Exists; }

	void Open(wstring binaryFile);
	void Open(char* s, unsigned int size);
	void Close();

private: 

	bool m_Exists;
	std::unique_ptr<istream> m_pReader;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	BinaryReader(const BinaryReader& yRef);									
	BinaryReader& operator=(const BinaryReader& yRef);
};

