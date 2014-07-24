#include "stdafx.h"

/*
void TestMultiMap(){
	CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
	embDB::RBMultiMap<int64, int64> map(alloc);
	for(int64 i = 1000; i > 0; --i){
		for(int64 i = 1000; i > 0; --i)
		{
			map.insert(i, i);
		}
	}
	for(int64 i = 1000; i > 0; --i){
		for(int64 i = 1000; i > 0; --i)
		{
			map.remove(i);
		}
	}

	for(int64 i = 1000; i > 0; --i){
		for(int64 i = 1000; i > 0; --i)
		{
			map.remove(i);
		}
	}
	embDB::RBMultiMap<int64, int64>::iterator it = map.begin();
	assert(!!it.IsNull());
	for(int64 i = 1000; i > 0; --i){
		for(int64 i = 1000; i > 0; --i)
		{
			map.insert(i, i);
		}
	}
	it = map.begin();
	while(!it.IsNull()){
		int64 tt = it.key();
		int64 val = it.value();
		tt = *it;
		it++;
	}
	it = map.last();
	while(!it.IsNull()){
		int64 tt = it.key();
		int64 val = it.value();
		tt = *it;
		it--;
	}
	it = map.begin();
	while(!it.IsNull()){
		int64 tt = it.key();
		int64 val = it.value();
		tt = *it;
		it = map.remove(it);
	}

}*/