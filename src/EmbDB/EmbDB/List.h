#ifndef _EMBEDDED_DATABASE_NODE_LIST_
#define _EMBEDDED_DATABASE_NODE_LIST_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"

template<class TVal>
class TLNode
{
public:
	TLNode(const TVal& _val) : val_(_val), m_pNext(0), m_pPrev(0)
	{
	}
	TLNode *m_pNext;
	TLNode *m_pPrev;
	TVal val_;
};


template<class TValue>
class TList
{
public:
	typedef TLNode<TValue> TListElem;

	TList(CommonLib::alloc_t *pAlloc) :
		m_pFirst(0)
		,m_pLast(0)
		,m_nSize(0)
		,m_pAlloc(pAlloc)
	{}

   ~TList()
   {
	   if(!m_pFirst)
		   return;

	   TListElem *pNode = m_pFirst;


	   TListElem *pNextNode = pNode->m_pNext;
	   if(!pNextNode)
	   {
		   m_pAlloc->free(pNode);
		   return;
	   }
	   while(pNextNode)
	   {
		   m_pAlloc->free(pNode);
		   pNode = pNextNode;
		   pNextNode = pNextNode->m_pNext;
	   }
	   m_pAlloc->free(pNode);

	   m_nSize = 0;
	   m_pFirst = 0;
	   m_pLast = 0;

   }
	void push_top(const TValue& val){
		TListElem* pElem =  new (m_pAlloc->alloc(sizeof(TListElem))) TListElem(val);
		if(!m_pFirst){
			m_pFirst = pElem;
			m_pLast= pElem;
		}
		else{
			pElem->m_pNext = m_pFirst;
			m_pFirst->m_pPrev = pElem;
			m_pFirst = pElem;
		}
	}
	void push_back(const TValue& val){
		TListElem* pElem =  new (m_pAlloc->alloc(sizeof(TListElem))) TListElem(val);
		if(!m_pLast){
			m_pLast = pElem;
			m_pFirst= pElem;
		}
		else{
			pElem->m_pPrev = m_pLast;
			m_pLast->m_pNext = pElem;
			m_pLast = pElem;
		}
	}
	void remove(TListElem* pElement){
		if(pElement->m_pNext)
			pElement->m_pNext->m_pPrev = pElement->m_pPrev;
		else{
			//удаляем последний элемент
			m_pLast = pElement->m_pPrev;
		} 
		if(pElement->m_pPrev)
			pElement->m_pPrev->m_pNext = pElement->m_pNext;
		else{
			m_pFirst = pElement->m_pNext;
		}
		m_pAlloc->free(pElement);
	}

	void remove_back(){
		TListElem* pElement = m_pLast;
		m_pLast = pElement->m_pPrev;
		m_pLast->m_pNext = NULL;
		m_pAlloc->free(pElement);
		m_nSize--;
	}
	TValue& getTop(){
		return m_pFirst->val_;
	}
	TValue& getBack(){
		return m_pLast->val_;
	}
	size_t size() const{
		return m_nSize;
	}
	TListElem *m_pFirst;
	TListElem *m_pLast;
	size_t m_nSize;
	CommonLib::alloc_t *m_pAlloc;
};



#endif