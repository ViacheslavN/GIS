#ifndef _EMBEDDED_DATABASE_UNIQUE_INDEX_H_
#define _EMBEDDED_DATABASE_UNIQUE_INDEX_H_
#include "BaseBPMapv2.h"
#include "embDBInternal.h"
#include "DBFieldInfo.h"
#include "Index.h"

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
		typedef TIndexIterator<TBTree> TIndexIterator;


		CUniqueIndex( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc) :
			TBase(pTransactions, pAlloc)
			{}
		~CUniqueIndex(){}

		virtual bool insert (CommonLib::CVariant* pIndexKey, uint64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
		{

			iterator *pFromIterator = NULL;
			iterator RetIterator;
			if(pFromIterator)
			{
				TIndexIterator *pFieldIterator = (TIndexIterator*)pFromIter;
				assert(pFromIterator);
				pFromIterator = &pFieldIterator->m_ParentIt;
			}

			FType val;
			pIndexKey->getVal(val);
			bool bRet =  m_tree.insert(val, nOID, pFromIterator, pRetIter ? &RetIterator : NULL);


			if(pRetIter)
			{
				if(*pRetIter) 
					((TIndexIterator*)(*pRetIter))->set(RetIterator);
				else
					*pRetIter = new TIndexIterator(RetIterator); 
			}
			return bRet;
		}
		virtual bool update (CommonLib::CVariant* pOldIndexKey, CommonLib::CVariant* pNewIndexKey, uint64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
		{
			//FType val;
			//pIndexKey->getVal(val);
			//return  m_tree.update(nOID, val);
			return true;

		}
		virtual bool remove (CommonLib::CVariant* pIndexKey, IIndexIterator** pRetIter = NULL)
		{
			return true;
		}
		virtual bool remove (IIndexIterator* pIter )
		{
			return true;
		}
		virtual IIndexIteratorPtr find(CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			TBTree::iterator it = m_tree.find(val);
			TIndexIterator *pIndexIterator = new TIndexIterator(it);
			return IIndexIteratorPtr(pIndexIterator);
		}
		virtual IIndexIteratorPtr lower_bound(CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);

			TBTree::iterator it = m_tree.lower_bound(val);
			TIndexIterator *pIndexIterator = new TIndexIterator(it);
			return IIndexIteratorPtr(pIndexIterator);
		}
		virtual IIndexIteratorPtr upper_bound(CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			TBTree::iterator it = m_tree.upper_bound(val);
			TIndexIterator *pIndexIterator = new TIndexIterator(it);
			return IIndexIteratorPtr(pIndexIterator);
		}

		bool remove (CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			return m_tree.remove(val);
		}

		virtual bool commit()
		{
			return m_tree.commit();
		}
	};

	template<class _FType, int FieldDataType,
	class _TInnerCompressor = embDB::BPInnerNodeSimpleCompressorV2<_FType> ,
	class _TLeafCompressor = embDB::BPLeafNodeMapSimpleCompressorV2<_FType, uint64 >,
	class _TComp = embDB::comp<_FType> >
	class UniqueIndexFieldHandler : public CIndexHandlerBase 
	{
	public:

		typedef _FType FType;
		typedef _TComp TComp;
		typedef _TInnerCompressor TInnerCompressor;
		typedef _TLeafCompressor TLeafCompressor;


		typedef embDB::TBPMapV2<FType, uint64, TComp, 
			embDB::IDBTransaction, TInnerCompressor, TLeafCompressor> TBTree;

		typedef CUniqueIndex<FType, TBTree, FieldDataType> TIndex;

		UniqueIndexFieldHandler(CommonLib::alloc_t* pAlloc) : CIndexHandlerBase(pAlloc)
		{

		}
		~UniqueIndexFieldHandler()
		{

		}
	
		virtual bool save(int64 nAddr, IDBTransaction *pTran)
		{
			//m_nFieldInfoPage = nAddr;
			return CIndexHandlerBase::save<TIndex>(nAddr, pTran, m_pAlloc, INDEX_PAGE, INDEX_INFO_PAGE);

		/*	FilePagePtr pPage(pTran->getFilePage(nAddr));
			if(!pPage.get())
				return false;
			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, FIELD_PAGE, FIELD_INFO_PAGE);
			int64 m_nBTreeRootPage = -1;
			FilePagePtr pRootPage(pTran->getNewPage());
			if(!pRootPage.get())
				return false;
			m_nBTreeRootPage = pRootPage->getAddr();
			stream.write(m_nBTreeRootPage);
			header.writeCRC32(stream);
			pPage->setFlag(eFP_CHANGE, true);
			pTran->saveFilePage(pPage);

			TIndex field(pTran, m_pAlloc);
			field.init(m_nBTreeRootPage);
			return field.save();*/
		}
		virtual bool load(int64 nAddr, IDBStorage *pStorage)
		{
			return true;
		}

		virtual IndexFiled* getIndex(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{

			TIndex * pIndex = new  TIndex(pTransactions, m_pAlloc);
			pIndex->load(m_fi.m_nFieldPage, pTransactions->getType());
			return pIndex;	
		}


		virtual bool release(IndexFiled* pIndex)
		{
			TIndex* pDelIndex = (TIndex*)pIndex;

			//TIndex::TBTree *pBTree = pDelIndex->getBTree();

			delete pDelIndex;
			return true;
		}


	};


	typedef UniqueIndexFieldHandler<int64,  dtInteger64> TUniqueIndexNT64;
	typedef UniqueIndexFieldHandler<uint64, dtUInteger64> TUniqueIndexUINT64;
	typedef UniqueIndexFieldHandler<int32,  dtInteger32> TUniqueIndexINT32;
	typedef UniqueIndexFieldHandler<uint32, dtUInteger32> TUniqueIndexUINT32;
	typedef UniqueIndexFieldHandler<int16,  dtInteger16> TUniqueIndexINT16;
	typedef UniqueIndexFieldHandler<uint16, dtUInteger16> TUniqueIndexUINT16;
	typedef UniqueIndexFieldHandler<int32,  dtUInteger8> TUniqueIndexINT8;
	typedef UniqueIndexFieldHandler<uint32, dtInteger8> TUniqueIndexUINT8;
	typedef UniqueIndexFieldHandler<double, dtDouble> TUniqueIndexDouble;
	typedef UniqueIndexFieldHandler<float,  dtFloat> TUniqueIndexFloat;

}


#endif