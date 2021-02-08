#include <SPL/Runtime/Utility/Singleton.h>

template <class T>
T* SPL::Singleton<T>::_instance = 0;

template <class T>
SPL::Singleton<T>::~Singleton() { _instance = 0; }
