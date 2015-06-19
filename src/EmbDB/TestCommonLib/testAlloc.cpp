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

void testAlloc ()
{
	CommonLib::simple_alloc_t alloc;
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