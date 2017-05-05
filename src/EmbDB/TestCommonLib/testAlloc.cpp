#include "stdafx.h"
#include <vector>
#include "CommonLibrary/alloc_t.h"


class IBase
{
public:
	IBase(){}
	virtual ~IBase()
	{
		int i = 0;
		i++;
	}
	virtual void funk() = 0;

};


class BaseImp : public IBase
{
public:
	BaseImp() : a(1), b(2), c(3)
	{}
	virtual ~BaseImp()
	{
		a = 0;
		b = 0;
		c = 0;

	}
	virtual void funk() {a += b +c;};
private:
	int a, b,c; 

};



template<typename T>
class Allocator 
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
		typedef Allocator<U> other;
	};

public:
	inline explicit Allocator(CommonLib::alloc_t* pAlloc = nullptr) : m_pAlloc(pAlloc)
	{
		if (!m_pAlloc)
			m_pAlloc = &m_alloc;
	}
	inline ~Allocator()
	{
	
	}
	inline explicit Allocator(Allocator const& alloc)
	{
		if (alloc.IsExternalAlloc())
			m_pAlloc = const_cast<CommonLib::alloc_t*>(alloc.GetAlloc());
		else
			m_pAlloc = &m_alloc;
	}

	template<typename U>
	inline explicit Allocator(Allocator<U> const& alloc)
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
	inline pointer allocate(size_type cnt,	typename std::allocator<void>::const_pointer = 0) 
	{

		//new (m_pAlloc->alloc(cnt * sizeof(T))) TTreeNode(key, val);

		return reinterpret_cast<pointer>(m_pAlloc->alloc(cnt * sizeof(T)));
	}
	inline void deallocate(pointer p, size_type) {
		//::operator delete(p);
		m_pAlloc->free(p);
	}

	//    size
	inline size_type max_size() const {
		return std::numeric_limits<size_type>::max() / sizeof(T);
	}

	//    construction/destruction
	inline void construct(pointer p, const T& t)
	{ 
		new(p) T(t); 
	}
	inline void destroy(pointer p) { p->~T(); }

	inline bool operator==(Allocator const&) { return true; }
	inline bool operator!=(Allocator const& a) { return !operator==(a); }

	CommonLib::alloc_t* GetAlloc() {return m_pAlloc;}
	const CommonLib::alloc_t* GetAlloc() const { return m_pAlloc; }

	bool IsExternalAlloc() const { return m_pAlloc != &m_alloc; }
private:

	CommonLib::alloc_t* m_pAlloc;
	CommonLib::simple_alloc_t m_alloc;

};    //    end of class Allocator 

typedef Allocator<int64> TAlloc;
typedef std::vector<int64, TAlloc> TVector;

void testAlloc ()
{
	CommonLib::simple_alloc_t alloc;
	int64 nSize = std::numeric_limits<int64>::max() / sizeof(__int64);
	TAlloc vecalloc(&alloc);

	int64 nSize1 = vecalloc.max_size();
	{
		TVector * pv = new TVector( TAlloc() );
		pv->push_back(1);
	 

		TVector v{ (TAlloc(&alloc)) };
		v.push_back(1);
		 
		/*v.push_back(1);
		v.push_back(2);
		v.push_back(3);
		v.push_back(4);
		v.push_back(5);*/

	}

	
	std::vector<IBase*> m_vecBase;


	for (size_t i = 0; i < 1000; ++i)
	{
		IBase* pIBase = new (alloc.alloc(sizeof(BaseImp))) BaseImp();
		m_vecBase.push_back(pIBase);
	}

	for (size_t i = 0, sz = m_vecBase.size(); i < sz; ++i)
	{
		m_vecBase[i]->~IBase();
		alloc.free(m_vecBase[i]);
	}

	 m_vecBase.clear();


};