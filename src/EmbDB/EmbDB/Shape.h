#ifndef _EMBEDDED_DATABASE_SPHAPE_IMP_H_
#define _EMBEDDED_DATABASE_SPHAPE_IMP_H_

#include "IShape.h"
namespace embDB
{

	class Shape : public  IShape
	{
	public:
		Shape(){}
		virtual ~Shape(){}
		virtual eSpatialCoordinatesType getCoordType() const;
		virtual eShapeType getShapeType() const;
		virtual uint32 getPointCount() const;
		virtual ePointType getPointType() const;

		virtual bool getPoints16(uint16 **ppPoints, uint32 nCnt) const;
		virtual bool getPoints32(uint32 **ppPoints, uint32 nCnt) const;
		virtual bool getPoints64(uint64 **ppPoints, uint32 nCnt) const;
		virtual bool getPoints(double **ppPoints, uint32 nCnt) const;

		virtual bool setPoints16(uint16 *ppPoints, uint32 nCnt);
		virtual bool setPoints32(uint32 *ppPoints, uint32 nCnt);
		virtual bool setPoints64(uint64 *ppPoints, uint32 nCnt);
		virtual bool setPoints(double *ppPoints, uint32 nCnt) const;
	private:
		double m_dGrigSizeX;
		double m_dGrigSizeY;
		double m_dOffsetX;
		double m_dOffsetY;
	};
}
#endif