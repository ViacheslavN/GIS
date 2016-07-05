#include "stdafx.h"
#include "../../EmbDB/CacheLRU_2Q.h"


class CObj
{
public:
	   CObj(int nID) : m_nID(nID)
	   {}
	   ~CObj(){}

	   int GetID() const {return m_nID;}
private:
	int m_nID;
};


typedef embDB::TCacheLRU_2Q<__int64, CObj> TCache;

void TestCacheLRU_2Q()
{
	CommonLib::simple_alloc_t alloc;
	{
		TCache cache(&alloc);

		for (int i = 0, sz = 100; i < sz; i++)
		{
			CObj *pObj = new CObj(i);
			cache.AddElem(pObj->GetID(), pObj);
		}

		for (size_t i = 0, sz = 100; i < sz; i++)
		{
		 
		
			CObj *pObj =  cache.GetElem(i);
			
		}
	}
	
}