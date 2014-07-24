#ifndef _DATASET_LITE_I_SPATIAL_DATASET_H_
#define _DATASET_LITE_I_SPATIAL_DATASET_H_
#include "CommonLibrary/str_t.h"
#include "../../EmbDB/ITransactions.h"
#include "../../EmbDB/ICursor.h"
#include "../../EmbDB/ISpatialQuery.h"
namespace DatasetLite
{

	class ISpatialDataset
	{
	public:
		ISpatialDataset(){}
		virtual ~ISpatialDataset(){}
		virtual bool open(const CommonLib::str_t& sDbName)  = 0;
		virtual bool create(const CommonLib::str_t& sDbName, size_t nPageSize)  = 0;
		virtual bool close()  = 0;
		virtual embDB::ITransactions* startTransaction(embDB::eTransactionsType trType) = 0;
		virtual bool closeTransaction(embDB::ITransactions* ) = 0;
		virtual embDB::ICursor * spatialQuery();

	};
}
#endif