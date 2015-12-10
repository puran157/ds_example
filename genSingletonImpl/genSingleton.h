#include <iostream>

template <class T>
class SingleProvider
{
	public:
		static T* getInstance()
		{
			static T *instance = NULL;
			if(instance == NULL)
				instance = new T();
			return instance;
		}
};
