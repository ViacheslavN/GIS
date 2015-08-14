#ifndef _DATASET_LITE_I_SPATIAL_DATASET_H_
#define _DATASET_LITE_I_SPATIAL_DATASET_H_
#include "CommonLibrary/str_t.h"
#include "CommonLibrary/IGeoShape.h"
#include "CommonLibrary/IRefCnt.h"
#include "CommonLibrary/Variant.h"

#include "../EmbDB/ITransactions.h"
#include "../EmbDB/ICursor.h"
#include "../EmbDB/ISpatialQuery.h"
#include "ShapeLib/shapefil.h"
namespace DatasetLite
{

	struct ISpatialLiteDataset;
	struct IShapeFileIndex;
	struct IShapeCursor;


	COMMON_LIB_REFPTR_TYPEDEF(ISpatialLiteDataset);
	COMMON_LIB_REFPTR_TYPEDEF(IShapeFileIndex);
	COMMON_LIB_REFPTR_TYPEDEF(IShapeCursor);

	
	struct ISpatialLiteDataset : public CommonLib::AutoRefCounter
	{
	public:
		ISpatialLiteDataset(){}
		virtual ~ISpatialLiteDataset(){}
		virtual bool close()  = 0;
		virtual embDB::ITransactions* startTransaction(embDB::eTransactionsType trType) = 0;
		virtual bool closeTransaction(embDB::ITransactions* ) = 0;
		virtual embDB::ICursor * spatialQuery();



		static ISpatialLiteDatasetPtr open(const CommonLib::str_t& sDbName);
		static ISpatialLiteDatasetPtr create(const CommonLib::str_t& sDbName, size_t nPageSize, embDB::eSpatialCoordinatesType, CommonLib::bbox& extent,
			embDB::eDataTypes spatialType, embDB::eDataTypes valueType);

	};



	struct IShapeCursor  : public CommonLib::AutoRefCounter
	{
		IShapeCursor(){}
		virtual ~IShapeCursor(){}
		virtual bool next() = 0;
		virtual int row() const = 0;
		virtual bool IsEnd() const =0;
	 
	};

	struct IShapeFileIndex  : public CommonLib::AutoRefCounter
	{
	public:
		IShapeFileIndex(){}
		virtual ~IShapeFileIndex(){}
		virtual const CommonLib::bbox& extent() const = 0;
		virtual IShapeCursorPtr spatialQuery(const CommonLib::bbox& extent) = 0;

 		static IShapeFileIndexPtr open(const CommonLib::str_t& sDbName);
		static IShapeFileIndexPtr create(const CommonLib::str_t& sDbName, size_t nPageSize, embDB::eSpatialCoordinatesType type,
			embDB::eShapeType spatialType,const CommonLib::str_t& sShapeFileName);

	};


	struct IShapeFileIndexPoint : IShapeFileIndex
	{
		IShapeFileIndexPoint(){}
		virtual ~IShapeFileIndexPoint(){}

		bool insert(double dX, double dY, int nShapeId);
		
	};

	struct IShapeFileIndexRect : IShapeFileIndex
	{
		IShapeFileIndexRect(){}
		virtual ~IShapeFileIndexRect(){}

		bool insert(const CommonLib::bbox& extent, int nShapeId);
	};


	
}
#endif