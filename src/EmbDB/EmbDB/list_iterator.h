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

	TListNode(const Type& val) : m_val(val), m_pNext(0), m_pPrev(0)
	{
	}
	TListNode *m_pNext;
	TListNode *m_pPrev;
	Type m_val;
};

template <class TypeVal>
class TList;


template <class TypeKey>
class TListIterator
{
public:
	bool next();
	bool back();
	bool IsEnd();
protected:
	friend class  TList;

	TList* pList_;
	TListNode*   pNode_;

};
}
#endif //_EMBEDDED_DATABASE_TREES_SIMPLE_LIST_ITERATOR_