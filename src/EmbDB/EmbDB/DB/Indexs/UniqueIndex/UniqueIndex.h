#ifndef _EMBEDDED_DATABASE_UNIQUE_INDEX_H_
#define _EMBEDDED_DATABASE_UNIQUE_INDEX_H_

#include "embDBInternal.h"
#include "DBFieldInfo.h"
#include "Index.h"

#include "../../BTreePlus/BPMap.h"
#include "../../BTreePlus/BaseNodeCompressor.h"
#include "../../BTreePlus/BaseDIffEncoder.h"
#include "../../Fields/BaseFieldEncoders.h"


namespace embDB
{


	template<class _TIndexType, class _TBTree, int FieldDataType>
	class CUniqueIndex : public CIndexBase<_TBTree>, public  IndexFiled
	{
	public:


		typedef CIndexBase<_TBTree> TBase;
		typedef typename TBase::TBTree TBTree;
		typedef _TIndexType FType;
		typedef typename TBTree::iterator  iterator;
		typedef TIndexIterator<iterator> IndexIterator;


		CUniqueIndex( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, uint32 nPageSize) :
			TBase(pTransactions, pAlloc,  nPageSize)
			{}
		~CUniqueIndex(){}
		virtual indexTypes GetType() const  {return itUnique;}

		virtual bool insert (const CommonLib::CVariant* pIndexKey, int64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
		{

			iterator *pFromIterator = NULL;
			iterator RetIterator;
			if(pFromIterator)
			{
				IndexIterator *pFieldIterator = (IndexIterator*)pFromIter;
				assert(pFromIterator);
				pFromIterator = &pFieldIterator->m_ParentIt;
			}

			FType val;
			pIndexKey->getVal(val);
			bool bRet =  this->m_tree.insert(val, nOID/*, pFromIterator, pRetIter ? &RetIterator : NULL*/);


			if(pRetIter)
			{
				if(*pRetIter) 
					((IndexIterator*)(*pRetIter))->set(RetIterator);
				else
					*pRetIter = new IndexIterator(RetIterator, this); 
			}
			return bRet;
		}
		virtual bool update (const CommonLib::CVariant* pOldIndexKey, CommonLib::CVariant* pNewIndexKey, int64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
		{
			//FType val;
			//pIndexKey->getVal(val);
			//return  m_tree.update(nOID, val);
			return true;

		}
		virtual bool remove (const CommonLib::CVariant* pIndexKey, IIndexIterator** pRetIter = NULL)
		{
			return true;
		}
		virtual bool remove ( IIndexIterator* pIter )
		{
			return true;
		}

		virtual bool IsExsist(const CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			typename TBTree::iterator it = this->m_tree.find(val);
			return !it.isNull();
		}

		virtual IIndexIteratorPtr find(const CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			typename TBTree::iterator it = this->m_tree.find(val);
			IndexIterator *pIndexIterator = new IndexIterator(it, this); // TO DO use
			return IIndexIteratorPtr(pIndexIterator);
		}
		virtual IIndexIteratorPtr lower_bound(const CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);

			typename TBTree::iterator it = this->m_tree.lower_bound(val);
			IndexIterator *pIndexIterator = new IndexIterator(it, this);  // TO DO use
			return IIndexIteratorPtr(pIndexIterator);
		}
		virtual IIndexIteratorPtr upper_bound(const CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			typename TBTree::iterator it = this->m_tree.upper_bound(val);
			IndexIterator *pIndexIterator = new IndexIterator(it, this);  // TO DO use
			return IIndexIteratorPtr(pIndexIterator);
		}

		bool remove (const CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			return this->m_tree.remove(val);
		}

		virtual bool commit()
		{
			return this->m_tree.commit();
		}
	};






	template<class _FType, int FieldDataType,
	class _TKeyEncoder= TEmptyValueEncoder<_FType>,
	class _TComp = embDB::comp<_FType> >
	class UniqueIndexFieldHolder : public CIndexHolderBase 
	{
	public:

		typedef _FType FType;
		typedef _TComp TComp;
		typedef _TKeyEncoder TKeyEncoder;

		typedef TValueDiffEncoder<int64, int64, SignedNumLenEncoder64> TOIDEncoder;
		typedef TValueDiffEncoder<int64, int64, SignedNumLenEncoder64> TLinkEncoder;
  
		typedef TBaseNodeCompressor<_FType, int64, embDB::IDBTransaction, TKeyEncoder, TLinkEncoder>  TInnerCompressor;
		typedef TBaseNodeCompressor<FType, int64, embDB::IDBTransaction, TKeyEncoder, TOIDEncoder> TLeafCompressor;



		typedef embDB::TBPMap<FType, int64, TComp, 
			embDB::IDBTransaction, TInnerCompressor, TLeafCompressor> TBTree;

		typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;

		typedef CUniqueIndex<FType, TBTree, FieldDataType> TIndex;

		UniqueIndexFieldHolder(IDBFieldHolder *pField, CommonLib::alloc_t* pAlloc, int64 nIndexPage,const SIndexProp &ip) : CIndexHolderBase(pField, pAlloc, itUnique, nIndexPage, ip)
		{

		}
		~UniqueIndexFieldHolder()
		{

		}
	
		virtual bool save(CommonLib::IWriteStream* pStream,  IDBTransaction *pTran)
		{
			TLeafCompressorParams leafCompParams;
			TInnerCompressorParams innerCompParams;
			return CIndexHolderBase::save<TIndex, TInnerCompressorParams, TLeafCompressorParams>(pStream, pTran, m_pAlloc, &innerCompParams, &leafCompParams);
		}
		virtual bool load(int64 nAddr, IDBStorage *pStorage)
		{
			return true;
		}

		virtual IndexFiledPtr getIndex(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{

			TIndex * pIndex = new  TIndex(pTransactions, m_pAlloc, m_nNodePageSize);
			pIndex->load(m_nBTreeRootPage, pTransactions->getType());
			return IndexFiledPtr(pIndex);

			return IndexFiledPtr();
		}


		virtual bool release(IndexFiled* pIndex)
		{
			/*TIndex* pDelIndex = (TIndex*)pIndex;

			TIndex::TBTree *pBTree = pDelIndex->getBTree();

			delete pDelIndex;*/
			return true;
		}


	};



	typedef TValueDiffEncoder<int16, int16, TUnsignedNumLenEncoder16> TInt16KeyEncoder;
	typedef TValueDiffEncoder<uint16, uint16, TUnsignedNumLenEncoderU16> TUInt16KeyEncoder;

	typedef TValueDiffEncoder<int32, int32, TUnsignedNumLenEncoder32> TInt32KeyEncoder;
	typedef TValueDiffEncoder<uint32, uint32, TUnsignedNumLenEncoderU32> TUInt32KeyEncoder;

	typedef TValueDiffEncoder<int64, int64, TUnsignedNumLenEncoder64> TInt64KeyEncoder;
	typedef TValueDiffEncoder<uint64, uint64, TUnsignedNumLenEncoderU64> TUInt64KeyEncoder;


	typedef UniqueIndexFieldHolder<int64,  dtInteger64, TInt64KeyEncoder > TUniqueIndexNT64;
	typedef UniqueIndexFieldHolder<uint64, dtUInteger64, TUInt64KeyEncoder > TUniqueIndexUINT64;
	typedef UniqueIndexFieldHolder<int32,  dtInteger32, TInt32KeyEncoder> TUniqueIndexINT32;
	typedef UniqueIndexFieldHolder<uint32, dtUInteger32, TUInt32KeyEncoder> TUniqueIndexUINT32;
	typedef UniqueIndexFieldHolder<int16,  dtInteger16, TInt16KeyEncoder> TUniqueIndexINT16;
	typedef UniqueIndexFieldHolder<uint16, dtUInteger16, TUInt16KeyEncoder> TUniqueIndexUINT16;
	typedef UniqueIndexFieldHolder<int32,  dtUInteger8> TUniqueIndexINT8;
	typedef UniqueIndexFieldHolder<uint32, dtInteger8> TUniqueIndexUINT8;
	typedef UniqueIndexFieldHolder<double, dtDouble> TUniqueIndexDouble;
	typedef UniqueIndexFieldHolder<float,  dtFloat> TUniqueIndexFloat;

}


#endif