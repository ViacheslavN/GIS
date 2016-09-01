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
		bool IsFree(TObj& pObj)
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
			TCacheVal() : /*m_nCnt(0)*/m_type(UNDEFINED)
			{}
			TCacheVal( const TKey key, TObj obj, eQueryType type) : m_key(key), m_obj(obj),
				/*m_nCnt(0)*/	m_type(type)
			{}

			~TCacheVal()
			{

			}
			TKey m_key;
			TObj m_obj;
			//uint64 m_nCnt;
			eQueryType m_type;
		};


		typedef TList<TCacheVal> QList;

	 public:
		 typedef typename QList::iterator TListIterator;
		
		TCacheLRU_2Q(CommonLib::alloc_t* pAlloc, TObj nullObj = TObj()) : 
			m_pAlloc(pAlloc), m_TopList(pAlloc), m_BackList(pAlloc),
				m_NullObj(nullObj)
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
				iterator(QList *pTopList, QList *pBackList) : m_pTopList(pTopList), m_pBackList(pBackList)
				{

					assert(pTopList != NULL);
					assert(pBackList != NULL);

					m_type = BACK;
					m_listIt = m_pBackList->back();
					if(m_listIt.IsNull())
					{
						m_type = TOP;
						m_listIt = m_pTopList->back();
					}

				}
				bool isNull()
				{
					return m_listIt.IsNull();
				}
				TObj& object(){return m_listIt.value().m_obj;}
				TKey&  key(){return m_listIt.value().m_key;}
				bool next()
				{
					if( m_listIt.IsNull())
					{
						if(m_type == TOP)
							return false;
						m_type = TOP;
						m_listIt = m_pTopList->back();
						return m_listIt.IsNull();

					}

					return m_listIt.next();
				}
			private:
				eQueryType m_type;
				QList *m_pTopList;
				QList *m_pBackList;
				TListIterator m_listIt;
		};
 

		void AddElem(const TKey& key, TObj& pObj, bool bAddBack = true)
		{

			QList::iterator it;
			if(bAddBack)
				it =  m_BackList.push_back(TCacheVal(key, pObj, BACK));
			else
				it =  m_TopList.push_back(TCacheVal(key, pObj, TOP));
			m_CacheMap.insert(std::make_pair(key, it.node()));
		}

		uint32 size() const {return (uint32)m_CacheMap.size();}
		void clear()
		{
	 		m_CacheMap.clear();
			m_TopList.clear();
			m_BackList.clear();
		}
		TObj remove(const TKey& key)
		{
			TCacheMap::iterator it = m_CacheMap.find(key);
			if(it == m_CacheMap.end())
				return m_NullObj;

			QList::TNode* pNode =  it->second;
			TCacheVal & cacheVal = pNode->m_val;
			TObj pObj =  cacheVal.m_obj;

			if(cacheVal.m_type == BACK)
				m_BackList.remove(pNode);
			else
				m_TopList.remove(pNode);

			m_CacheMap.erase(it);
			return pObj;

		}
		TObj remove_back()
		{
			TObj pObj = removeBack(m_BackList);
			if(pObj == m_NullObj)
				 pObj = removeBack(m_TopList);

			return pObj;
		}

		TObj& GetElem(const TKey& key, bool bNotMove = false)
		{
			TCacheMap::iterator it = m_CacheMap.find(key);
			if(it == m_CacheMap.end())
				return m_NullObj;


			QList::TNode* pNode =  it->second;
			TCacheVal & cacheVal = pNode->m_val;

			 

			if(bNotMove)
				return cacheVal.m_obj;


			if(cacheVal.m_type == BACK)
			{

				if(!m_BackList.isTop(pNode))
				{
					m_BackList.remove(pNode, false);
					m_BackList.push_top(pNode);
				}
				else
				{
					m_BackList.remove(pNode, false);
					cacheVal.m_type = TOP;
					m_TopList.push_back(pNode);
				}
				
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
	
		iterator begin() {return iterator(&m_TopList, &m_BackList);}
	 private:
		TObj removeBack(QList& list)
		{
			QList::iterator listIt = list.back();
			if(listIt.IsNull())
				return m_NullObj;


			while(!listIt.IsNull())
			{
				
				if(m_FreeChecker.IsFree(listIt.value().m_obj))
				{
					TObj pObj = listIt.value().m_obj;
					m_CacheMap.erase(listIt.value().m_key);
					list.remove(listIt);
					return pObj;
				}
				else
				{
					listIt.next();
				}
			}
			return m_NullObj;
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
		uint32 m_nK;

		TObj m_NullObj;
	 
	};   

}

#endif