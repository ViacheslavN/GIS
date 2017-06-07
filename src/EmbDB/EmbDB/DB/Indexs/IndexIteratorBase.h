#ifndef _EMBEDDED_DATABASE_I_INDEX_ITERATOR_BASE_H_
#define _EMBEDDED_DATABASE_I_INDEX_ITERATOR_BASE_H_
#include "../../embDBInternal.h"
namespace embDB
{


	template<class TIterator, class I>
	class TIndexIteratorBase: public I
	{
	public:
		typedef TIterator  iterator;

		TIndexIteratorBase(iterator& it, IndexFiled *pIndex) : m_ParentIt(it)
		{
			m_pIndex = pIndex;
		}
		virtual ~TIndexIteratorBase(){}

		virtual bool next()
		{
			return m_ParentIt.next();
		}
		virtual bool isNull()
		{
			return m_ParentIt.isNull();
		}
		virtual bool isValid()
		{
			return !m_ParentIt.isNull();
		}
	/*	virtual bool getKey(CommonLib::CVariant* pVal)
		{
			pVal->setVal(m_ParentIt.key());
			return true;
		}
		virtual uint64 getRowID()
		{
			return m_ParentIt.value();
		}*/
		virtual int64 addr() const
		{
			return m_ParentIt.addr();
		}
		virtual int32 pos() const
		{
			return m_ParentIt.pos();
		}
		virtual bool copy(IIndexIterator *pIter)
		{
			m_pIndex = pIter->GetIndex();
			return m_ParentIt.setAddr(pIter->addr(), pIter->pos());
		}
		void set(iterator it)
		{
			m_ParentIt = it;
		}
		virtual IndexFiledPtr GetIndex()
		{
			return m_pIndex;
		}
	public:
		iterator m_ParentIt;
		IndexFiledPtr m_pIndex;


	};

}

#endif