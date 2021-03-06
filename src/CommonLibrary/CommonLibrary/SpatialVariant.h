#ifndef _EMBEDDED_DATABASE_SPATIAL_VARIANT_FIELD_H_
#define _EMBEDDED_DATABASE_SPATIAL_VARIANT_FIELD_H_
#include "BaseVariant.h"
#include "SpatialKey.h"
namespace CommonLib
{

	template <class _TCoordType, int FieldType>
	class PointFieldVariant : public BaseVariant<CommonLib::TPoint2D<_TCoordType> , FieldType>
	{
	public:
		typedef _TCoordType TCoordType ;
		typedef CommonLib::BaseVariant<CommonLib::TPoint2D<TCoordType> , FieldType> TBase;
		typedef CommonLib::TPoint2D<TCoordType> TPoint;
		PointFieldVariant(){};
		PointFieldVariant(TCoordType x, TCoordType y) 
		{
			m_bIsEmpty = false;
			m_Value.m_x = x;
			m_Value.m_y = y;
		}

	};

	template <class _TCoordType, int FieldType>
	class RectFieldVariant : public BaseVariant<CommonLib::TRect2D<_TCoordType> , FieldType>
	{
	public:
		typedef _TCoordType TCoordType ;
		typedef BaseVariant<CommonLib::TRect2D<TCoordType> , FieldType> TBase;
		typedef CommonLib::TRect2D<TCoordType> TPoint;
		RectFieldVariant(){};
		RectFieldVariant(TCoordType xMin, TCoordType yMin, TCoordType xMax, TCoordType yMax) 
		{
			m_bIsEmpty = false;
			m_Value.m_minX = xMin;
			m_Value.m_minY = yMin;
			m_Value.m_maxX = xMax;
			m_Value.m_maxY = yMax;
		}

	};




	typedef PointFieldVariant<uint16, CommonLib::dtPoint16> TVariantPointField16;
	typedef PointFieldVariant<uint32, CommonLib::dtPoint16> TVariantPointField32;
	typedef PointFieldVariant<uint64, CommonLib::dtPoint64> TVariantPointField64;

	typedef RectFieldVariant<uint16, CommonLib::dtShape16> TVariantShapeField16;
	typedef RectFieldVariant<uint32, CommonLib::dtShape32> TVariantShapeField32;
	typedef RectFieldVariant<uint64, CommonLib::dtShape64> TVariantShapeField64;
}

#endif