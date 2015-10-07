#ifndef _EMBEDDED_DATABASE_FIELD_VALUE_H_
#define _EMBEDDED_DATABASE_FIELD_VALUE_H_
#include "Key.h"
#include "BaseBPMapv2.h"
#include "VariantField.h"
#include "DBFieldInfo.h"
#include "IDBTransactions.h"
#include "Database.h"
#include "DBMagicSymbol.h"
#include "IDBStorage.h"
#include "IDBTransactions.h"
#include "BaseBPTreeRO.h"
namespace embDB
{




	//class IndexFiled;


	class IOIDFiledRO   //read only
	{
	public:
		IOIDFiledRO() {}
		virtual ~IOIDFiledRO() {}
		virtual bool find(int64 nOID, IVariant* pFieldVal) = 0;
	};

	//TBaseBPlusTree(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, bool bMulti = false) :
	

	template<class _TBTree>
	class ValueFieldBase 
	{
	public:
		ValueFieldBase( IDBTransactions* pTransactions, CommonLib::alloc_t* pAlloc) :
			  m_pDBTransactions(pTransactions),
			  m_tree(-1, pTransactions, pAlloc, 100), 
			  m_nBTreeRootPage(-1)
			  {

			  }

			typedef _TBTree TBTree;
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


			virtual bool init(int64 nBTreeRootPage)
			{

				m_nBTreeRootPage = nBTreeRootPage;
				m_tree.setRootPage(m_nBTreeRootPage);
				return m_tree.init();
			}

			TBTree* getBTree() {return &m_tree;}
	 protected:
		IDBTransactions* m_pDBTransactions;
		TBTree m_tree;
		int64 m_nBTreeRootPage;
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
	
	
	
	template<class _FType, class _TBTree, int FieldDataType>
	class ValueField : public ValueFieldBase<_TBTree>, public  IValueFiled
	{
		public:
	 
	
			typedef FieldIterator<_TBTree> TFieldIterator;

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

			ValueField( IDBTransactions* pTransactions, CommonLib::alloc_t* pAlloc) :
			  ValueFieldBase(pTransactions, pAlloc)
			{

			}
			~ValueField(){}
			typedef ValueFieldBase<_TBTree> TBase;
			typedef typename TBase::TBTree TBTree;
			typedef typename TBTree::iterator  iterator;
			typedef _FType FType;

			virtual bool insert (uint64 nOID, CommonLib::CVariant* pVariant, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL)
			{
				iterator *pFromIterator = NULL;
				iterator RetIterator;
				if(pFromIterator)
				{
					TFieldIterator *pFieldIterator = (TFieldIterator*)pFromIter;
					assert(pFromIterator);
					pFromIterator = &pFieldIterator->m_ParentIt;
				}

			
				FType val = pVariant->Get<FType>();
				bool bRet =  m_tree.insert(nOID, val, pFromIterator, pRetIter ? &RetIterator : NULL);


				if(pRetIter)
				{
					if(*pRetIter) 
						((TFieldIterator*)(*pRetIter))->set(RetIterator);
					else
						*pRetIter = new TFieldIterator(RetIterator); 
				}
				return bRet;
			}
			virtual uint64 insert (CommonLib::CVariant* pVariant, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL)
			{

				iterator *pFromIterator = NULL;
				iterator RetIterator;
				if(pFromIter)
				{
					TFieldIterator *pFieldIterator = (TFieldIterator*)pFromIter;
					assert(pFieldIterator);
					pFromIterator = &pFieldIterator->m_ParentIt;
				}

				FType val = pVariant->Get<FType>();
				uint64 nOID;
				if(!m_tree.insertLast(TOIDIncFunctor(), val, &nOID, pFromIterator, pRetIter ? &RetIterator : NULL))
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


			virtual bool update (uint64 nOID, CommonLib::CVariant* pFieldVal)
			{
				FType val;
				pFieldVal->getVal(val);
				return  m_tree.update(nOID, val);
			}
			virtual bool remove(uint64 nOID)
			{
				return m_tree.remove(nOID);
			}
			virtual bool remove (uint64 nOID, IFieldIterator **pRetIter = NULL) 
			{
				if(pRetIter)
				{
					TBTree::iterator retIt;
					return m_tree.remove(nOID);
				}
				else
					return m_tree.remove(nOID);
			}
			virtual bool find(uint64 nOID, CommonLib::CVariant* pFieldVal)
			{
				TBTree::iterator it = m_tree.find(nOID);
				if(it.isNull())
					return false;
				pFieldVal->setVal(it.value());
				return true;
			}
			FieldIteratorPtr find(uint64 nOID)
			{
				TBTree::iterator it = m_tree.find(nOID);
				TFieldIterator *pFiledIterator = new TFieldIterator(it);
				return FieldIteratorPtr(pFiledIterator);
			}
			FieldIteratorPtr begin()
			{
				TBTree::iterator it = m_tree.begin();
				TFieldIterator *pFiledIterator = new TFieldIterator(it);
				return FieldIteratorPtr(pFiledIterator);
			}
			virtual FieldIteratorPtr last()
			{
				TBTree::iterator it = m_tree.last();
				TFieldIterator *pFiledIterator = new TFieldIterator(it);
				return FieldIteratorPtr(pFiledIterator);
			}
			virtual bool commit()
			{
				return m_tree.commit();
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
				embDB::IDBTransactions, TInnerCompressor, TLeafCompressor> TBTree;
 
			typedef ValueField<FType, TBTree, FieldDataType> TField;
	
			ValueFieldHandler(CommonLib::alloc_t* pAlloc) : CDBFieldHandlerBase(pAlloc)
			{}
			~ValueFieldHandler()
			{}
			
			virtual bool save(int64 nAddr, IDBTransactions *pTran)
			{
				return CDBFieldHandlerBase::save<TField>(nAddr, pTran, m_pAlloc, FIELD_PAGE, FIELD_INFO_PAGE);
			}
			virtual IValueFiled* getOIDField(IDBTransactions* pTransactions, IDBStorage *pStorage)
			{
				TField * pField = new  TField(pTransactions, m_pAlloc);
				pField->load(m_fi.m_nFieldPage, pTransactions->getType());
				return pField;	
			}
			virtual bool release(IValueFiled* pField)
			{
				TField* pOIDField = (TField*)pField;

				TField::TBTree *pBTree = pOIDField->getBTree();

				delete pField;
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