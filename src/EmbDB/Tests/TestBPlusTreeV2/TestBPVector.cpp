#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../EmbDB/BPVectorNoPod.h"
#include <vector>
#include <algorithm>
#include <set>
#include "CommonLibrary/DebugTime.h"
#include "../../EmbDB/CompositeIndexKey.h"
#include "../../EmbDB/StringVal.h"
//#include "../../EmbDB/VariantField.h"
template<class _Ty>
	struct comp
	{	
		bool LE(const _Ty& _Left, const _Ty& _Right) const{
			return (_Left < _Right);
		}
		bool EQ(const _Ty& _Left, const _Ty& _Right) const{
			return (_Left == _Right);
		}
		/*bool MR(const _Ty& _Left, const _Ty& _Right){
			return (_Left > _Right);
		}*/
	};


void ShowVector(embDB::TBPVector<int32>& vec)
{
	for (size_t i = 0, sz = vec.size(); i < sz; ++i)
	{
		std::cout << vec[i] << ", ";
	}

	std::cout << std::endl;
}

void TestSearch()
{
	embDB::TBPVector<int64> vec;
	for (int i = 0; i < 100000; ++i)
	{
		vec.push_back(i);
	}


	double tmUpperBound = 0;
	double tmLowerBound = 0;
	CommonLib::TimeUtils::CDebugTime time;
	time.start();
	for (int i =0; i < 100000; ++i)
	{
		
		for (int64 j = 0; j < 100000; ++j)
		{
			vec.upper_bound(j, comp<int64>());
		}
	}
	tmUpperBound = time.stop();
	time.start();
	short nType = 0;

	for (int i =0; i < 100000; ++i)
	{

		for (int64 j = 0; j < 100000; ++j)
		{
			vec.lower_bound(j, nType, comp<int64>());
		
		}
	}
	tmLowerBound = time.stop();

	std::cout << "Upper bound time: " << tmUpperBound << " Lower bound time: " << tmLowerBound << std::endl;
}

void TestInsert()
{
	embDB::TBPVector<int32> vec;
	embDB::TBPVector<int32> vec1;
	embDB::TBPVector<int32> vec3;
	size_t n = 20;
	for (size_t i = 0; i < n; ++i)
	{
		vec.push_back(i);
		vec1.push_back(i);
		vec3.push_back(i);
	}
	ShowVector(vec);
	std::cout << "movel 2 2  " << std::endl;
	vec.movel(2, 2);
	ShowVector(vec);

	std::cout << "mover 0 3 " << std::endl;
	vec1.mover(0, 3);
	ShowVector(vec1);

	std::cout << "mover 3  5 " << std::endl;
	vec3.mover(3, 5);
	ShowVector(vec3);
}

/*
m_innerKeyMemSet.mover(0, nCnt );
m_innerLinkMemSet.mover(0, nCnt);

m_innerKeyMemSet[nCnt - 1] = LessMin;
m_innerLinkMemSet[nCnt - 1] = m_nLess;

size_t newSize = pNode->m_innerLinkMemSet.size() -nCnt;


m_innerKeyMemSet.copy(pNode->m_innerKeyMemSet, 0, newSize + 1, pNode->m_innerKeyMemSet.size());
m_innerLinkMemSet.copy(pNode->m_innerLinkMemSet, 0, newSize + 1, pNode->m_innerLinkMemSet.size());

*/
template<class TVector>
bool UnionVec(TVector& dstVec, TVector& srcVec, bool bLeft, int *nCheckIndex = 0 )
{
	if(bLeft)
	{
		if(nCheckIndex)
			*nCheckIndex += srcVec.size();
		srcVec.push_back(dstVec);
		srcVec.swap(dstVec);
	}
	else
	{
		dstVec.push_back(srcVec);
	}
	return true;
}

typedef embDB::TBPVector<embDB::sFixedStringVal> TFxStringVec;

void TestBPVector()
{
	

	TFxStringVec vec1;
	TFxStringVec vec2;
	TFxStringVec vec3;
	CommonLib::simple_alloc_t alloc;
	CommonLib::CString sString;

	for (__int64 i = 0; i < 127; ++i)
	{
		sString.format(L"Cтрока_Строка_Строка__%I64d", i);

	
		embDB::sFixedStringVal val;
		uint32 nUft8Len = sString.calcUTF8Length()  + 1;

		val.m_pBuf = (byte*)alloc.alloc(nUft8Len);
		sString.exportToUTF8((char*)val.m_pBuf, nUft8Len);
		vec1.push_back(val);

	}

	for (__int64 i = 127; i < 500; ++i)
	{
		sString.format(L"Cтрока_Строка_Строка__%I64d", i);


		embDB::sFixedStringVal val;
		uint32 nUft8Len = sString.calcUTF8Length()  + 1;

		val.m_pBuf = (byte*)alloc.alloc(nUft8Len);
		sString.exportToUTF8((char*)val.m_pBuf, nUft8Len);
		vec2.push_back(val);

	}
	UnionVec(vec1,vec2, false);

	for (__int64 i = 0; i < 120; ++i)
	{
		sString.format(L"Cтрока_Строка_Строка__%I64d", i);


		embDB::sFixedStringVal val;
		uint32 nUft8Len = sString.calcUTF8Length()  + 1;

		val.m_pBuf = (byte*)alloc.alloc(nUft8Len);
		sString.exportToUTF8((char*)val.m_pBuf, nUft8Len);
		vec3.push_back(val);

	}

	std::map<int, int> mapInt;
	mapInt[0] = 15;
	//TestInsert();
 
}
