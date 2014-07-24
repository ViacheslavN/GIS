#ifndef _EMBEDDED_DATABASE_TREES_SIMPLE_LIST_ITERATOR_
#define _EMBEDDED_DATABASE_TREES_SIMPLE_LIST_ITERATOR_

#include "GisCommonlibrary/general.h"
#include "GisCommonlibrary/alloc_t.h"
#include "simple_list_iterator.h"
namespace embDB
{
template<class Type>
class TListNode
{
public:

	TListNode(const Type& _val) : val_(_val), pNext_(0), pPrev_(0)
	{
	}
	TListNode *pNext_;
	TListNode *pPrev_;
	Type val_;
};

template <class TypeVal>
class TSimpleList;


template <class TypeKey>
class TSimpleListIterator
{
public:
	bool next();
	bool back();
	bool IsEnd();
protected:
	friend class  TSimpleList;

	TSimpleList* pList_;
	TListNode*   pNode_;

};
}
#endif //_EMBEDDED_DATABASE_TREES_SIMPLE_LIST_ITERATOR_