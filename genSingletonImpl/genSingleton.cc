#include <iostream>
#include "genSingleton.h"

class MyClass
{
	private:
		MyClass();
		/*copy constructor and assignment operator should also be private */
	friend class SingleProvider<MyClass>;
};

MyClass::MyClass()
{

}


main()
{
	MyClass *ptr1 = SingleProvider<MyClass>::getInstance();
	MyClass *ptr2 = SingleProvider<MyClass>::getInstance();
	
	/* same pointer always */
	std::cout<<ptr1<<" "<<ptr2;
}
