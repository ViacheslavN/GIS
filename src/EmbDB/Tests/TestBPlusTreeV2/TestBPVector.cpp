#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../EmbDB/BPVector.h"
#include <vector>
#include <algorithm>
template<class _Ty>
	struct comp
	{	
		bool LE(const _Ty& _Left, const _Ty& _Right){
			return (_Left < _Right);
		}
		bool EQ(const _Ty& _Left, const _Ty& _Right){
			return (_Left == _Right);
		}
		/*bool MR(const _Ty& _Left, const _Ty& _Right){
			return (_Left > _Right);
		}*/
	};

void TestBPVector()
{
	embDB::TBPVector<int32> vec1;

	vec1.push_back(0);
	vec1.push_back(1);
	vec1.push_back(5);
	vec1.push_back(7);
	short nType = 0;
	int index = vec1.lower_bound(6, nType, comp<int32>());
	int indexmr = vec1.upper_bound(6, nType, comp<int32>());
	int index1 = vec1.binary_search(5, comp<int32>());
	vec1.insert(6, index);
	vec1.insert(6, index);
	vec1.insert(6, index);
	vec1.insert(6, index);
	vec1.insert(6, index);

	indexmr = vec1.lower_bound(6, nType, comp<int32>());
	index = vec1.upper_bound(6, nType, comp<int32>());
	indexmr = vec1.upper_bound(8, nType, comp<int32>());
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