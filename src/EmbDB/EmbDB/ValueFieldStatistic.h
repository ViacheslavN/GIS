#pragma once
#include "embDBInternal.h"
namespace embDB
{
	class WriteStreamPage;
	class ReadStreamPage;



	template<class Type>
	class CValueFieldStatisticBase
	{
		public:
			CValueFieldStatisticBase(IDBTransaction* pTransaction) : m_pTransaction(pTransaction), m_nRootPage(-1)
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

			virtual bool IsValid() const = 0;
			virtual bool save() = 0;

			virtual eStatisticType GetType() const = 0;

			virtual void AddVarValue(const CommonLib::CVariant& value) = 0;
			virtual void RemoveVarValue(const CommonLib::CVariant& value) = 0;

			virtual void AddVarValue(const Type& value) = 0;
			virtual void RemoveVarValue(const Type& value) = 0;


			uint64 GetCount(const Type& value) const = 0;
			uint64 GetCount(const CommonLib::CVariant& value) const = 0;

			

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
			int64 m_nRootPage;
			
	};

	

}