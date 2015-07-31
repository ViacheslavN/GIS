#ifndef _DATASET_LITE_I_SPATIAL_DATASET_H_
#define _DATASET_LITE_I_SPATIAL_DATASET_H_
#include "CommonLibrary/str_t.h"
#include "CommonLibrary/IGeoShape.h"
#include "CommonLibrary/Variant.h"
#include "CommonLibrary/IRefCnt.h"
#include "../../EmbDB/ITransactions.h"
#include "../../EmbDB/ICursor.h"
#include "../../EmbDB/ISpatialQuery.h"
namespace DatasetLite
{

	struct ISpatialLiteDataset;



	typedef CommonLib::IRefCntPtr<ISpatialLiteDataset> ISpatialLiteDatasetPtr;

	ISpatialLiteDatasetPtr open(const CommonLib::str_t& sDbName);
	ISpatialLiteDatasetPtr create(const CommonLib::str_t& sDbName, size_t nPageSize, CommonLib::bbox& extent,
		CommonLib::eDataTypes spatialType, CommonLib::eDataTypes valueType);


	class ISpatialLiteDataset
	{
	public:
		ISpatialLiteDataset(){}
		virtual ~ISpatialLiteDataset(){}
		virtual bool close()  = 0;
		virtual embDB::ITransactions* startTransaction(embDB::eTransactionsType trType) = 0;
		virtual bool closeTransaction(embDB::ITransactions* ) = 0;
		virtual embDB::ICursor * spatialQuery();

	};




}
#endif