#ifndef _EMBEDDED_DATABASE_CACHE_LRU_2Q_H_
#define _EMBEDDED_DATABASE_CACHE_LRU_2Q_H_

#include "Commonlibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "List.h"
#include <map>
namespace embDB
{


	template<class TKey, class TObj>
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
			TCacheVal( const TKey key, TObj obj, eQueryType type) : m_key(key), m_obj(obj),
				m_type(type)
			{}
			TKey m_key;
			TObj m_obj;
			eQueryType m_type;
		};


		typedef TList<TCacheVal> QList;

	 public:

		 class iterator
		 {
			public:
				iterator(): m_bNull(false)
				{}
				iterator(TObj obj): obj(m_obj), m_bNull(true)
				{}
				bool IsNull() const {return m_bNull;}
				const TObj& value() const{return m_obj;}
				TObj& value() {return m_obj;}
			private:
				TObj m_obj
				bool m_bNull;


		 };

		 typedef QList::iterator TListIterator;
		
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


		void AddElem(const TKey& key, TObj pObj, bool bAddBack = false)
		{
 			QList::iterator it;
			if(bAddBack)
				it = m_BackList.push_back(TCacheVal(key, pObj, BACK));
			else
				it = m_BackList.puch_top(TCacheVal(key, pObj, BACK));
			m_CacheMap.insert(std::make_pair(key, it.node()));
		}

		size_t size() const {return m_CacheMap.size();}
		void clear()
		{
	 		m_CacheMap.clear();
			m_BackList.clear();
			m_TopList.clear();

		}
		iterator  remove(const TKey& key)
		{
			TCacheMap::iterator it = m_CacheMap.find(key);
			if(it == m_CacheMap.end())
				return iterator();

			QList::TNode* pNode =  it->second;
			TCacheVal & cacheVal = pNode->m_val;
			iterator it(cacheVal.m_obj);

			if(cacheVal.m_type == BACK)
				m_BackList.remove(pNode);
			else
				m_TopList.remove(pNode);
			else
				assert(false);

			m_CacheMap.erase(it);
			return pObj;

		}
		iterator remove_back()
		{

			if(m_BackList.size())
				 return removeBack(m_BackList)
			if(m_TopList.size())
				 return removeBack(m_BackList)
			return iterator();
		}

		iterator GetElem(const TKey& key, bool bNotMove = false)
		{
			TCacheMap::iterator it = m_CacheMap.find(key);
			if(it == m_CacheMap.end())
				return iterator();


			QList::TNode* pNode =  it->second;
			TCacheVal & cacheVal = pNode->m_val;
			if(bNotMove)
				iterator(cacheVal.m_obj);

			if(cacheVal.m_type == BACK)
			{
				m_BackList.remove(pNode, false);
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
			return iterator(cacheVal.m_obj);
		}
	
		TListIterator topList() {return m_TopList.begin();}
		TListIterator backList() {return m_BackList.begin();}
	 private:
		iterator removeBack(QList& list)
		{
			QList::iterator listIt = list.back();
			if(listIt.iterator)
				return iterator();

			iterator it(listIt.value());
			m_CacheMap.erase(listIt.value().m_key);
			list.remove(listIt);

			return it;
		}

	private:

		CommonLib::simple_alloc_t m_simple_alloc;
		typedef std::map<TKey, QList::TNode*> TCacheMap;

		CommonLib::alloc_t* m_pAlloc;
		TCacheMap m_CacheMap;
		QList m_TopList;
		QList m_BackList;
	 
	};   

}

#endif