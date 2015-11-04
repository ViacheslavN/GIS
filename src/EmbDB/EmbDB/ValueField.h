#ifndef _EMBEDDED_DATABASE_FIELD_VALUE_H_
#define _EMBEDDED_DATABASE_FIELD_VALUE_H_
#include "embDBInternal.h"
#include "Key.h"
#include "BaseBPMapv2.h"
//#include "VariantField.h"
#include "DBFieldInfo.h"
#include "Database.h"
#include "DBMagicSymbol.h"
#include "BaseBPTreeRO.h"
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
			uint64 inc(uint64 nKey)
			{
				return  nKey + 1;
			}
			uint64 begin()
			{
				return 1;
			}
		};

		ValueFieldBase( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, const sFieldInfo *pFieldInfo) :
			  m_pDBTransactions(pTransactions),
			  m_tree(-1, pTransactions, pAlloc, 100), 
			  m_nBTreeRootPage(-1), m_pAlloc(pAlloc), m_pFieldInfo(pFieldInfo)
			  {

			  }

			typedef _TBTree TBTree;
			typedef typename TBTree::iterator iterator;



			virtual const sFieldInfo* getFieldInfoType() const 
			{
				return m_pFieldInfo;
			}
			virtual bool save()
			{
				return m_tree.saveBTreeInfo();
			}
			virtual bool load(int64 nAddr, eTransactionsType type)
			{

				int64 m_nFieldInfoPage = nAddr;
				FilePagePtr pPage = m_pDBTransactions->getFilePage(nAddr);
				if(!pPage.get())
					return false;
				CommonLib::FxMemoryReadStream stream;
				stream.attach(pPage->getRowData(), pPage->getPageSize());
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
				stream.read(m_nBTreeRootPage);
				m_tree.setRootPage(m_nBTreeRootPage);
				return m_tree.loadBTreeInfo(); 
			 
			}


			virtual bool init(int64 nBTreeRootPage, int64 nInnerCompPage = -1, int64 nLeafCompPage = -1)
			{

				m_nBTreeRootPage = nBTreeRootPage;
				m_tree.setRootPage(m_nBTreeRootPage);
				m_tree.setCompPageInfo(nInnerCompPage, nLeafCompPage);
				if(! m_tree.init())
					return false;

				/*if(nInnerCompPage != -1 || nLeafCompPage != -1)
				{
					m_tree.setCompPageInfo(nInnerCompPage, nLeafCompPage);
					m_tree.saveBTreeInfo(); 
				}*/
			
				return true;
			}

			TBTree* getBTree() {return &m_tree;}

			template<class TValue>
			bool insertImpl (uint64 nOID, const TValue& value, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL)
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
						((TFieldIterator*)(*pRetIter))->set(RetIterator);
					else
						*pRetIter = (IFieldIterator*)new TFieldIterator(RetIterator); 
				}
				return bRet;
			}

			template<class TValue>
			uint64 insertImpl (const TValue& value, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL)
			{

				iterator *pFromIterator = NULL;
				iterator RetIterator;
				if(pFromIter)
				{
					TFieldIterator *pFieldIterator = (TFieldIterator*)pFromIter;
					assert(pFieldIterator);
					pFromIterator = &pFieldIterator->m_ParentIt;
				}
				uint64 nOID;
				if(!m_tree.insertLast(TOIDIncFunctor(), value, &nOID, pFromIterator, pRetIter ? &RetIterator : NULL))
					return  0;

				if(pRetIter)
				{
					if(*pRetIter) 
						((TFieldIterator*)(*pRetIter))->set(RetIterator);
					else
						*pRetIter = new TFieldIterator(RetIterator); 
				}
				return nOID;
			}

			template<class TValue>
			bool updateImpl (uint64 nOID, const TValue& value)
			{
				//TO DO Update  Index
				return  m_tree.update(nOID, value);
			}
			virtual bool remove(uint64 nOID)
			{
				//TO DO Delete from Index
				return m_tree.remove(nOID);

				 
			}
			virtual bool remove (uint64 nOID, IFieldIterator **pRetIter) 
			{
				if(pRetIter)
				{
					TBTree::iterator retIt;
					return m_tree.remove(nOID);
				}
				else
					return m_tree.remove(nOID);
			}
			virtual bool insert (uint64 nOID, CommonLib::CVariant* pVariant, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL)
			{
				bool bRet = insertImpl(nOID, pVariant->Get<FType>(), pFromIter, pRetIter);
				if(bRet && m_pIndex.get())
				{
					m_pIndex->insert(pVariant, nOID);
				}
				return bRet;
			}
			virtual uint64 insert (CommonLib::CVariant* pVariant, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL)
			{
				uint64 nOID =  insertImpl(pVariant->Get<FType>(), pFromIter, pRetIter);
				if(nOID != 0 && m_pIndex.get())
				{
					m_pIndex->insert(pVariant, nOID);
				}
				return nOID;
			}
			virtual bool update (uint64 nOID, CommonLib::CVariant* pVariant)
			{
				return updateImpl(nOID, pVariant->Get<FType>()); 
			}
			virtual bool commit()
			{
				if(m_pIndex.get())
					m_pIndex->commit();
				return m_tree.commit();
			}
			IFieldIteratorPtr find(uint64 nOID)
			{
				TBTree::iterator it = m_tree.find(nOID);
				TFieldIterator *pFiledIterator = new TFieldIterator(it);
				return IFieldIteratorPtr(pFiledIterator);
			}
			IFieldIteratorPtr begin()
			{
				TBTree::iterator it = m_tree.begin();
				TFieldIterator *pFiledIterator = new TFieldIterator(it);
				return IFieldIteratorPtr(pFiledIterator);
			}
			virtual IFieldIteratorPtr last()
			{
				TBTree::iterator it = m_tree.last();
				TFieldIterator *pFiledIterator = new TFieldIterator(it);
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
	 protected:
		IDBTransaction* m_pDBTransactions;
		TBTree m_tree;
		int64 m_nBTreeRootPage;
		CommonLib::alloc_t* m_pAlloc;
		IndexFiledPtr m_pIndex;
		const sFieldInfo *m_pFieldInfo;
	};
	



	template<class TBTree>
	class FieldIterator: public IFieldIterator
	{
	public:
		typedef typename TBTree::iterator  iterator;

		FieldIterator(iterator& it) : m_ParentIt(it){}
		FieldIterator() {}
		virtual ~FieldIterator(){}
	
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
		virtual bool getVal(CommonLib::CVariant* pVal)
		{
			pVal->setVal(m_ParentIt.value());
			return true;
		}
		virtual uint64 getRowID()
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
			return m_ParentIt.setAddr(pIter->addr(), pIter->pos());
		}

		void set(iterator it)
		{
			m_ParentIt = it;
		}
	public:
		iterator m_ParentIt;

	};
	
	
	
	template<class _FType, class _TBTree, class _FieldIterator>
	class ValueField : public ValueFieldBase<_FType, _TBTree, _FieldIterator>
	{
		public:
	 
	
			typedef _FieldIterator TFieldIterator;


			ValueField( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, const sFieldInfo* pFieldInfo) :
			  ValueFieldBase(pTransactions, pAlloc, pFieldInfo)
			{

			}
			~ValueField(){}
			typedef ValueFieldBase<_FType, _TBTree, TFieldIterator> TBase;
			typedef typename TBase::TBTree TBTree;
			typedef typename TBTree::iterator  iterator;
			typedef _FType FType;


			virtual bool find(uint64 nOID, CommonLib::CVariant* pFieldVal)
			{
				TBTree::iterator it = m_tree.find(nOID);
				if(it.isNull())
					return false;
				pFieldVal->setVal(it.value());
				return true;
			}

			
		
	};
	
	template<class _FType, int FieldDataType,
		class _TLeafCompressor = embDB::BPLeafNodeMapSimpleCompressorV2<uint64, _FType > 	
	>
	class ValueFieldHandler :  CDBFieldHandlerBase
	{
		public:

			typedef _FType FType;
			typedef embDB::BPInnerNodeSimpleCompressorV2<uint64> TInnerCompressor;
			typedef _TLeafCompressor TLeafCompressor;

			typedef embDB::TBPMapV2<uint64, FType, embDB::comp<uint64>, 
				embDB::IDBTransaction, TInnerCompressor, TLeafCompressor> TBTree;
 
			typedef FieldIterator<TBTree> TFieldIterator;
			typedef ValueField<FType, TBTree, TFieldIterator> TField;
	
			ValueFieldHandler(CommonLib::alloc_t* pAlloc) : CDBFieldHandlerBase(pAlloc)
			{}
			~ValueFieldHandler()
			{}
			
			virtual bool save(int64 nAddr, IDBTransaction *pTran)
			{
				return CDBFieldHandlerBase::save<TField>(nAddr, pTran, m_pAlloc, FIELD_PAGE, FIELD_INFO_PAGE);
			}
			virtual IValueFieldPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage)
			{
				TField * pField = new  TField(pTransactions, m_pAlloc, &m_fi);
				pField->load(m_fi.m_nFieldPage, pTransactions->getType());
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