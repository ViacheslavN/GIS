#ifndef _EMBEDDED_DATABASE_I_DB_FIELD_H_
#define _EMBEDDED_DATABASE_I_DB_FIELD_H_

#include "IField.h"
#include "CommonLibrary/Variant.h"
namespace embDB
{

	class IFieldIterator : public CommonLib::AutoRefCounter
	{
	public:
		IFieldIterator(){};
		virtual ~IFieldIterator(){}
		virtual bool isValid() = 0;
		virtual bool next() = 0;
		virtual bool back() = 0;
		virtual bool isNull() = 0;
		virtual bool getVal(/*IVariant**/CommonLib::CVariant* pVal) = 0;
		virtual uint64 getRowID() = 0;

		virtual int64 addr() const = 0;
		virtual int32 pos() const = 0;

		virtual bool copy(IFieldIterator *pIter) = 0;
	};

	typedef CommonLib::IRefCntPtr<IFieldIterator> FieldIteratorPtr;

	template<class TKeyType>
	class TIIndexIterator : public CommonLib::RefCounter
	{
	public:
		TIIndexIterator(){};
		virtual ~TIIndexIterator(){};
		virtual bool isValid() = 0;
		virtual bool next() = 0;
		virtual bool isNull() = 0;
		virtual bool getKey(TKeyType* pIndexKey) = 0;
		virtual uint64 getRowID() = 0;


		virtual int64 addr() const = 0;
		virtual int32 pos() const = 0;

		virtual bool copy(TIIndexIterator *pIter) = 0;
	};

	typedef TIIndexIterator<IVariant> IIndexIterator;

	typedef CommonLib::IRefCntPtr<IIndexIterator> IndexIteratorPtr;

	class IIndexPageIterator  : public CommonLib::RefCounter
	{
	public:
		IIndexPageIterator();
		virtual ~IIndexPageIterator();
		virtual bool isValid() = 0;
		virtual bool next() = 0;
		virtual bool back() = 0;
		virtual bool isNull() = 0;
		virtual bool getKey(IVariant* pIndexKey) = 0;
		virtual uint64 getPage() = 0;
		virtual uint32 getPos() = 0;
	};

	typedef CommonLib::IRefCntPtr<IIndexPageIterator> IndexPageIteratorPtr;



	class IValueFiled: public CommonLib::RefCounter
	{
	public:
		IValueFiled() {}
		virtual ~IValueFiled() {}
		//virtual bool insert (uint64 nOID, IVariant* pFieldVal, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL) = 0;
		//virtual uint64 insert (IVariant* pFieldVal, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL) = 0;

		virtual bool insert (uint64 nOID, CommonLib::CVariant* pVal, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL) = 0;
		virtual uint64 insert ( CommonLib::CVariant* pVal, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL) = 0;


		virtual bool update (uint64 nRowID, CommonLib::CVariant* pFieldVal) = 0;
		virtual bool remove (uint64 nRowID, IFieldIterator **pRetIter = NULL) = 0;
		virtual bool find(uint64 nOID, CommonLib::CVariant* pFieldVal) = 0;
		virtual FieldIteratorPtr find(uint64 nRowID) = 0;
		virtual FieldIteratorPtr begin() = 0;
		virtual FieldIteratorPtr last() = 0;
		virtual bool commit() = 0;
	};

	template<class TKeyType, class TIterator, class TIteratorPtr>
	class TIndexFiled  
	{
	public:
		TIndexFiled() {}
		virtual ~TIndexFiled() {}
		virtual bool insert (TKeyType* pIndexKey, uint64 nOID, TIterator* pFromIter = NULL, TIterator** pRetIter = NULL) = 0;
		virtual bool update (TKeyType* pOldIndexKey, TKeyType* pNewIndexKey, uint64 nOID, TIterator* pFromIter = NULL, TIterator** pRetIter = NULL) = 0;
		virtual bool remove (TKeyType* pIndexKey, TIterator** pRetIter = NULL) = 0;
		virtual bool remove (IIndexIterator* pIter ) = 0;
		virtual TIteratorPtr find(TKeyType* pIndexKey) = 0;
		virtual TIteratorPtr lower_bound(TKeyType* pIndexKey) = 0;
		virtual TIteratorPtr upper_bound(TKeyType* pIndexKey) = 0;
		virtual bool commit() = 0;
	};


	typedef TIndexFiled</*IVariant*/CommonLib::CVariant, IIndexIterator, IndexIteratorPtr> IndexFiled;

	/*class IndexFiled  
	{
	public:
		IndexFiled() {}
		virtual ~IndexFiled() {}
		virtual bool insert (IFieldVariant* pIndexKey, uint64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL) = 0;
		virtual bool update (IFieldVariant* pOldIndexKey, IFieldVariant* pNewIndexKey, uint64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL) = 0;
		virtual bool remove (IFieldVariant* pIndexKey, IIndexIterator** pRetIter = NULL) = 0;
		virtual bool remove (IIndexIterator* pIter ) = 0;
		virtual IndexIteratorPtr find(IFieldVariant* pIndexKey) = 0;
		virtual IndexIteratorPtr lower_bound(IFieldVariant* pIndexKey) = 0;
		virtual IndexIteratorPtr upper_bound(IFieldVariant* pIndexKey) = 0;
		virtual bool commit() = 0;
	};*/



	class ICounterFiled: public CommonLib::RefCounter
	{
	public:
		ICounterFiled() {}
		virtual ~ICounterFiled() {}
		virtual bool insert (uint64 nRowID, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL) = 0;
		virtual bool update (uint64 nRowID, IVariant* pFieldVal) = 0;
		virtual bool remove (uint64 nRowID, IFieldIterator **pRetIter = NULL) = 0;
		virtual bool find(uint64 nOID, IVariant* pFieldVal) = 0;
		virtual FieldIteratorPtr find(uint64 nOID) = 0;
		virtual FieldIteratorPtr begin() = 0;
		virtual FieldIteratorPtr last() = 0;
		virtual bool commit() = 0;
	};


}
#endif



