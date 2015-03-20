#ifndef _EMBEDDED_DATABASE_I_DB_FIELD_H_
#define _EMBEDDED_DATABASE_I_DB_FIELD_H_

#include "IField.h"

namespace embDB
{

	class IFieldIterator : public AutoRefCounter
	{
	public:
		IFieldIterator(){};
		virtual ~IFieldIterator(){}
		virtual bool isValid() = 0;
		virtual bool next() = 0;
		virtual bool back() = 0;
		virtual bool isNull() = 0;
		virtual bool getVal(IFieldVariant* pIndexKey) = 0;
		virtual uint64 getObjectID() = 0;

		virtual int64 addr() const = 0;
		virtual int32 pos() const = 0;

		virtual bool copy(IFieldIterator *pIter) = 0;
	};

	typedef IRefCntPtr<IFieldIterator> FieldIteratorPtr;


	class IIndexIterator : public RefCounter
	{
	public:
		IIndexIterator(){};
		virtual ~IIndexIterator(){};
		virtual bool isValid() = 0;
		virtual bool next() = 0;
		virtual bool isNull() = 0;
		virtual bool getKey(IFieldVariant* pIndexKey) = 0;
		virtual uint64 getObjectID() = 0;


		virtual int64 addr() const = 0;
		virtual int32 pos() const = 0;

		virtual bool copy(IIndexIterator *pIter) = 0;
	};


	typedef IRefCntPtr<IIndexIterator> IndexIteratorPtr;

	class IIndexPageIterator  : public RefCounter
	{
	public:
		IIndexPageIterator();
		virtual ~IIndexPageIterator();
		virtual bool isValid() = 0;
		virtual bool next() = 0;
		virtual bool back() = 0;
		virtual bool isNull() = 0;
		virtual bool getKey(IFieldVariant* pIndexKey) = 0;
		virtual uint64 getPage() = 0;
		virtual uint32 getPos() = 0;
	};

	typedef IRefCntPtr<IIndexPageIterator> IndexPageIteratorPtr;



	class IOIDFiled: public RefCounter
	{
	public:
		IOIDFiled() {}
		virtual ~IOIDFiled() {}
		virtual bool insert (uint64 nOID, IFieldVariant* pFieldVal, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL) = 0;
		virtual uint64 insert (IFieldVariant* pFieldVal, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL) = 0;
		virtual bool update (uint64 nOID, IFieldVariant* pFieldVal) = 0;
		virtual bool remove (uint64 nOID, IFieldIterator **pRetIter = NULL) = 0;
		virtual bool find(uint64 nOID, IFieldVariant* pFieldVal) = 0;
		virtual FieldIteratorPtr find(uint64 nOID) = 0;
		virtual bool commit() = 0;
	};
}
#endif



