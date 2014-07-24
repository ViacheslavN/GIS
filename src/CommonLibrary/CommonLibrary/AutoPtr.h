#ifndef AUTOPTR_H
#define AUTOPTR_H

#include "Nocopy.h"

template<typename T>
class AutoPtr
{
	typedef AutoPtr<T> this_type;
	mutable T * ptr;

	T * release()const
	{
		T * tmp = ptr;
		ptr = 0;
		return tmp;
	}
public:
	typedef T element_type;

	AutoPtr( T * ptr = 0 )
		:ptr( ptr )
	{}
	AutoPtr( const this_type & au )
		:ptr( au.release() )
	{}
	const this_type & operator=( const this_type & au )
	{
		if( this != &au )
		{
			delete ptr; ptr = 0;
			ptr = au.release();
		}
		return *this;
	}
	const this_type & operator=( T * p )
	{
		delete ptr;
		ptr = p;
		return *this;
	}
	~AutoPtr()
	{
		delete ptr; ptr = 0;
	}
	inline bool is_valid() const { return ptr!=0;}

	inline T * operator->()const
	{
		return ptr;
	}
	inline T & operator*()const
	{
		return *ptr;
	}
	inline operator T *()const
	{
		return ptr;
	}
};

template<typename T>
class AutoVecPtr : private Nocopy
{
public:
	AutoVecPtr( T * ptr = 0)
		:	m_ptr( ptr )
	{}

	~AutoVecPtr()
	{
		delete [] m_ptr; m_ptr = 0;
	}

	const AutoVecPtr<T> & operator = ( T * p )
	{
		delete [] m_ptr;
		m_ptr = p;
		return *this;
	}

	operator T *()const
	{
		return m_ptr;
	}
private:
	T * m_ptr;
};

#endif //AUTOPTR_H