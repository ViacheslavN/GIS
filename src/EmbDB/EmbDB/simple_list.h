#ifndef _EMBEDDED_DATABASE_TREES_SIMPLE_LIST_
#define _EMBEDDED_DATABASE_TREES_SIMPLE_LIST_

#include "GisCommonlibrary/general.h"
#include "GisCommonlibrary/alloc_t.h"
#include "simple_list_iterator.h"
namespace embDB
{

template <class TypeVal>
class TSimpleList
{
public:
	typedef TListNode<TypeVal> _Node;
	typedef TSimpleListIterator<TypeVal> iterator;

	TSimpleList(alloc_t* alloc);
	~TSimpleList();

	void push_back(const TypeVal& val);
	void puch_top(const TypeVal& val);
	void sort();
	size_t size() const;
	void clear();
	iterator begin();
	iterator last();
	iterator remove(const iterator& it);
protected:
	void DestroyList();
protected:
	_Node pFirst_;
	_Node pLast_;
	 alloc_t* alloc_;
};


 #include "RBTree.cpp"
#endif