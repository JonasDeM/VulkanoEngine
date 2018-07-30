#pragma once


//When to use Singleton (if you answer yes to EVERY point):
//								* Only 1 instance of an object in the entire program.
//								* Derives from a base class
//								* You need it to be accessible in a lot of places.
//
//When NOT to use Singleton (if you answer yes to 1 point):	
//								* Manager class that doesn't derive from a base class: maybe use static manager.
//								* For convienience, but you don't want to make a global. (bad design)
template<class T>
class Singleton
{
public:
	
	static T* GetInstance()
	{
		if(!m_Instance)
		{
			m_Instance = new T();
		}

		return m_Instance;
	}

	static void DestroyInstance()
	{
		delete m_Instance;
		m_Instance = nullptr;
	}

private:
	static T* m_Instance;
};

template<class T> 
T* Singleton<T>::m_Instance = nullptr;

