#ifndef _EMBEDDED_DATABASE_FIELD_RID_H_
#define _EMBEDDED_DATABASE_FIELD_RID_H_
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







	class IOIDFiledRO   //read only
	{
	public:
		IOIDFiledRO() {}
		virtual ~IOIDFiledRO() {}
		virtual bool find(int64 nOID, IFieldVariant* pFieldVal) = 0;
	};

	//TBaseBPlusTree(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, bool bMulti = false) :
	

	template<class _TBTree>
	class OIDFieldBase 
	{
	public:
		OIDFieldBase( IDBTransactions* pTransactions, CommonLib::alloc_t* pAlloc) :
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
			virtual bool load(int64 nAddr)
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
					CommonLib::str_t sMsg;
					sMsg.format(_T("OIDField: Page %I64d Error CRC for node page"), pPage->getAddr());
					m_pDBTransactions->error(sMsg); //TO DO log error
					return false;
				}
				if(header.m_nObjectPageType != FIELD_PAGE || header.m_nSubObjectPageType != FIELD_INFO_PAGE)
				{
					CommonLib::str_t sMsg;
					sMsg.format(_T("OIDField: Page %I64d Not field info page"), pPage->getAddr());
					m_pDBTransactions->error(sMsg); //TO DO log error
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
		virtual bool getVal(IFieldVariant* pVal)
		{
			return pVal->setVal(m_ParentIt.value());
		}
		virtual uint64 getObjectID()
		{
			return m_ParentIt.key();
		}


		virtual int64 addr() const
		{
			return m_ParentIt.addr();
		}
		virtual int32 pos() const
		{
			return pos();
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
	class OIDField : public OIDFieldBase<_TBTree>, public  IOIDFiled
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

			OIDField( IDBTransactions* pTransactions, CommonLib::alloc_t* pAlloc) :
			  OIDFieldBase(pTransactions, pAlloc)
			{

			}
			~OIDField(){}
			typedef OIDFieldBase<_TBTree> TBase;
			typedef typename TBase::TBTree TBTree;
			typedef typename TBTree::iterator  iterator;
			typedef _FType FType;
		
			virtual bool insert (uint64 nOID, IFieldVariant* pFieldVal, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL)
			{
				iterator *pFromIterator = NULL;
				iterator RetIterator;
				if(pFromIterator)
				{
					TFieldIterator *pFieldIterator = (TFieldIterator*)pFromIter;
					assert(pFromIterator);
					pFromIterator = &pFieldIterator->m_ParentIt;
				}
				
				FType val;
				pFieldVal->getVal(val);
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
			virtual uint64 insert (IFieldVariant* pFieldVal, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL)
			{

				iterator *pFromIterator = NULL;
				iterator RetIterator;
				if(pFromIter)
				{
					TFieldIterator *pFieldIterator = (TFieldIterator*)pFromIter;
					assert(pFieldIterator);
					pFromIterator = &pFieldIterator->m_ParentIt;
				}

				FType val;
				pFieldVal->getVal(val);
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
			virtual bool update (uint64 nOID, IFieldVariant* pFieldVal)
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
					return true;
				}
				else
				return m_tree.remove(nOID);
			}
			virtual bool find(uint64 nOID, IFieldVariant* pFieldVal)
			{
				TBTree::iterator it = m_tree.find(nOID);
				if(it.isNull())
					return false;
				return pFieldVal->setVal(it.value());
			}
			FieldIteratorPtr find(uint64 nOID)
			{
				TBTree::iterator it = m_tree.find(nOID);
				TFieldIterator *pFiledIterator = new TFieldIterator(it);
				return FieldIteratorPtr(pFiledIterator);
			}
			virtual bool commit()
			{
				return m_tree.commit();
			}
		
	};



	template<class _FType, class _TBTree, int FieldDataType>
	class OIDFieldRO : public OIDFieldBase<_TBTree>,  IOIDFiledRO
	{
	public:
		OIDFieldRO( IDBTransactions* pTransactions, CommonLib::alloc_t* pAlloc) :
		  OIDFieldBase(pTransactions, pAlloc)
		  {

		  }
		  ~OIDFieldRO(){}
		  typedef OIDFieldBase<_TBTree> TBase;
		  typedef typename TBase::TBTree TBTree;
		  typedef _FType FType;

		  virtual bool find(int64 nOID, IFieldVariant* pFieldVal)
		  {
			  TBTree::iterator it = m_tree.find(nOID);
			  if(it.isNull())
				  return false;
			  return pFieldVal->setVal(it.value());
		  }
	};

	template<class _FType, int FieldDataType,
		class _TLeafCompressor = embDB::BPLeafNodeMapSimpleCompressorV2<uint64, _FType > 	
	>
	class OIDFieldHandler : public IDBFieldHandler
	{
		public:

			typedef _FType FType;
			typedef embDB::BPInnerNodeSimpleCompressorV2<uint64> TInnerCompressor;
			typedef _TLeafCompressor TLeafCompressor;

			typedef embDB::TBPMapV2<uint64, FType, embDB::comp<uint64>, 
				embDB::IDBTransactions, TInnerCompressor, TLeafCompressor> TBTree;
 
			typedef OIDField<FType, TBTree, FieldDataType> TOIDField;
	
			OIDFieldHandler(CommonLib::alloc_t* pAlloc) : m_pAlloc(pAlloc)
			{

			}
			~OIDFieldHandler()
			{

			}
			virtual bool lock()
			{
				return true;
			}
			virtual bool unlock()
			{
				return true;
			}

			eDataTypes getType() const
			{
				return (eDataTypes)m_fi.m_nFieldDataType;
			}
			const CommonLib::str_t& getName() const
			{
				return m_fi.m_sFieldName;
			}

			virtual sFieldInfo* getFieldInfoType()
			{
				return &m_fi;
			}
			virtual void setFieldInfoType(sFieldInfo& fi)
			{
				m_fi = fi;
			}
			virtual bool save(int64 nAddr, IDBTransactions *pTran)
			{
				//m_nFieldInfoPage = nAddr;
				FilePagePtr pPage(pTran->getFilePage(nAddr));
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

				TOIDField field(pTran, m_pAlloc);
				field.init(m_nBTreeRootPage);
				return field.save();
			}
			virtual bool load(int64 nAddr, IDBStorage *pStorage)
			{
				return true;
			}

			virtual IOIDFiled* getOIDField(IDBTransactions* pTransactions, IDBStorage *pStorage)
			{

				TOIDField * pField = new  TOIDField(pTransactions, m_pAlloc);
				pField->load(m_fi.m_nFieldPage);
				return pField;	
			}

		
			virtual bool release(IOIDFiled* pField)
			{
				TOIDField* pOIDField = (TOIDField*)pField;

				TOIDField::TBTree *pBTree = pOIDField->getBTree();

				delete pField;
				return true;
			}

			bool isCanBeRemoving()
			{
				return true;
			}
		private:
			sFieldInfo m_fi;
			CommonLib::alloc_t* m_pAlloc;

	};

	/*template<class _FType, int FieldDataType>
	class OIDFieldCounter : public OIDField<_FType, FieldDataType>
	{
		public:
	};*/

	typedef OIDFieldHandler<int64, ftInteger64> TOIDFieldINT64;
	typedef OIDFieldHandler<uint64, ftUIInteger64> TOIDFieldUINT64;
	typedef OIDFieldHandler<int32, ftInteger32> TOIDFieldINT32;
	typedef OIDFieldHandler<uint32, ftUIInteger32> TOIDFieldUINT32;
	typedef OIDFieldHandler<int16, ftInteger16> TOIDFieldINT16;
	typedef OIDFieldHandler<uint16, ftUInteger16> TOIDFieldUINT16;
	typedef OIDFieldHandler<int32, ftUInteger8> TOIDFieldINT8;
	typedef OIDFieldHandler<uint32, ftInteger8> TOIDFieldUINT8;
	typedef OIDFieldHandler<double, ftDouble> TOIDFieldDouble;
	typedef OIDFieldHandler<float, ftFloat> TOIDFieldFloat;
    //typedef OIDFieldCounter<int32, ftInteger32> TOIDFieldINT32Counter;
	 
}
#endif