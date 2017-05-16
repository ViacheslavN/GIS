#pragma once
#include "Commonlibrary/general.h"
#include "Commonlibrary/alloc_t.h"
 
namespace embDB
{

	template<typename T>
	class STLAllocator
	{
	public:
		//    typedefs
		typedef T value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;

	public:
		//    convert an allocator<T> to allocator<U>
		template<typename U>
		struct rebind {
			typedef STLAllocator<U> other;
		};

	public:
		inline explicit STLAllocator(CommonLib::alloc_t* pAlloc = nullptr) : m_pAlloc(pAlloc)
		{
			if (!m_pAlloc)
				m_pAlloc = &m_alloc;
		}
		inline ~STLAllocator()
		{

		}
		inline explicit STLAllocator(STLAllocator const& alloc)
		{
			if (alloc.IsExternalAlloc())
				m_pAlloc = const_cast<CommonLib::alloc_t*>(alloc.GetAlloc());
			else
				m_pAlloc = &m_alloc;
		}

		template<typename U>
		inline explicit STLAllocator(STLAllocator<U> const& alloc)
		{
			if (alloc.IsExternalAlloc())
				m_pAlloc = const_cast<CommonLib::alloc_t*>(alloc.GetAlloc());
			else
				m_pAlloc = &m_alloc;
		}

		//    address
		inline pointer address(reference r) { return &r; }
		inline const_pointer address(const_reference r) { return &r; }

		//    memory allocation
		inline pointer allocate(size_type cnt, typename std::allocator<void>::const_pointer = 0)
		{

			//new (m_pAlloc->alloc(cnt * sizeof(T))) TTreeNode(key, val);

			return reinterpret_cast<pointer>(m_pAlloc->alloc(cnt * sizeof(T)));
		}
		inline void deallocate(pointer p, size_type) {
			//::operator delete(p);
			m_pAlloc->free(p);
		}

		//    size
		inline size_type max_size() const 
		{
			//return size_t(-1) / sizeof(value_type);
			return (std::numeric_limits<size_t>::max)() / sizeof(value_type);
		}

		//    construction/destruction
		inline void construct(pointer p, const T& t)
		{
			new(p) T(t);
		}
		inline void destroy(pointer p) { p->~T(); }

		
		inline bool operator==(STLAllocator const& a) const
		{ 
			return true; 
		}
		inline bool operator!=(STLAllocator const& a) const
		{
			return !operator==(a); 
		}

		CommonLib::alloc_t* GetAlloc() { return m_pAlloc; }
		const CommonLib::alloc_t* GetAlloc() const { return m_pAlloc; }

		bool IsExternalAlloc() const { return m_pAlloc != &m_alloc; }
	private:

		CommonLib::alloc_t* m_pAlloc;
		CommonLib::simple_alloc_t m_alloc;

	};

}