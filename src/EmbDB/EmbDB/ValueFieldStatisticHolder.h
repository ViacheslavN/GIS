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

		TFieldStatisticHolder(eStatisticType typeStat, eUpdateStatisticType updateType) : m_typeStat(typeStat), m_UpdateType(updateType)
		{}
		virtual ~TFieldStatisticHolder() {}

		virtual bool save(CommonLib::IWriteStream* pStream, IDBTransaction *pTran)
		{
			m_typeStat = typeStat;

			return true;
		}
		virtual bool load(CommonLib::IReadStream* pStream, IDBStorage *pStorage)
		{
			return true;
		}


		virtual IFieldStatisticPtr getFieldStatistic(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{

		}
		virtual bool release(IFieldStatisticPtr* pFieldStatistic)
		{

		}

		virtual bool Update(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{

		}

		virtual bool lock()
		{

		}
		virtual bool unlock()
		{

		}
		virtual bool isCanBeRemoving()
		{

		}
	private:
		IFieldStatisticPtr m_pOfflineFieldStatistic;
		eStatisticType m_typeStat;
		eUpdateStatisticType m_UpdateType;

	};



	typedef TFieldStatisticHolder<TCharFullStatistic> TFieldStatisticCharHolder
	typedef TFieldStatisticHolder<TByteFullStatistic> TFieldStatisticCharHolder
	typedef TFieldStatisticHolder<TUint16FullStatistic> TFieldStatisticCharHolder
	typedef TFieldStatisticHolder<TInt16FullStatistic> TFieldStatisticCharHolder
 


}
