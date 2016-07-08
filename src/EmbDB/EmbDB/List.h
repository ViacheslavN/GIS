#ifndef _EMBEDDED_DATABASE_LIST_H_
#define _EMBEDDED_DATABASE_LIST_H_

#include "Commonlibrary/general.h"
#include "Commonlibrary/alloc_t.h"
 
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
		iterator(TNode*  pNode = NULL) : m_pNode(pNode)
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
		bool IsNull()
		{
			return m_pNode == NULL;
		}

		TypeVal & value(){return m_pNode->m_val;}
		const TypeVal & value() const{return m_pNode->m_val;}
		TNode*  node() {return m_pNode;}
		const TNode* node() const {return m_pNode;}

		friend class TList;
	protected: 
		TNode*   m_pNode;
	};
	TList(CommonLib::alloc_t* pAlloc) : m_pAlloc(pAlloc), m_nSize(0), m_pBack(NULL), m_pBegin(NULL)
	{
		if(!m_pAlloc)
		{
			m_pAlloc = &m_simple_alloc;
		}

	}
	~TList()
	{
		clear();
	}

	iterator push_back(const TypeVal& val)
	{
		 TNode* pElem =  new (m_pAlloc->alloc(sizeof(TNode))) TNode(val);
		 return push_back(pElem);
	}
	iterator push_top(const TypeVal& val)
	{
		TNode* pElem =  new (m_pAlloc->alloc(sizeof(TNode))) TNode(val);
		return push_top(pElem);
	}


	iterator push_back(TNode* pElem)
	{
		if(m_pBack == pElem)
			return iterator(pElem);

		if(m_pBack == NULL)
			m_pBack = m_pBegin = pElem;
		else
		{
			pElem->m_pNext = m_pBack;
			pElem->m_pPrev = NULL;
			m_pBack->m_pPrev = pElem;
			m_pBack = pElem;
		}

		m_nSize++;
		return iterator(pElem);
	}
	iterator push_top(TNode* pElem)
	{

		if(m_pBegin == pElem)
			return iterator(pElem);

		if(m_pBegin == NULL)
			m_pBack = m_pBegin = pElem;
		else
		{
			m_pBegin->m_pNext = pElem;
			pElem->m_pPrev = m_pBegin;
			pElem->m_pNext = NULL;
			m_pBegin = pElem;
		}

		m_nSize++;
		return iterator(pElem);
	}

	uint32 size() const{return m_nSize;}
	void clear()
	{

		if(m_pBegin == NULL|| m_pBack == NULL)
			return;

		TNode *pNode = m_pBack;
		TNode *pNextNode = pNode->m_pNext;
		if(!pNextNode)
		{
			m_pAlloc->free(pNode);
			m_pBegin = 0;
			m_pBack = 0;
			return;
		}
		while(pNextNode)
		{
			m_pAlloc->free(pNode);
			pNode = pNextNode;
			pNextNode = pNextNode->m_pNext;
		}
		m_pAlloc->free(pNode);

		m_pBegin = 0;
		m_pBack = 0;
		m_nSize = 0;
	}
	iterator begin() {return iterator(m_pBegin);}
	iterator back(){return iterator(m_pBack);}
	iterator insert(const iterator& it, const TypeVal& val)
	{
		TNode* pNode = it.m_pNode;
		if(!pNode)
			return iterator(NULL);

		TNode* pElem =  new (m_pAlloc->alloc(sizeof(TNode))) TNode(val);

		pElem->m_pNext = pNode->m_pNext;
		pElem->m_pPrev = pNode;
		if(pNode->m_pNext)
		{
			pNode->m_pNext->m_pPrev = pElem;
		}
		else
		{
			m_pBegin = pElem;
			pNode->m_pNext = pElem;
		}
		m_nSize++;
		return iterator(pElem);
	}
	iterator remove(TNode* pNode, bool bDel = true)
	{		
		if(!pNode)
			return iterator(NULL);

		TNode *pPrev = pNode->m_pPrev;
		TNode *pNext = pNode->m_pNext;

		if(pNext)
			pNext->m_pPrev = pPrev;
		if(pPrev)
			pPrev->m_pNext = pNext;

		if(!pNext)
			m_pBegin = pPrev;
		if(!pPrev)
			m_pBack = pNext;

		if(bDel)
			m_pAlloc->free(pNode);
		m_nSize--;
		return iterator(pNext);
	}
	iterator remove(const iterator& it, bool bDel = true)
	{
		TNode* pNode = it.m_pNode;
		return remove(pNode, bDel);
	}
	 bool isTop(const TNode *pNode) const
	 {
		 return m_pBegin == pNode;
	 }
protected:
	TNode* m_pBegin;
	TNode* m_pBack;
	CommonLib::alloc_t* m_pAlloc;
	CommonLib::simple_alloc_t m_simple_alloc;
	uint32 m_nSize;


};

}
#endif