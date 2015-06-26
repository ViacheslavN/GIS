#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../EmbDB/BPVectorNoPod.h"
#include <vector>
#include <algorithm>
#include <set>
#include "CommonLibrary/DebugTime.h"
#include "../../EmbDB/CompositeIndex.h"
#include "../../EmbDB/VariantField.h"
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


void TestBPVector()
{
	

	//TestInsert();

	return;

	return;
	embDB::TBPVector<int32> vec1;

	vec1.push_back(0);
	vec1.push_back(1);
	vec1.push_back(2);
	vec1.push_back(2);
	vec1.push_back(2);
	vec1.push_back(5);
	vec1.push_back(7);
	vec1.push_back(7);
	short nType = 0;
	int index = vec1.upper_bound(-1, comp<int32>());
	vec1.insert(2, index);

	index = vec1.lower_bound(6, nType, comp<int32>());
	int indexmr = vec1.lower_bound(6, nType, comp<int32>());
	int index1 = vec1.binary_search(0, comp<int32>());
	vec1.insert(6, index);
	vec1.insert(6, index);
	vec1.insert(6, index);
	vec1.insert(6, index);
	vec1.insert(6, index);

	indexmr = vec1.lower_bound(6, nType, comp<int32>());
	index = vec1.upper_bound(6, comp<int32>());
	indexmr = vec1.upper_bound(8, comp<int32>());
	indexmr = vec1.lower_bound(8, nType, comp<int32>());
 
	for (size_t i = 0, sz = vec1.size(); i < sz; ++i)
	{
		std::cout << vec1[i] << std::endl;
	}


	std::vector<int> data;
	data.push_back(0);
	data.push_back(1);
	data.push_back(5);
	data.push_back(7);

	auto lower = std::lower_bound(data.begin(), data.end(), 4);
	auto upper = std::upper_bound(data.begin(), data.end(), 4);
 
 
	return;

	embDB::TBPVector<int32> vec;
	int32 nCount = 10;

	for (int32 i = 0; i < nCount; ++i )
	{
		vec.push_back(i);
	}
	for (int32 i = 0; i < nCount; ++i )
	{
		vec.insert(i, (size_t)i);
	}

	for (size_t i = 0, sz = vec.size(); i < sz; ++i)
	{
		std::cout << vec[i] << std::endl;
	}

	for (int32 i = 0; i < 1; ++i )
	{
		vec.remove(9);
	}
	std::cout << "remove" << std::endl;
	for (size_t i = 0, sz = vec.size(); i < sz; ++i)
	{
		std::cout << vec[i] << std::endl;
	}
}
