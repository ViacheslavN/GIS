#ifndef _EMBEDDED_DATABASE_CACHE_NODES_H_
#define _EMBEDDED_DATABASE_CACHE_NODES_H_

#include "Commonlibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include <iostream>
#include "RBMap.h"
#include <set>
namespace embDB
{

	template<class TVal, class TObj>
	class TCacheNode
	{
	public:
		TCacheNode(const TVal& _val, TObj* _obj) : val_(_val), m_pNext(0), m_pPrev(0), obj_(_obj)
		{
		}
		TCacheNode *m_pNext;
		TCacheNode *m_pPrev;
		TVal val_;
		TObj *obj_;
	};

	template<class TKey, class TObj>
	class TSimpleCache
	{
	public:
		TSimpleCache(CommonLib::alloc_t* pAlloc) : 
		    m_pAlloc(pAlloc)
			, m_set(pAlloc)
			,m_pFirst(0)
			,m_pBack(0)
		{}
		~TSimpleCache()
		{
		
			clear();
		}


		struct SCacheElem
		{
			SCacheElem() : pListEl(0)
			{}
			TCacheNode<TKey, TObj> *pListEl;
		};

		void AddElem(const TKey& key, TObj* pObj, bool bAddBack = false)
		{
		
			TCacheEl* pElem =  new (m_pAlloc->alloc(sizeof(TCacheEl))) TCacheEl(key, pObj);
			SCacheElem el;
			el.pListEl = pElem;
			if(!m_set.insert(key, el))
			{
				assert(0);
				m_pAlloc->free(pElem);
				return;
			}
			if(bAddBack)
			{
				if(!m_pBack)
				{
					m_pFirst = pElem;
					m_pBack = pElem;
				}
				else
				{
					m_pBack->m_pNext = pElem;
					pElem->m_pPrev = m_pBack;
					m_pBack = pElem;
				}
				return;
			}
			if(!m_pFirst)
			{
				m_pFirst = pElem;
				m_pBack = pElem;
			}
			else
			{
				pElem->m_pNext = m_pFirst;
				m_pFirst->m_pPrev = pElem;
				m_pFirst = pElem;
			}
		}

		size_t size() const {return m_set.size();}
		void clear()
		{
			if(!m_pFirst)
				return;

			TCacheEl *pNode = m_pFirst;


			TCacheEl *pNextNode = pNode->m_pNext;
			if(!pNextNode)
			{
				m_pAlloc->free(pNode);
				m_pFirst = 0;
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

			m_pFirst = 0;
			m_pBack = 0;
			
		}
		TObj*  remove(const TKey& key)
		{
			TCacheSet::TTreeNode *pNode = m_set.findNode(key);
			TObj* pObj = NULL;
			if(m_set.isNull(pNode))
				return NULL;
			SCacheElem& el = pNode->m_val;
			TCacheEl* pListEl = el.pListEl;
			pObj = pListEl->obj_;
		
			
			if(m_pBack == m_pFirst)
			{
				m_pAlloc->free(m_pBack);
				m_pFirst = NULL;
				m_pBack = NULL;

			}
			else
			{
				if(pListEl == m_pBack)
					m_pBack = pListEl->m_pPrev;
				if(pListEl == m_pFirst)
					m_pFirst = pListEl->m_pNext;

				if(pListEl->m_pPrev)
				{
					pListEl->m_pPrev->m_pNext = pListEl->m_pNext;
				}
				if(pListEl->m_pNext)
				{
					pListEl->m_pNext->m_pPrev = pListEl->m_pPrev;
				}
				m_pAlloc->free(pListEl);
			}
			m_set.deleteNode(pNode, true, true);
			return pObj;

		}
		TObj* remove_back()
		{
			if(m_pBack)
			{
				TObj* pObj = NULL;
				TCacheEl* pListEl = m_pBack;
				TCacheSet::TTreeNode *pNode = NULL;
				while(pListEl)
				{
					if(pListEl->obj_->IsFree())
					{
						pObj = pListEl->obj_;
						m_set.remove(pListEl->val_);
						break;
					}
					pListEl = pListEl->m_pPrev;
				}
				if(!pObj)
					return pObj;

				if(m_pBack == m_pFirst)
				{
					m_pAlloc->free(m_pBack);
					m_pFirst = NULL;
					m_pBack = NULL;

				}
				else
				{
					if(pListEl == m_pBack)
						m_pBack = pListEl->m_pPrev;
					if(pListEl == m_pFirst)
						m_pFirst = pListEl->m_pNext;

					if(pListEl->m_pPrev)
					{
						pListEl->m_pPrev->m_pNext = pListEl->m_pNext;
					}
					if(pListEl->m_pNext)
					{
						pListEl->m_pNext->m_pPrev = pListEl->m_pPrev;
					}
					m_pAlloc->free(pListEl);
				}

				return pObj;
			}

			return NULL;
		}

		TObj* GetElem(const TKey& key, bool bNotMove = false)
		{
			TCacheSet::TTreeNode *pNode = m_set.findNode(key);
			if(m_set.isNull(pNode))
				return NULL;

			SCacheElem& el = pNode->m_val;
			if(bNotMove)
				return el.pListEl->obj_;
			if(el.pListEl != m_pFirst)
			{
				TCacheEl* pPrevEl = el.pListEl->m_pPrev;	
				TCacheEl* pNextEl = el.pListEl->m_pNext;	
				if(pPrevEl)
				{
					pPrevEl->m_pNext = pNextEl;
				}
				if(pNextEl)
				{
					pNextEl->m_pPrev = pPrevEl;
				}
				if(el.pListEl == m_pBack)
				{
					m_pBack = m_pBack->m_pPrev;
				}

				el.pListEl->m_pNext = m_pFirst;
				el.pListEl->m_pPrev = NULL;
				m_pFirst->m_pPrev = el.pListEl;
				m_pFirst = el.pListEl;
			}

			return el.pListEl->obj_;
		}


	public:
		typedef RBMap<TKey, SCacheElem> TCacheSet;
		typedef TCacheNode<TKey, TObj> TCacheEl;
		CommonLib::alloc_t* m_pAlloc;
		TCacheSet m_set;
		TCacheEl *m_pFirst;
		TCacheEl *m_pBack;
	};   

}

#endif