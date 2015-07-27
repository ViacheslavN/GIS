#ifndef _LIB_COMMON_LIBRARY_STL_ALLOC_H_
#define _LIB_COMMON_LIBRARY_STL_ALLOC_H_
#include "alloc_t.h"

namespace CommonLib
{


template <typename T, typename TAlloc>
class stl_global_alloc
{
public:
	typedef T                 value_type;
	typedef value_type*       pointer;
	typedef value_type&       reference;
	typedef const value_type* const_pointer;
	typedef const value_type& const_reference;
	typedef ptrdiff_t         difference_type;
	typedef size_t            size_type;
public:
	template<class _Other>
	struct rebind
	{	// convert an allocator<_Ty> to an allocator <_Other>
		typedef stl_global_alloc<_Other, TAlloc> other;
	};
	pointer address(reference _Val) const
	{	// return address of mutable _Val
		return (&_Val);
	}
	const_pointer address(const_reference _Val) const
	{	// return address of nonmutable _Val
		return (&_Val);
	}
	stl_global_alloc() throw()
		: m_pAlloc(TAlloc::GetAllocator())
	{	// construct default allocator (do nothing)
	}
	stl_global_alloc(const stl_global_alloc<T, TAlloc>& a) throw()
		: m_pAlloc(a.alloc_)
	{	// construct by copying (do nothing)
	}
	template<class _Other>
	stl_global_alloc(const stl_global_alloc<_Other, TAlloc>& b) throw()
		: m_pAlloc(b.alloc_)
	{	// construct from a related allocator (do nothing)
	}
	template<class _Other>
	stl_global_alloc<T, TAlloc>& operator=(const stl_global_alloc<_Other, TAlloc>& b)
	{	// assign from a related allocator (do nothing)
		m_pAlloc = b.alloc_;
		return (*this);
	}
	void deallocate(pointer _Ptr, size_type)
	{	// deallocate object at _Ptr, ignore size
		m_pAlloc->free(_Ptr);
	}
	pointer allocate(size_type _Count)
	{	// allocate array of _Count elements
		return (pointer)m_pAlloc->alloc(sizeof(T) * _Count);
	}
	pointer allocate(size_type _Count, const void *)
	{	// allocate array of _Count elements, ignore hint
		return (allocate(_Count));
	}
	void construct(pointer _Ptr, const T& _Val)
	{	// construct object at _Ptr with value _Val
		::new (_Ptr) T(_Val);
	}
	void destroy(pointer _Ptr)
	{	// destroy object at _Ptr
		_Ptr; // disable warning
		_Ptr->~T();
	}
	size_t max_size() const throw()
	{	// estimate maximum array size
		size_t _Count = (size_t)(-1) / sizeof (T);
		return (0 < _Count ? _Count : 1);
	}
public:
	alloc_t* m_pAlloc;
};

// allocator TEMPLATE OPERATORS
template<class _Ty,
class _Other, class TAlloc> inline
	bool operator==(const stl_global_alloc<_Ty, TAlloc>& a, const stl_global_alloc<_Other, TAlloc>& b) throw()
{	// test for allocator equality (always true)
	return a.m_pAlloc == b.m_pAlloc;
}

template<class _Ty,
class _Other, class TAlloc> inline
	bool operator!=(const stl_global_alloc<_Ty, TAlloc>& a, const stl_global_alloc<_Other, TAlloc>& b) throw()
{	// test for allocator inequality (always false)
	return a.m_pAlloc != b.m_pAlloc;
}

}
#endif