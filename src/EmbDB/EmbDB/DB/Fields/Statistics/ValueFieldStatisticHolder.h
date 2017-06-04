#pragma once
#include "embDBInternal.h"
#include "ValueFieldFullStatistic.h"
 
namespace embDB
{
	template<class _TFieldStat>
	class  TFieldStatisticHolder : public IFieldStatisticHolder
	{
	public:

		typedef _TFieldStat TFieldStat;

		TFieldStatisticHolder(CommonLib::alloc_t *pAlloc, const SStatisticInfo& si, bool bCheckCRC) :
			m_si(si), m_pAlloc(pAlloc), m_bCheckCRC(bCheckCRC)
		{}
		virtual ~TFieldStatisticHolder() {}

		virtual bool save(CommonLib::IWriteStream* pStream, IDBTransaction *pTran)
		{

			FilePagePtr pRootPage(pTran->getNewPage(m_si.m_nPageSize));
			if (!pRootPage.get())
				return false;
			m_nRootPage = pRootPage->getAddr();
			pStream->write(m_nRootPage);

			TFieldStat fieldStat(m_pAlloc, pTran, m_si, m_bCheckCRC);
			fieldStat.Init(m_nRootPage, m_bCheckCRC);
			return true;
		}
		virtual bool load(CommonLib::IReadStream* pStream, IDBStorage *pStorage)
		{
			m_nRootPage = pStream->readInt64();
		
			return true;
		}


		virtual IFieldStatisticPtr getFieldStatistic(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{
			
			if (m_pStatistic.get())
			{
				((TFieldStat*)m_pStatistic.get())->SetTransaction(pTransactions); //To DO fix
				return m_pStatistic;
			}

			TFieldStat* pFieldStat = new  TFieldStat(m_pAlloc, pTransactions, m_si, m_bCheckCRC);
			if (!pFieldStat->load(m_nRootPage, m_bCheckCRC))
			{
				delete pFieldStat;
				return IFieldStatisticPtr();
			}

			if (m_si.m_UpdateStat == usManualUpdateStat)
			{
				m_pStatistic = pFieldStat;
				return m_pStatistic;
			}
			else
				return IFieldStatisticPtr(pFieldStat);
		}
		virtual bool release(IFieldStatisticPtr* pFieldStatistic)
		{
			if (m_si.m_UpdateStat == usManualUpdateStat)
			{

			}

			return true;
		}

		virtual bool Update(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{
			return true;
		}

		virtual bool lock()
		{
			return true;
		}
		virtual bool unlock()
		{
			return true;
		}
		virtual bool isCanBeRemoving()
		{
			return true;
		}
	private:
		SStatisticInfo m_si;
		eUpdateStatisticType m_UpdateType;
		CommonLib::alloc_t *m_pAlloc;
		int64 m_nRootPage;
		bool m_bCheckCRC;

		IFieldStatisticPtr m_pStatistic;
	};



	typedef TFieldStatisticHolder<TCharFullStatistic> TFieldStatisticCharHolder;
	typedef TFieldStatisticHolder<TByteFullStatistic> TFieldStatisticByteHolder;
	typedef TFieldStatisticHolder<TUint16FullStatistic> TFieldStatisticUint16Holder;
	typedef TFieldStatisticHolder<TInt16FullStatistic> TFieldStatisticInt16Holder;
	typedef TFieldStatisticHolder<TUint32FullStatistic> TFieldStatisticUint32Holder;
	typedef TFieldStatisticHolder<TInt32FullStatistic> TFieldStatisticInt32Holder;
	typedef TFieldStatisticHolder<TUint64FullStatistic> TFieldStatisticUint64Holder;
	typedef TFieldStatisticHolder<TInt64FullStatistic> TFieldStatisticInt64Holder;


}
