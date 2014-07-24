#ifndef _EMBEDDED_DATABASE_SPATIAL_VARIANT_FIELD_H_
#define _EMBEDDED_DATABASE_SPATIAL_VARIANT_FIELD_H_
#include "VariantField.h"
#include "SpatialKey.h"
namespace embDB
{

	template <class _TCoordType, int FieldType>
	class PointFieldVariant : public IBaseFieldVariant<TPoint2D<_TCoordType> , FieldType>
	{
	public:
		typedef _TCoordType TCoordType ;
		typedef IBaseFieldVariant<TPoint2D<TCoordType> , FieldType> TBase;
		typedef TPoint2D<TCoordType> TPoint;
		PointFieldVariant(){};
		PointFieldVariant(TCoordType x, TCoordType y) 
		{
			m_bIsEmpty = false;
			m_Value.m_x = x;
			m_Value.m_y = y;
		}

	};

	template <class _TCoordType, int FieldType>
	class RectFieldVariant : public IBaseFieldVariant<TRect2D<_TCoordType> , FieldType>
	{
	public:
		typedef _TCoordType TCoordType ;
		typedef IBaseFieldVariant<TRect2D<TCoordType> , FieldType> TBase;
		typedef TRect2D<TCoordType> TPoint;
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




	typedef PointFieldVariant<uint16, ftPoint16> TVariantPointField16;
	typedef PointFieldVariant<uint32, ftPoint16> TVariantPointField32;
	typedef PointFieldVariant<uint64, ftPoint64> TVariantPointField64;

	typedef RectFieldVariant<uint16, ftShape16> TVariantShapeField16;
	typedef RectFieldVariant<uint32, ftShape32> TVariantShapeField32;
	typedef RectFieldVariant<uint64, ftShape64> TVariantShapeField64;
}

#endif