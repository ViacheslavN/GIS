#pragma once
#include "embDBInternal.h"
namespace embDB
{
	class WriteStreamPage;
	class ReadStreamPage;



	template<class Type>
	class CValueFieldStatisticBase : public IFieldStatistic
	{
		public:
			CValueFieldStatisticBase(CommonLib::alloc_t *pAlloc, IDBTransaction* pTransaction, bool bCheckCRC = false) : 
				m_pTransaction(pTransaction), m_nRootPage(-1), m_bCheckCRC(bCheckCRC), m_pAlloc(pAlloc)
			{

			}
			virtual ~CValueFieldStatisticBase()
			{

			}

			virtual bool Init(int64 nRootPage, bool bCheckCRC)
			{
				m_nRootPage = nRootPage;
				FilePagePtr pFieldInfoPage = m_pTransaction->getFilePage(nRootPage, MIN_PAGE_SIZE);
				if (!pFieldInfoPage.get())
					return false; // TO DO Error

				WriteStreamPage stream(m_pTransaction.get(), MIN_PAGE_SIZE, bCheckCRC, FIELD_PAGE, FIELD_INFO_STATISTIC);
				stream.open(pFieldInfoPage);
				stream.write((uint32)GetType());

				if (!InitCustom(&stream))
					return false;  // TO DO Error

				stream.Save();
				return true;
			}
			virtual bool load(int64 nRootPage, bool bCheckCRC)
			{
				m_nRootPage = nRootPage;
				FilePagePtr pFieldInfoPage = m_pTransaction->getFilePage(nRootPage, MIN_PAGE_SIZE);
				if (!pFieldInfoPage.get())
					return false; // TO DO Error

				ReadStreamPage stream(m_pTransaction.get(), MIN_PAGE_SIZE, bCheckCRC, FIELD_PAGE, FIELD_STATISTIC);
				stream.open(pFieldInfoPage);

				if (!LoadCustom(&stream))
					return false;  // TO DO Error

				return true;
			}
 			virtual void AddVarValue(const Type& value) = 0;
			virtual void RemoveVarValue(const Type& value) = 0;
 
			virtual uint32 GetLastUpdateTime() const { return m_nTimeUpdate; }
			virtual uint32 GetLastUpdateDate() const { return m_nDateUpdate; }
			

		protected:
			virtual bool InitCustom(WriteStreamPage* stream)
			{
				return true;
			}
			virtual bool LoadCustom(ReadStreamPage* stream)
			{
				return true;
			}
		protected:
			IDBTransactionPtr m_pTransaction;
			CommonLib::alloc_t *m_pAlloc;
			int64 m_nRootPage;
			uint32 m_nTimeUpdate;
			uint32 m_nDateUpdate;
			bool m_bCheckCRC;
			
	};

	

}