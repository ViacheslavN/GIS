#ifndef _EMBEDDED_DATABASE_CACHE_LRU_2Q_H_
#define _EMBEDDED_DATABASE_CACHE_LRU_2Q_H_

#include "Commonlibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "List.h"
#include <map>
namespace embDB
{

	template<class TObj>
	struct TEmptyFreeChecker
	{
		bool IsFree(TObj* pObj)
		{
			return true;
		}
	};

	template<class TKey, class TObj, class TFreeChecker = TEmptyFreeChecker<TObj> >
	class TCacheLRU_2Q
	{

		enum eQueryType
		{
			UNDEFINED = 0,
			TOP = 1,
			BACK = 2
		};

		struct TCacheVal
		{
			TCacheVal() : m_type(UNDEFINED)
			{}
			TCacheVal( const TKey key, TObj* obj, eQueryType type) : m_key(key), m_obj(obj),
				m_type(type)
			{}
			TKey m_key;
			TObj* m_obj;
			eQueryType m_type;
		};


		typedef TList<TCacheVal> QList;

	 public:
		 typedef typename QList::iterator TListIterator;
		
		TCacheLRU_2Q(CommonLib::alloc_t* pAlloc) : 
			m_pAlloc(pAlloc), m_TopList(pAlloc), m_BackList(pAlloc)
		{
			if(!m_pAlloc)
				m_pAlloc = &m_simple_alloc;
		}
		~TCacheLRU_2Q()
		{
			clear();
		}


		void AddElem(const TKey& key, TObj* pObj, bool bAddBack = true)
		{
 			QList::iterator it;
			if(bAddBack)
				it = m_BackList.push_back(TCacheVal(key, pObj, BACK));
			else
				it = m_TopList.push_top(TCacheVal(key, pObj, TOP));
			m_CacheMap.insert(std::make_pair(key, it.node()));
		}

		uint32 size() const {return m_CacheMap.size();}
		void clear()
		{
	 		m_CacheMap.clear();
			m_BackList.clear();
			m_TopList.clear();

		}
		TObj*  remove(const TKey& key)
		{
			TCacheMap::iterator it = m_CacheMap.find(key);
			if(it == m_CacheMap.end())
				return NULL;

			QList::TNode* pNode =  it->second;
			TCacheVal & cacheVal = pNode->m_val;

			assert(cacheVal.m_type == BACK || cacheVal.m_type == TOP)

			if(cacheVal.m_type == BACK)
				m_BackList.remove(pNode);
			else
				m_TopList.remove(pNode);
		 

			m_CacheMap.erase(it);
			return cacheVal.m_obj;

		}
		TObj* remove_back()
		{

			TObj* pObj = NULL;
			if(m_BackList.size())
				pObj = removeBack(m_BackList)

            if(!pObj)
			{
				if(m_TopList.size())
					pObj = removeBack(m_TopList)
			}
			return pObj;
		}

		TObj* GetElem(const TKey& key, bool bNotMove = false)
		{
			TCacheMap::iterator it = m_CacheMap.find(key);
			if(it == m_CacheMap.end())
				return NULL;


			QList::TNode* pNode =  it->second;
			TCacheVal & cacheVal = pNode->m_val;
			if(bNotMove)
				return cacheVal.m_obj;

			if(cacheVal.m_type == BACK)
			{
				m_BackList.remove(pNode, false);
				cacheVal.m_type = TOP;
				m_TopList.push_back(pNode);
				 
			}
			else
			{
				if(!m_TopList.isTop(pNode))
				{
					m_TopList.remove(pNode, false);
					m_TopList.push_top(pNode);
				}
			}
			return cacheVal.m_obj;
		}
	
		TListIterator topList() {return m_TopList.begin();}
		TListIterator backList() {return m_BackList.begin();}
	 private:
		TObj* removeBack(QList& list)
		{
			QList::iterator listIt = list.back();
			if(listIt.IsNull())
				return NULL;


			while(!listIt.IsNull())
			{
				TObj* pObj = listIt.value();
				if(m_FreeChecker.IsFree(pObj))
				{
					m_CacheMap.erase(listIt.value().m_key);
					list.remove(listIt);
					return pObj;
				}
			}
			return NULL;
		}

	private:

		CommonLib::simple_alloc_t m_simple_alloc;
		typedef typename QList::TNode TListNode;
		typedef std::map<TKey, TListNode*> TCacheMap;

		CommonLib::alloc_t* m_pAlloc;
		TCacheMap m_CacheMap;
		QList m_TopList;
		QList m_BackList;
		TFreeChecker m_FreeChecker;
	 
	};   

}

#endif