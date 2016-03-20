#ifndef _EMBEDDED_DATABASE_CACHE_LRU_H_
#define _EMBEDDED_DATABASE_CACHE_LRU_H_

#include "Commonlibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include <iostream>
#include <map>
namespace embDB
{


	template<class _TKey, class _TObj>
	class TCacheLRU
	{
		typedef _TKey TKey;
		typedef _TObj TObj;

		class TListNode
		{
		public:
			TListNode(const TKey& _key, TObj* _obj) : m_pNext(0), m_pPrev(0), obj_(_obj), key_(_key)
			{
			}
			TListNode *m_pNext;
			TListNode *m_pPrev;
			TObj *obj_;
			TKey key_;
		};


	public:

		class iterator
		{
			public:
				iterator(TListNode* pNode) : m_pNode(pNode)
				{}
				bool isNull()
				{
					return m_pNode == NULL;
				}
				void next()
				{
					assert(m_pNode);
					m_pNode = m_pNode->m_pNext;
				}
				void back()
				{
					assert(m_pNode);
					m_pNode = m_pNode->m_pPrev;
				}
				TKey& key(){return m_pNode->key_;}
				TObj* object(){return m_pNode->obj_;}

				const TKey& key() const {return m_pNode->key_;}
				const TObj* object() const {return m_pNode->obj_;}

			private:
				TListNode* m_pNode;
		};

		TCacheLRU(CommonLib::alloc_t* pAlloc) : 
		  m_pAlloc(pAlloc)
			  ,m_pFirst(0)
			  ,m_pBack(0)
		  {
			  if(!m_pAlloc)
				  m_pAlloc = &m_simple_alloc;
		  }
		  ~TCacheLRU()
		  {
			  clear();
		  }



		  void AddElem(const TKey& key, TObj* pObj, bool bAddBack = false)
		  {

			  TListNode* pElem =  new (m_pAlloc->alloc(sizeof(TListNode))) TListNode(key, pObj);
			  m_CacheMap.insert(std::make_pair(key, pElem));
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

		  uint32 size() const {return m_CacheMap.size();}
		  void clear()
		  {
			  if(!m_pFirst)
				  return;

			  TListNode *pNode = m_pFirst;


			  TListNode *pNextNode = pNode->m_pNext;
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

			  m_CacheMap.clear();

		  }
		  TObj*  remove(const TKey& key)
		  {
			 typename TCacheMap::iterator it = m_CacheMap.find(key);
			  if(it == m_CacheMap.end())
				  return NULL;

			  TObj*  pObj = it->second->obj_;
			  if(m_pBack == m_pFirst)
			  {
				  m_pAlloc->free(m_pBack);
				  m_pFirst = NULL;
				  m_pBack = NULL;

			  }
			  else
			  {
				  TListNode* pListEl = it->second;
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
			  m_CacheMap.erase(it);
			  return pObj;

		  }
		  TObj* remove_back()
		  {
			  if(m_pBack)
			  {
				  TObj* pObj = NULL;
				  TListNode* pListEl = m_pBack;
				  while(pListEl)
				  {
					  pObj = pListEl->obj_;
					  if(pObj->IsFree())
					  {

						  m_CacheMap.erase(pListEl->key_);
						  break;
					  }
					  pListEl = pListEl->m_pPrev;
					  pObj = NULL;
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
			  typename TCacheMap::iterator it = m_CacheMap.find(key);
			  if(it == m_CacheMap.end())
				  return NULL;

			  TObj*  pObj = it->second->obj_;
			  if(bNotMove)
				  return pObj;
			  TListNode* el = it->second;
			  if(el != m_pFirst)
			  {
				  TListNode* pPrevEl = el->m_pPrev;	
				  TListNode* pNextEl = el->m_pNext;	
				  if(pPrevEl)
				  {
					  pPrevEl->m_pNext = pNextEl;
				  }
				  if(pNextEl)
				  {
					  pNextEl->m_pPrev = pPrevEl;
				  }
				  if(el == m_pBack)
				  {
					  m_pBack = m_pBack->m_pPrev;
				  }

				  el->m_pNext = m_pFirst;
				  el->m_pPrev = NULL;
				  m_pFirst->m_pPrev = el;
				  m_pFirst = el;
			  }

			  return pObj;
		  }

		  iterator back() {return iterator(m_pBack);}
		  iterator begin() {return iterator(m_pFirst);}
	public:

		CommonLib::simple_alloc_t m_simple_alloc;
		typedef std::map<TKey, TListNode*> TCacheMap;

		CommonLib::alloc_t* m_pAlloc;
		TCacheMap m_CacheMap;
		TListNode *m_pFirst;
		TListNode *m_pBack;
	};   

}

#endif