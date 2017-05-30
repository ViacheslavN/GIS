#ifndef _EMBEDDED_DATABASE_FIELD_VALUE_H_
#define _EMBEDDED_DATABASE_FIELD_VALUE_H_
#include "embDBInternal.h"
#include "Key.h"
#include "BaseBPMapv2.h"
//#include "VariantField.h"
#include "DBFieldInfo.h"
#include "DB/Database.h"
#include "DBMagicSymbol.h"
//#include "BaseBPTreeRO.h"
#include "FieldIteratorBase.h"
#include "BaseInnerNodeDIffCompress.h"
#include "BaseLeafNodeCompDiff.h"
#include "BaseInnerNodeDIffCompress.h"
#include "BaseInnerNodeDIffCompress2.h"
#include "BaseValueDiffCompressor.h"
#include "utils/compress/SignedNumLenDiffCompress.h"
#include "BaseLeafNodeCompDiff2.h"
#include "BaseValueCompressor.h"
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

	//TBaseBPlusTree(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, uint32 nChacheSize, bool bMulti = false) :
	
	template<class Type>
	class TVarConvertor
	{
		public:
			void convert(CommonLib::CVariant *pVar, const Type& value)
			{
				pVar->setVal<Type>(value);
			}
	};



	template<class FType, class _TBTree, class TFieldIterator, class TConverTypeToVar>
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
		ValueFieldBase( IDBFieldHolder* pFieldHolder, IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, uint32 nPageSize, uint32 nBTreeChacheSize) :
			  m_pDBTransactions(pTransactions),
			  m_tree(-1, pTransactions, pAlloc, nBTreeChacheSize, nPageSize), 
			  m_nBTreeInfoPage(-1), m_pAlloc(pAlloc), m_pFieldHolder(pFieldHolder)
			  {


				  m_tree.SetMinSplit(true);
			  }

			typedef _TBTree TBTree;
			typedef typename TBTree::iterator iterator;
			typedef typename TBTree::TComp TComp;
			typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
			typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;
			typedef typename TBTree::TValue TValue;

			~ValueFieldBase()
			{

			}
			
			
			virtual bool load(int64 nRootBPTreeField)
			{
				m_nBTreeInfoPage = nRootBPTreeField;
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

			virtual bool removeWithIndex(int64 nOID)
			{ 
				assert(m_pIndex.get());
				typename  TBTree::iterator it = m_tree.find(nOID);
				if (it.isNull())
					return false;

				CommonLib::CVariant var;
				//var.setVal<FType>(it.value());

				m_ConvertTypeToVar.convert(&var, it.value());

				m_pIndex->remove(&var);
				return	m_tree.remove(it);
			}
			virtual bool remove(int64 nOID)
			{
		 
				if(m_pIndex.get())
				{
					return removeWithIndex(nOID);
				}
				else 
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

				if (!CheckUnique(pVariant))
					return false; //TO DO log

				bool bRet = insertImpl(nOID, pVariant->Get<FType>(), pFromIter, pRetIter);
				if (bRet  && (m_pIndex.get() || m_pUniqueCheck.get()))
				{
					if (m_pIndex.get())
						m_pIndex->insert(pVariant, nOID);

					if (m_pUniqueCheck.get())
						m_pUniqueCheck->insert(pVariant);
				}
				return bRet;
			}
			virtual int64 insert (CommonLib::CVariant* pVariant, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL)
			{
				if (!CheckUnique(pVariant))
					return false; //TO DO log

				int64 nOID =  insertImpl(pVariant->Get<FType>(), pFromIter, pRetIter);
				if(nOID != 0 && (m_pIndex.get() || m_pUniqueCheck.get()))
				{
					if(m_pIndex.get())
						m_pIndex->insert(pVariant, nOID);

					if (m_pUniqueCheck.get())
						m_pUniqueCheck->insert(pVariant);
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
				TFieldIterator *pFiledIterator = new TFieldIterator(it, this); //TO DO reuse
				return IFieldIteratorPtr(pFiledIterator);
			}

			void find(int64 nOID, IFieldIteratorPtr& retPtr, IFieldIterator* pFromIter = NULL)
			{

				iterator *pFromIterator = NULL;
				if(pFromIter)
				{
					//TFieldIterator *pFieldIterator = dynamic_cast<TFieldIterator*>(pFromIter);
					//assert(pFieldIterator);
					TFieldIterator *pFieldIterator = (TFieldIterator*)pFromIter;
					pFromIterator = &pFieldIterator->m_ParentIt;
				}

				typename  TBTree::iterator it = m_tree.find(nOID, pFromIterator, true);
				if(!retPtr.get())
				{
					retPtr =  new TFieldIterator(it, this);  
				}
				else
				{
					TFieldIterator *pFieldIterator =(TFieldIterator*)retPtr.get();
					pFieldIterator->setParentIt(it);
				}
				 
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

			virtual IDBFieldHolderPtr GetFieldHolder() const
			{
				return m_pFieldHolder;
			}

			virtual IFieldStatisticPtr GetStatistic()
			{
				return m_pFieldStatistic;
			}
			virtual void SetStatistic(IFieldStatistic *pStatistic)
			{
				m_pFieldStatistic = pStatistic;
			}
			virtual IUniqueCheckPtr GetUniqueCheck()
			{
				return m_pUniqueCheck;
			}
			virtual void SetUniqueCheck(IUniqueCheck *pUniqueCheck)
			{
				m_pUniqueCheck = pUniqueCheck;
			}
 

			bool CheckUnique(const CommonLib::CVariant *pVar)
			{
				if (!m_pFieldHolder->GetIsUnique())
					return true;
				if (m_pUniqueCheck.get())
					return m_pUniqueCheck->IsExsist(pVar);
				if(m_pIndex.get())
					return m_pIndex->IsExsist(pVar);
				return false; //TO DO error
			}
	 protected:
		IDBTransaction* m_pDBTransactions;
		TBTree m_tree;
		int64 m_nBTreeInfoPage;
		CommonLib::alloc_t* m_pAlloc;
		IndexFiledPtr m_pIndex;
		IFieldStatisticPtr m_pFieldStatistic;
		IUniqueCheckPtr m_pUniqueCheck;
		IDBFieldHolderPtr m_pFieldHolder;
		const sFieldInfo *m_pFieldInfo;
		TConverTypeToVar m_ConvertTypeToVar;
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
	class ValueField : public ValueFieldBase<_FType, _TBTree, _FieldIterator,  TVarConvertor<_FType> >
	{
		public:
	 
	
			typedef _FieldIterator TFieldIterator;
			typedef ValueFieldBase<_FType, _TBTree, TFieldIterator, TVarConvertor<_FType> > TBase;

			ValueField(IDBFieldHolder* pFieldHolder,  IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, uint32 nPageSize, uint32 nBTreeChacheSize) :
			  TBase(pFieldHolder, pTransactions, pAlloc, nPageSize, nBTreeChacheSize)
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
			virtual bool removeWithIndex(int64 nOID)
			{ 
				iterator it = this->m_tree.find(nOID);
				if(it.isNull())
					return false;

				CommonLib::CVariant var(it.value());
				if(m_pIndex->GetType() == itUnique)
					m_pIndex->remove(&var);
				else if(m_pIndex->GetType() == itMultiRegular)
				{
					MultiIndexFiled *pMulitIndex = dynamic_cast<MultiIndexFiled *>(m_pIndex.get());
					assert(pMulitIndex);
					pMulitIndex->remove(&var, nOID);
				}
				return this->m_tree.remove(it);
			}
			
		
	};
	
	template<class _FType, int FieldDataType,
		class _TLeafCompressor = embDB::TBaseLeafNodeDiffComp<int64, _FType, embDB::IDBTransaction, embDB::OIDCompressor> 	
	>
	class ValueFieldHolder :  CDBFieldHolderBase<IDBFieldHolder>
	{
		public:

			typedef _FType FType;

			typedef TBaseValueDiffCompress<int64, int64, SignedDiffNumLenCompressor64i> TInnerLinkCompress;
			typedef embDB::TBPBaseInnerNodeDiffCompressor2<int64, embDB::OIDCompressor, TInnerLinkCompress>  TInnerCompressor;

			//typedef embDB::TBPBaseInnerNodeDiffCompressor<int64, embDB::OIDCompressor, embDB::InnerLinkCompress>	TInnerCompressor;
			typedef _TLeafCompressor TLeafCompressor;



			typedef embDB::TBPMapV2<int64, FType, embDB::comp<int64>, 
			embDB::IDBTransaction, TInnerCompressor, TLeafCompressor> TBTree;
 
			typedef typename TBTree::iterator  iterator;
			typedef FieldIterator<TBTree> TFieldIterator;
			typedef ValueField<FType, TBTree, TFieldIterator> TField;

			typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
			typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;
	
			ValueFieldHolder(CommonLib::alloc_t* pAlloc, const SFieldProp* pFieldProp, int64 nPageAdd) : CDBFieldHolderBase<IDBFieldHolder>(pAlloc, pFieldProp, nPageAdd)
			{}
			~ValueFieldHolder()
			{}
			
			virtual bool save(CommonLib::IWriteStream* pStream,  IDBTransaction *pTran)
			{

				TLeafCompressorParams leafCompParams;
				TInnerCompressorParams innerCompParams;


				leafCompParams.m_compressType = m_CompressType;
				leafCompParams.m_bCalcOnlineSize = m_bOnlineCalcCompSize;
				leafCompParams.m_nErrorCalc		  = m_nCompCalcError;

				innerCompParams.m_compressType = m_CompressType;
				innerCompParams.m_bCalcOnlineSize = m_bOnlineCalcCompSize;
				innerCompParams.m_nErrorCalc		  = m_nCompCalcError;
				

				return CDBFieldHolderBase::save<TField, TInnerCompressorParams, TLeafCompressorParams>(pStream, pTran, m_pAlloc,&innerCompParams, &leafCompParams);
			}

			virtual bool load(CommonLib::IReadStream* pStream,  IDBTransaction *pTran)
			{
				return true;
			}


			virtual IValueFieldPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage)
			{
				TField * pField = new  TField(this, pTransactions, m_pAlloc,m_nPageSize, m_nBTreeChacheSize);
				pField->load(m_nFieldInfoPage);
				if(m_pIndexHolder.get())
				{
					pField->SetIndex(m_pIndexHolder->getIndex(pTransactions, pStorage).get());
				}
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

	typedef  embDB::TBaseLeafNodeDiffComp2<int64, int32, embDB::IDBTransaction, embDB::OIDCompressor, TBaseValueDiffCompress<int32, int32, SignedDiffNumLenCompressor32i> > TInteger32Compress;
    typedef  embDB::TBaseLeafNodeDiffComp2<int64, uint32, embDB::IDBTransaction, embDB::OIDCompressor, TBaseValueDiffCompress<uint32, int32, SignedDiffNumLenCompressor32u> > TUInteger32Compress;
	
	typedef  embDB::TBaseLeafNodeDiffComp2<int64, int64, embDB::IDBTransaction, embDB::OIDCompressor, TBaseValueDiffCompress<int64, int32, SignedDiffNumLenCompressor64i> >TInteger64Compress;
	typedef  embDB::TBaseLeafNodeDiffComp2<int64, uint64, embDB::IDBTransaction, embDB::OIDCompressor, TBaseValueDiffCompress<uint64, int64, SignedDiffNumLenCompressor64u> >TUInteger64Compress;

	typedef  embDB::TBaseLeafNodeDiffComp2<int64, int16, embDB::IDBTransaction, embDB::OIDCompressor, TBaseValueDiffCompress<int16, int16, SignedDiffNumLenCompressor16i> > TInteger16Compress;
	typedef  embDB::TBaseLeafNodeDiffComp2<int64, uint16, embDB::IDBTransaction, embDB::OIDCompressor, TBaseValueDiffCompress<uint16, int16, SignedDiffNumLenCompressor16u> > TUInteger16Compress;



	//typedef  embDB::TBaseLeafNodeDiffComp<int64, int32, embDB::IDBTransaction, embDB::OIDCompressor,  TBaseValueCompress<int32,UnsignedNumLenCompressor32i > > TInteger32Compress;
	//typedef  embDB::TBaseLeafNodeDiffComp<int64, int64, embDB::IDBTransaction, embDB::OIDCompressor,  TBaseValueCompress<int64, UnsignedNumLenCompressor64i> >TInteger64Compress;

	typedef ValueFieldHolder<int64, dtInteger64, TInteger64Compress> TValFieldINT64;
	typedef ValueFieldHolder<uint64,dtUInteger64, TUInteger64Compress> TValFieldUINT64;
	typedef ValueFieldHolder<int32, dtInteger32, TInteger32Compress> TValFieldINT32;
	typedef ValueFieldHolder<uint32,dtUInteger32, TUInteger32Compress> TValFieldUINT32;
	typedef ValueFieldHolder<int16, dtInteger16, TInteger16Compress> TValFieldINT16;
	typedef ValueFieldHolder<uint16,dtUInteger16, TUInteger16Compress> TValFieldUINT16;
	typedef ValueFieldHolder<int32, dtUInteger8> TValFieldINT8;
	typedef ValueFieldHolder<uint32,dtInteger8> TValFieldUINT8;
	typedef ValueFieldHolder<double,dtDouble> TValFieldDouble;
	typedef ValueFieldHolder<float, dtFloat> TValFieldFloat;
 
}
#endif