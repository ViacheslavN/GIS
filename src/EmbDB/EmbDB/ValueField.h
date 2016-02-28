#ifndef _EMBEDDED_DATABASE_FIELD_VALUE_H_
#define _EMBEDDED_DATABASE_FIELD_VALUE_H_
#include "embDBInternal.h"
#include "Key.h"
#include "BaseBPMapv2.h"
//#include "VariantField.h"
#include "DBFieldInfo.h"
#include "Database.h"
#include "DBMagicSymbol.h"
//#include "BaseBPTreeRO.h"
#include "FieldIteratorBase.h"
#include "BaseInnerNodeDIffCompress.h"
#include "BaseLeafNodeCompDiff.h"
namespace embDB
{




	//class IndexFiled;


	class IOIDFiledRO   //read only
	{
	public:
		IOIDFiledRO() {}
		virtual ~IOIDFiledRO() {}
		virtual bool find(int64 nOID, CommonLib::CVariant* pFieldVal) = 0;
	};

	//TBaseBPlusTree(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, bool bMulti = false) :
	

	template<class FType, class _TBTree, class TFieldIterator>
	class ValueFieldBase : public  IValueField
	{
	public:


		class TOIDIncFunctor
		{
		public:
			int64 inc(uint64 nKey)
			{
				return  nKey + 1;
			}
			int64 begin()
			{
				return 1;
			}
		};
		TOIDIncFunctor m_OIDIncFunck;
		ValueFieldBase( IDBFieldHandler* pFieldHandler, IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, uint32 nPageSize) :
			  m_pDBTransactions(pTransactions),
			  m_tree(-1, pTransactions, pAlloc, 100, nPageSize), 
			  m_nBTreeInfoPage(-1), m_pAlloc(pAlloc), m_pFieldHandler(pFieldHandler)
			  {


				  m_tree.SetMinSplit(true);
			  }

			typedef _TBTree TBTree;
			typedef typename TBTree::iterator iterator;
			typedef typename TBTree::TComp TComp;
			typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
			typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;


			~ValueFieldBase()
			{

			}
			
			
			virtual bool load(int64 nRootBPTreeField)
			{
				m_nBTreeInfoPage = nRootBPTreeField;
			/*	FilePagePtr pPage = m_pDBTransactions->getFilePage(nRootBPTreeField, MIN_PAGE_SIZE);
				if(!pPage.get())
					return false;
				CommonLib::FxMemoryReadStream stream;
				stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
				sFilePageHeader header(stream);
				if(!header.isValid())
				{
					m_pDBTransactions->error(_T("OIDField: Page %I64d Error CRC for node page"), pPage->getAddr()); //TO DO log error
					return false;
				}
				if(header.m_nObjectPageType != FIELD_PAGE || header.m_nSubObjectPageType != FIELD_INFO_PAGE)
				{
					m_pDBTransactions->error(_T("OIDField: Page %I64d Not field info page"), pPage->getAddr()); //TO DO log error
					return false;
				}
				stream.read(m_nBTreeInfoPage);*/
				m_tree.setRootPage(m_nBTreeInfoPage);
				return m_tree.loadBTreeInfo(); 
			 
			}


			virtual bool init(int64 nBTreeRootPage, TInnerCompressorParams *pInnerCompressorParams = NULL,
				TLeafCompressorParams *pLeafCompressorParams = NULL)
			{

				m_nBTreeInfoPage = nBTreeRootPage;
				
				if(!m_tree.init(m_nBTreeInfoPage, pInnerCompressorParams, pLeafCompressorParams))
					return false;

				return true;
			}

			TBTree* getBTree() {return &m_tree;}

			template<class TValue>
			bool insertImpl (int64 nOID, const TValue& value, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL)
			{
				iterator *pFromIterator = NULL;
				iterator RetIterator;
				if(pFromIterator)
				{
					TFieldIterator *pFieldIterator = (TFieldIterator*)pFromIter;
					assert(pFromIterator);
					pFromIterator = &pFieldIterator->m_ParentIt;
				}
				bool bRet =  m_tree.insert(nOID, value, pFromIterator, pRetIter ? &RetIterator : NULL);


				if(pRetIter)
				{
					if(*pRetIter) 
						((TFieldIterator*)(*pRetIter))->set(RetIterator, this);
					else
						*pRetIter = (IFieldIterator*)new TFieldIterator(RetIterator, this); 
				}
				return bRet;
			}

			template<class TValue>
			int64 insertImpl (const TValue& value, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL)
			{

				iterator *pFromIterator = NULL;
				iterator RetIterator;
				if(pFromIter)
				{
					TFieldIterator *pFieldIterator = (TFieldIterator*)pFromIter;
					assert(pFieldIterator);
					pFromIterator = &pFieldIterator->m_ParentIt;
				}
				int64 nOID;
				if(!m_tree.insertLast(m_OIDIncFunck, value, &nOID, pFromIterator, pRetIter ? &RetIterator : NULL))
					return  0;

				if(pRetIter)
				{
					if(*pRetIter) 
						((TFieldIterator*)(*pRetIter))->set(RetIterator, this);
					else
						*pRetIter = new TFieldIterator(RetIterator, (IValueField*)this); 
				}
				return nOID;
			}

			template<class TValue>
			bool updateImpl (int64 nOID, const TValue& value)
			{
				//TO DO Update  Index
				return  m_tree.update(nOID, value);
			}
			virtual bool remove(int64 nOID)
			{
				//TO DO Delete from Index
				return m_tree.remove(nOID);

				 
			}
			virtual bool remove (int64 nOID, IFieldIterator **pRetIter) 
			{
				if(pRetIter)
				{
					typename TBTree::iterator retIt;
					return m_tree.remove(nOID);
				}
				else
					return m_tree.remove(nOID);
			}
			virtual bool insert (int64 nOID, CommonLib::CVariant* pVariant, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL)
			{
				bool bRet = insertImpl(nOID, pVariant->Get<FType>(), pFromIter, pRetIter);
				if(bRet && m_pIndex.get())
				{
					m_pIndex->insert(pVariant, nOID);
				}
				return bRet;
			}
			virtual int64 insert (CommonLib::CVariant* pVariant, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL)
			{
				int64 nOID =  insertImpl(pVariant->Get<FType>(), pFromIter, pRetIter);
				if(nOID != 0 && m_pIndex.get())
				{
					m_pIndex->insert(pVariant, nOID);
				}
				return nOID;
			}
			virtual bool update (int64 nOID, CommonLib::CVariant* pVariant)
			{
				return updateImpl(nOID, pVariant->Get<FType>()); 
			}
			virtual bool commit()
			{
				if(m_pIndex.get())
					m_pIndex->commit();
				return m_tree.commit();
			}
			IFieldIteratorPtr find(int64 nOID,  IFieldIterator* pFromIter = NULL)
			{

				iterator *pFromIterator = NULL;
				if(pFromIter)
				{
					TFieldIterator *pFieldIterator = dynamic_cast<TFieldIterator*>(pFromIter);
					assert(pFieldIterator);
					pFromIterator = &pFieldIterator->m_ParentIt;
				}

				typename  TBTree::iterator it = m_tree.find(nOID, pFromIterator, true);
				TFieldIterator *pFiledIterator = new TFieldIterator(it, this);
				return IFieldIteratorPtr(pFiledIterator);
			}
	
			virtual IFieldIteratorPtr upper_bound(int64 nRowID,  IFieldIterator* pFromIter = NULL)
			{

				iterator *pFromIterator = NULL;
				if(pFromIter)
				{
					TFieldIterator *pFieldIterator = dynamic_cast<TFieldIterator*>(pFromIter);
					assert(pFieldIterator);
					pFromIterator = &pFieldIterator->m_ParentIt;
				}

				typename  TBTree::iterator it = m_tree.upper_bound(nRowID, pFromIterator, true);
				TFieldIterator *pFiledIterator = new TFieldIterator(it, this);
				return IFieldIteratorPtr(pFiledIterator);
			}
			virtual IFieldIteratorPtr lower_bound(int64 nRowID,  IFieldIterator* pFromIter = NULL)
			{
				iterator *pFromIterator = NULL;
				if(pFromIter)
				{
					TFieldIterator *pFieldIterator = dynamic_cast<TFieldIterator*>(pFromIter);
					assert(pFieldIterator);
					pFromIterator = &pFieldIterator->m_ParentIt;
				}

				typename  TBTree::iterator it = m_tree.lower_bound(nRowID, pFromIterator, true);
				TFieldIterator *pFiledIterator = new TFieldIterator(it, this);
				return IFieldIteratorPtr(pFiledIterator);
			}

			IFieldIteratorPtr begin()
			{
				typename  TBTree::iterator it = m_tree.begin();
				TFieldIterator *pFiledIterator = new TFieldIterator(it, this);
				return IFieldIteratorPtr(pFiledIterator);
			}
			virtual IFieldIteratorPtr last()
			{
				typename  TBTree::iterator it = m_tree.last();
				TFieldIterator *pFiledIterator = new TFieldIterator(it, this);
				return IFieldIteratorPtr(pFiledIterator);
			}
			virtual IndexFiledPtr GetIndex()
			{
				return m_pIndex;
			}
			virtual void SetIndex(IndexFiled *pIndex)
			{
				m_pIndex = pIndex;
			}

			virtual IDBFieldHandlerPtr GetFieldHandler() const
			{
				return m_pFieldHandler;
			}
	 protected:
		IDBTransaction* m_pDBTransactions;
		TBTree m_tree;
		int64 m_nBTreeInfoPage;
		CommonLib::alloc_t* m_pAlloc;
		IndexFiledPtr m_pIndex;
		IDBFieldHandlerPtr m_pFieldHandler;
		const sFieldInfo *m_pFieldInfo;
	};
	



	template<class TBTree>
	class FieldIterator: public TFieldIteratorBase<TBTree, IFieldIterator>
	{
	public:
	 
		typedef typename TBTree::iterator  iterator;
		typedef TFieldIteratorBase<TBTree, IFieldIterator> TBase;

		FieldIterator(iterator& it,  IValueField* pField) : TBase(it, pField)
		{
		}
		FieldIterator() {}
		virtual ~FieldIterator(){}

		virtual bool getVal(CommonLib::CVariant* pVal)
		{
			pVal->setVal(this->m_ParentIt.value());
			return true;
		}
	};
	
	
	
	template<class _FType, class _TBTree, class _FieldIterator>
	class ValueField : public ValueFieldBase<_FType, _TBTree, _FieldIterator>
	{
		public:
	 
	
			typedef _FieldIterator TFieldIterator;
			typedef ValueFieldBase<_FType, _TBTree, TFieldIterator> TBase;

			ValueField(IDBFieldHandler* pFieldHandler,  IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, uint32 nPageSize) :
			  TBase(pFieldHandler, pTransactions, pAlloc, nPageSize)
			{

			}

		 
			~ValueField()
			{}
			
			typedef typename TBase::TBTree TBTree;
			typedef typename TBTree::iterator  iterator;
			typedef _FType FType;


			virtual bool find(int64 nOID, CommonLib::CVariant* pFieldVal)
			{
				typename TBTree::iterator it = this->m_tree.find(nOID);
				if(it.isNull())
					return false;
				pFieldVal->setVal(it.value());
				return true;
			}

			
		
	};
	
	template<class _FType, int FieldDataType,
		class _TLeafCompressor = embDB::TBaseLeafNodeDiffComp<int64, _FType, embDB::IDBTransaction, embDB::OIDCompressor> 	
	>
	class ValueFieldHandler :  CDBFieldHandlerBase<IDBFieldHandler>
	{
		public:

			typedef _FType FType;
			typedef embDB::TBPBaseInnerNodeDiffCompressor<int64, embDB::OIDCompressor, embDB::InnerLinkCompress>	TInnerCompressor;
			typedef _TLeafCompressor TLeafCompressor;



			typedef embDB::TBPMapV2<int64, FType, embDB::comp<int64>, 
			embDB::IDBTransaction, TInnerCompressor, TLeafCompressor> TBTree;
 
			typedef typename TBTree::iterator  iterator;
			typedef FieldIterator<TBTree> TFieldIterator;
			typedef ValueField<FType, TBTree, TFieldIterator> TField;

			typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
			typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;
	
			ValueFieldHandler(CommonLib::alloc_t* pAlloc, const SFieldProp* pFieldProp, int64 nPageAdd) : CDBFieldHandlerBase<IDBFieldHandler>(pAlloc, pFieldProp, nPageAdd)
			{}
			~ValueFieldHandler()
			{}
			
			virtual bool save(CommonLib::IWriteStream* pStream,  IDBTransaction *pTran)
			{
				return CDBFieldHandlerBase::save<TField, TInnerCompressorParams, TLeafCompressorParams>(pStream, pTran, m_pAlloc);
			}

			virtual bool load(CommonLib::IReadStream* pStream,  IDBTransaction *pTran)
			{
				return true;
			}


			virtual IValueFieldPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage)
			{
				TField * pField = new  TField(this, pTransactions, m_pAlloc,m_nPageSize);
				pField->load(m_nFieldInfoPage);
				return IValueFieldPtr(pField);	
			}
			virtual bool release(IValueField* pField)
			{
				/*TField* pOIDField = (TField*)pField;

				TField::TBTree *pBTree = pOIDField->getBTree();

				delete pField;*/
				return true;
			}
	};
 
	typedef ValueFieldHandler<int64, dtInteger64> TValFieldINT64;
	typedef ValueFieldHandler<uint64,dtUInteger64> TValFieldUINT64;
	typedef ValueFieldHandler<int32, dtInteger32> TValFieldINT32;
	typedef ValueFieldHandler<uint32,dtUInteger32> TValFieldUINT32;
	typedef ValueFieldHandler<int16, dtInteger16> TValFieldINT16;
	typedef ValueFieldHandler<uint16,dtUInteger16> TValFieldUINT16;
	typedef ValueFieldHandler<int32, dtUInteger8> TValFieldINT8;
	typedef ValueFieldHandler<uint32,dtInteger8> TValFieldUINT8;
	typedef ValueFieldHandler<double,dtDouble> TValFieldDouble;
	typedef ValueFieldHandler<float, dtFloat> TValFieldFloat;
 
}
#endif