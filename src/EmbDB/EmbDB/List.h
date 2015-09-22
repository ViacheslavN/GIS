#ifndef _EMBEDDED_DATABASE_LIST_
#define _EMBEDDED_DATABASE_LIST_

#include "GisCommonlibrary/general.h"
#include "GisCommonlibrary/alloc_t.h"
 
namespace embDB
{

template <class TypeVal>
class TList
{
public:

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

	typedef TListNode<TypeVal> TNode;

	class iterator
	{
	public:
		iterator(TNode*  pNode) : m_pNode(pNode)
		{}
		bool next()
		{
			m_pNode = m_pNode->m_pNext;
			return m_pNode != NULL;
		}
		bool back()
		{
			m_pNode = m_pNode->m_pPrev;
			return m_pNode != NULL;
		}
		bool IsEnd()
		{
			return m_pNode == NULL;
		}

		TypeVal & value(){return m_pNode->m_val;}
		const TypeVal & value() const{return m_pNode->m_val;}
		TNode*  node() {return m_pNode};
		const TNode* node() const {return m_pNode};
	protected: 
		TNode*   m_pNode;
	};
	TList(alloc_t* pAlloc) : m_pAlloc(pAlloc)
	{}
	~TList();

	TNode* push_back(const TypeVal& val);
	TNode* puch_top(const TypeVal& val);
	size_t size() const;
	void clear();
	iterator begin() {return iterator(m_pFirst);}
	iterator last(){return iterator(m_pLast)}
	iterator insert(const iterator& it);
	iterator remove(const iterator& it);
protected:
	void DestroyList();
protected:
	TNode m_pFirst;
	TNode m_pLast;
	 alloc_t* m_pAlloc;
};

#endif