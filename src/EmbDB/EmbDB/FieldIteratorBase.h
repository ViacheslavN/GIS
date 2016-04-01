#ifndef _EMBEDDED_DATABASE_I_FIELD_ITERATOR_BASE_H_
#define _EMBEDDED_DATABASE_I_FIELD_ITERATOR_BASE_H_
#include "embDBInternal.h"
namespace embDB
{


	template<class TBTree, class I>
	class TFieldIteratorBase: public I
	{
	public:
		typedef typename TBTree::iterator  iterator;

		TFieldIteratorBase(iterator& it,  IValueField* pField) : m_ParentIt(it)
		{
			m_pField = pField;
		}
		TFieldIteratorBase() {}
		virtual ~TFieldIteratorBase(){}

		virtual bool isValid()
		{
			return !m_ParentIt.isNull();
		}
		virtual bool next() 
		{
			return m_ParentIt.next();
		}
		virtual bool back() 
		{
			return m_ParentIt.back();
		}
		virtual bool isNull()
		{
			return m_ParentIt.isNull();
		}
		virtual int64 getRowID()
		{
			return m_ParentIt.key();
		}


		virtual int64 addr() const
		{
			return m_ParentIt.addr();
		}
		virtual int32 pos() const
		{
			return m_ParentIt.pos();
		}

		virtual bool copy(IFieldIterator *pIter)
		{
			m_pField = pIter->getField();
			return m_ParentIt.setAddr(pIter->addr(), pIter->pos());
		}

		void set(iterator it,  IValueField* pField)
		{
			m_ParentIt = it;
			m_pField = pField;
		}
		void setParentIt(iterator it)
		{
			m_ParentIt = it;
		}
		virtual IValueFieldPtr getField() 
		{
			return m_pField;
		}
	public:
		iterator m_ParentIt;
		IValueFieldPtr m_pField;

	};

}

#endif