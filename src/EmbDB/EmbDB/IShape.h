#ifndef _EMBEDDED_DATABASE_I_SPHAPE_H_
#define _EMBEDDED_DATABASE_I_SPHAPE_H_

#include "IField.h"
namespace embDB
{

	class IShape
	{
	public:
		IShape(){}
		virtual ~IShape(){}
		virtual eSpatialCoordinatesType getCoordType() const = 0;
		virtual eShapeType getShapeType() const = 0;
		virtual uint32 getPointCount() const = 0;
		virtual ePointType getPointType() const = 0;

		virtual bool getPoints16(uint16 **ppPoints, uint32 nCnt) const = 0;
		virtual bool getPoints32(uint32 **ppPoints, uint32 nCnt) const = 0;
		virtual bool getPoints64(uint64 **ppPoints, uint32 nCnt) const = 0;
		virtual bool getPoints(double **ppPoints, uint32 nCnt) const = 0;

		virtual bool setPoints16(uint16 *ppPoints, uint32 nCnt) = 0;
		virtual bool setPoints32(uint32 *ppPoints, uint32 nCnt) = 0;
		virtual bool setPoints64(uint64 *ppPoints, uint32 nCnt) = 0;
		virtual bool setPoints(double *ppPoints, uint32 nCnt) const = 0;
	};
}
#endif