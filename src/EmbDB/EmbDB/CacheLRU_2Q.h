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

		/*enum eQueryType
		{
			UNDEFINED = 0,
			TOP = 1,
			BACK = 2
		};*/

		struct TCacheVal
		{
			TCacheVal() : m_nCnt(0)/*m_type(UNDEFINED)*/
			{}
			TCacheVal( const TKey key, TObj* obj/*, eQueryType type*/) : m_key(key), m_obj(obj),
				m_nCnt(0)/*,	m_type(type)*/
			{}
			TKey m_key;
			TObj* m_obj;
			uint64 m_nCnt;
			//eQueryType m_type;
		};


		typedef TList<TCacheVal> QList;

	 public:
		 typedef typename QList::iterator TListIterator;
		
		TCacheLRU_2Q(CommonLib::alloc_t* pAlloc, int nK = 2) : 
			m_pAlloc(pAlloc), m_List(pAlloc), m_nK(nK)
		{
			if(!m_pAlloc)
				m_pAlloc = &m_simple_alloc;
		}
		~TCacheLRU_2Q()
		{
			clear();
		}


		class iterator
		{
			public:
				iterator(TListIterator listIt) : m_listIt(listIt)
				{}
				bool isNull() {return m_listIt.IsNull();}
				TObj* object(){return m_listIt.value().m_obj;}
				TKey  key(){return m_listIt.value().m_key;}
				bool next() {return m_listIt.next();}
			private:
				TListIterator m_listIt;
		};
 

		void AddElem(const TKey& key, TObj* pObj, bool bAddBack = true)
		{

			QList::iterator it = m_List.push_back(TCacheVal(key, pObj));
			m_CacheMap.insert(std::make_pair(key, it.node()));
		}

		uint32 size() const {return m_CacheMap.size();}
		void clear()
		{
	 		m_CacheMap.clear();
			m_List.clear();
		}
		TObj*  remove(const TKey& key)
		{
			TCacheMap::iterator it = m_CacheMap.find(key);
			if(it == m_CacheMap.end())
				return NULL;

			QList::TNode* pNode =  it->second;
			TCacheVal & cacheVal = pNode->m_val;
			TObj* pObj =  cacheVal.m_obj;
	
			m_List.remove(pNode);

			m_CacheMap.erase(it);
			return pObj;

		}
		TObj* remove_back()
		{
			return removeBack(m_List);
		}

		TObj* GetElem(const TKey& key, bool bNotMove = false)
		{
			TCacheMap::iterator it = m_CacheMap.find(key);
			if(it == m_CacheMap.end())
				return NULL;


			QList::TNode* pNode =  it->second;
			TCacheVal & cacheVal = pNode->m_val;


			cacheVal.m_nCnt++;

			if(bNotMove || cacheVal.m_nCnt < m_nK)
				return cacheVal.m_obj;


			if(!m_List.isTop(pNode))
			{
				m_List.remove(pNode, false);
				m_List.push_top(pNode);
			}
			return cacheVal.m_obj;
		}
	
		iterator begin() {return iterator(m_List.back());}
	 private:
		TObj* removeBack(QList& list)
		{
			QList::iterator listIt = list.back();
			if(listIt.IsNull())
				return NULL;


			while(!listIt.IsNull())
			{
				TObj* pObj = listIt.value().m_obj;
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
		QList m_List;
		TFreeChecker m_FreeChecker;
		uint32 m_nK;
	 
	};   

}

#endif