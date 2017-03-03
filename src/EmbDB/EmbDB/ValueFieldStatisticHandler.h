#pragma once
#include "embDBInternal.h"
namespace embDB
{
	struct IFieldStatisticHandler : public CommonLib::AutoRefCounter
	{
	public:
		IFieldStatisticHandler() {}
		virtual ~IFieldStatisticHandler() {}

		virtual bool save(CommonLib::IWriteStream* pStream, IDBTransaction *pTran);
		virtual bool load(CommonLib::IReadStream* pStream, IDBStorage *pStorage);


		virtual IFieldStatisticPtr getFieldStatistic(IDBTransaction* pTransactions, IDBStorage *pStorage);
		virtual bool release(IFieldStatisticPtr* pInxex);

		virtual bool Update(IDBTransaction* pTransactions, IDBStorage *pStorage);

		virtual bool lock() = 0;
		virtual bool unlock() = 0;
		virtual bool isCanBeRemoving() = 0;

	};
}
