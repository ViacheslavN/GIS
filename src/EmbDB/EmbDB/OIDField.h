#ifndef _EMBEDDED_DATABASE_FIELD_OID_H_
#define _EMBEDDED_DATABASE_FIELD_OID_H_
#include "Key.h"
#include "BaseBPMap.h"
#include "VariantField.h"
#include "DBField.h"
#include "IDBTransactions.h"
#include "Database.h"
#include "DBMagicSymbol.h"
#include "IDBStorage.h"
#include "IDBTransactions.h"
#include "BaseBPTreeRO.h"
namespace embDB
{



	class IOIDFiled  
	{
	public:
		IOIDFiled() {}
		virtual ~IOIDFiled() {}
		virtual bool insert (uint64 nOID, IFieldVariant* pFieldVal) = 0;
		virtual uint64 insert (IFieldVariant* pFieldVal) = 0;
		virtual bool update (uint64 nOID, IFieldVariant* pFieldVal) = 0;
		virtual bool remove (uint64 nOID) = 0;
		virtual bool find(uint64 nOID, IFieldVariant* pFieldVal) = 0;
		virtual bool commit() = 0;
	};



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
				CFilePage * pPage = m_pDBTransactions->getFilePage(nAddr);
				if(!pPage)
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
					return true;
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
		sFieldInfo m_fi;
		int64 m_nBTreeRootPage;
	};
	
	
	
	
	template<class _FType, class _TBTree, int FieldDataType>
	class OIDField : public OIDFieldBase<_TBTree>, public  IOIDFiled
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

			OIDField( IDBTransactions* pTransactions, CommonLib::alloc_t* pAlloc) :
			  OIDFieldBase(pTransactions, pAlloc)
			{

			}
			~OIDField(){}
			typedef OIDFieldBase<_TBTree> TBase;
			typedef typename TBase::TBTree TBTree;
			typedef _FType FType;
		
			virtual bool insert (uint64 nOID, IFieldVariant* pFieldVal)
			{
				FType val;
				pFieldVal->getVal(val);
				return m_tree.insert(nOID, val);

			}
			virtual uint64 insert (IFieldVariant* pFieldVal)
			{
				FType val;
				pFieldVal->getVal(val);
				uint64 nOID;
				if(!m_tree.insertLast(TOIDIncFunctor(), val, &nOID))
					return  0;
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
			virtual bool find(uint64 nOID, IFieldVariant* pFieldVal)
			{
				TBTree::iterator it = m_tree.find(nOID);
				if(it.isNull())
					return false;
				return pFieldVal->setVal(it.value());
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

	template<class _FType, int FieldDataType>
	class OIDFieldHandler : public IDBFieldHandler
	{
		public:

			typedef _FType FType;
			typedef embDB::BPInnerNodeSimpleCompressor<uint64, int64, embDB::comp<uint64> > TInnerCompressor;
			typedef embDB::BPLeafNodeSimpleCompressor<uint64, FType, embDB::comp<uint64> > TLeafCompressor;

			typedef embDB::TBPlusTreeMap<uint64, FType, int64, embDB::comp<uint64>, 
				embDB::IDBTransactions/*, TInnerCompressor, TLeafCompressor*/ > TBTree;

			typedef embDB::TBaseBPlusTreeRO<uint64, FType, int64, embDB::comp<uint64>, 
				embDB::IDBTransactions, TInnerCompressor, TLeafCompressor > TBTreeRO;


			typedef OIDField<FType, TBTree, FieldDataType> TOIDField;
			typedef OIDFieldRO<FType, TBTreeRO, FieldDataType> TOIDFieldRO;
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
				CFilePage * pPage = pTran->getFilePage(nAddr);
				if(!pPage)
					return false;
				CommonLib::FxMemoryWriteStream stream;
				stream.attach(pPage->getRowData(), pPage->getPageSize());
				sFilePageHeader header(stream, FIELD_PAGE, FIELD_INFO_PAGE);
				int64 m_nBTreeRootPage = -1;
					CFilePage *pRootPage = pTran->getNewPage();
				if(!pRootPage)
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

			IOIDFiled* getOIDField(IDBTransactions* pTransactions, IDBStorage *pStorage)
			{

				TOIDField * pField = new  TOIDField(pTransactions, m_pAlloc);
				pField->load(m_fi.m_nFieldPage);
				return pField;	
			}

			IOIDFiled* getOIDFieldRO(IDBTransactions* pTransactions, IDBStorage *pStorage)
			{
				TOIDFieldRO * pFieldRO = new  TOIDFieldRO(pTransactions, m_pAlloc);
				pFieldRO->load(m_fi.m_nFieldPage, pStorage);
				return pFieldRO;
			}

			bool release(IOIDFiled* pField)
			{
				TOIDField* pOIDField = (TOIDField*)pField;

				TOIDField::TBTree *pBTree = pOIDField->getBTree();

				delete pField;
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