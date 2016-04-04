#ifndef _DATASET_LITE_I_SPATIAL_DATASET_H_
#define _DATASET_LITE_I_SPATIAL_DATASET_H_
#include "CommonLibrary/String.h"
#include "CommonLibrary/IGeoShape.h"
#include "CommonLibrary/IRefCnt.h"
#include "CommonLibrary/Variant.h"

#include "../EmbDB/embDB.h"
#include "ShapeLib/shapefil.h"

#include "../GisEngine/GisGeometry/Geometry.h"
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
		virtual embDB::ITransaction* startTransaction(embDB::eTransactionType trType) = 0;
		virtual bool closeTransaction(embDB::ITransaction* ) = 0;
		virtual embDB::ICursor * spatialQuery();



		static ISpatialLiteDatasetPtr open(const CommonLib::CString& sDbName);
		static ISpatialLiteDatasetPtr create(const CommonLib::CString& sDbName, size_t nPageSize, embDB::eSpatialCoordinatesUnits uints, CommonLib::bbox& extent,
			embDB::eDataTypes spatialType, embDB::eDataTypes valueType);

	};



	struct IShapeCursor  : public CommonLib::AutoRefCounter
	{
		IShapeCursor(){}
		virtual ~IShapeCursor(){}
		virtual bool next() = 0;
		virtual int row() const = 0;
		virtual bool IsEnd() const =0;
		virtual CommonLib::bbox extent() const =0;
	 
	};

	struct IShapeFileIndex  : public CommonLib::AutoRefCounter
	{
	public:
		IShapeFileIndex(){}
		virtual ~IShapeFileIndex(){}
		virtual const CommonLib::bbox& extent() const = 0;
		virtual IShapeCursorPtr spatialQuery(const CommonLib::bbox& extent) = 0;
		virtual bool insert(ShapeLib::SHPObject* pObject, int nRow = -1) = 0;
		virtual bool commit() = 0;

 		static IShapeFileIndexPtr open(const CommonLib::CString& sDbName, int32 nShapeType);
		static IShapeFileIndexPtr create(const CommonLib::CString& sDbName, size_t nPageSize,
			const CommonLib::CString& sShapeFileName, GisEngine::GisCommon::Units units = GisEngine::GisCommon::UnitsUnknown, 
			double dOffsetX = 0, double dOffsetY = 0, double dScaleX = 0, double dScaleY = 0, CommonLib::bbox bbox = CommonLib::bbox());

	};


	struct IShapeFileIndexPoint : IShapeFileIndex
	{
		IShapeFileIndexPoint(){}
		virtual ~IShapeFileIndexPoint(){}

		virtual bool insert(double dX, double dY, int nShapeId) = 0;
		
	};

	struct IShapeFileIndexRect : IShapeFileIndex
	{
		IShapeFileIndexRect(){}
		virtual ~IShapeFileIndexRect(){}

		virtual bool insert(const CommonLib::bbox& extent, int nShapeId) = 0;
	};


	
}
#endif