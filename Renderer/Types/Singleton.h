#ifndef _SINGLETON_H
#define _SINGLETON_H

namespace AfterGlow
{

template<class T>
class Singleton;

#define CreateSingleton(T); \
	template<> T* AfterGlow::Singleton<T>::pInstance = 0;

#define DestroySingleton(T); \
	AfterGlow::Singleton<T>::destroyInstance();

//
// @TODO: Make threadsafe
//

template <class T>
class Singleton
{
public:
	//
	// Returns a mutable reference to the singleton object
	//
	static T& instance()
	{
		return *Singleton<T>::_getInstance();
	}

	//
	// Returns a constant reference to the singleton object
	//
	static const T& constInstance()
	{
		return *Singleton<T>::_getInstance();
	}

	//
	// Destroy the single object
	//
	static void destroyInstance()
	{
		delete Singleton<T>::_getInstance();
	}

protected:
	Singleton()
	{
	}

	virtual ~Singleton()
	{
		delete Singleton<T>::_getInstance();
	}

private:
	static T* pInstance;

	static T* _getInstance()
	{
		if(Singleton<T>::pInstance == NULL)
		{
			pInstance = new T();
		}
		
		return pInstance;
	}
};

}

#endif