#ifndef _LIB_COMMON_GEO_SHAPE_H_
#define _LIB_COMMON_GEO_SHAPE_H_
#include "IGeoShape.h"
#include "PodVector.h"
#include "compressutils.h"
#include "blob.h"
namespace CommonLib
{
	class IWriteStream;
	class IReadStream;
	class CGeoShape  : public AutoRefCounter
	{

		typedef TPodVector<GisXYPoint> TVecGeoPt;
		typedef TPodVector<double> TVecZ;
		typedef TPodVector<double> TVecM;
		typedef TPodVector<uint32> TVecParts;
		typedef TPodVector<patch_type> TVecPartTypes;
	public:



	

		struct compress_params
		{

			compress_params() : m_PointType(dtType64), m_dOffsetX(0), m_dOffsetY(0), m_dScaleX(0.00000001), m_dScaleY(0.00000001)
			{

			}

			eDataType m_PointType;
			double m_dOffsetX;
			double m_dOffsetY;
			double m_dScaleX;
			double m_dScaleY;


		};


		CGeoShape(alloc_t *pAlloc = 0);

		CGeoShape(const CGeoShape& shp);
		CGeoShape& operator = (const CGeoShape& shp);
		CGeoShape& operator += (const CGeoShape& shp);


		bool operator ==(const CGeoShape& shp) const
		{
			return false;
		}
		bool operator !=(const CGeoShape& shp) const
		{
			if(type() != shp.type())
				return true;

			if(getPartCount() != shp.getPartCount())
				return true;

			for(uint32 i = 0, sz = getPartCount(); i < sz; ++i)
			{
				uint32 nPart1 = getPart(i);
				uint32 nPart2 = shp.getPart(i);
				if(getPart(i) != shp.getPart(i) )
					return true;
			}
			if(getPointCnt() != shp.getPointCnt())
				return true;

			for(uint32 i = 0, sz = getPointCnt(); i < sz; ++i)
			{

				GisXYPoint pt1 = getPoints()[i];
				GisXYPoint pt2 = shp.getPoints()[i];
				if(ptX(i) != shp.ptX(i) || ptY(i) != shp.ptY(i))
					return true;
			}

			return false;
		}
		bool operator <(const CGeoShape& shp) const
		{
			return false;
		}
		bool operator >(const CGeoShape& shp) const
		{
			return false;
		}
		bool operator <=(const CGeoShape& shp) const
		{
			return false;
		}
		bool operator >=(const CGeoShape& shp) const
		{
			return false;
		}

		

		~CGeoShape();
		
		bool create(eShapeType shapeType);
		bool create(eShapeType shapeType, uint32 npoints, uint32 nparts = 1, uint32 ncurves = 0, uint32 mpatchSpecificSize = 0);
		bool attach(byte *pBuf, uint32 nSize, bool bCopy = false);
		bool attach(const CBlob *pBlob, bool bCopy = false);


		bool write(IWriteStream *pStream) const;
		bool read(IReadStream *pStream);

		static uint32 CalcSize(eShapeType shapeType, uint32 npoints, uint32 nparts, uint32 ncurves , uint32 mpatchSpecificSize);

		bool compress(IWriteStream *pStream, compress_params* pParams = NULL) const;
		bool decompress(IReadStream *pStream, compress_params* pParams = NULL);

		static void getTypeParams(eShapeType shapeType, eShapeType* pGenType, bool* has_z, bool* has_m, bool* has_curve, bool* has_id);
		
		void clear();
		void calcBB();
		const bbox& getBB() const;
		bbox& getBB();
		
		eShapeType type() const;
		eShapeType GetGeneralType() const;
		uint32  getPartCount() const;
		//const uint32&  getPart(uint32 idx) const;
		uint32  getPart(uint32 idx) const;
		const uint32*  getParts() const;
		uint32*  getParts();
		
		patch_type*       getPartsTypes();
		const patch_type* getPartsTypes() const;
		patch_type&       partType(uint32 idx);
		const patch_type& partType(uint32 idx) const;
		
		
		GisXYPoint* getPoints();
		const GisXYPoint* getPoints() const;
		void setPoints(const double *pPoint);
		uint32 getPointCnt() const;
		
		double&       ptX(uint32 idx); 
		const double& ptX(uint32 idx) const; 
		
		double&       ptY(uint32 idx); 
		const double& ptY(uint32 idx) const; 
		
		
		double* getZs();
		const double* getZs() const;
		void setZs(const double *pZs);
		double&       ptZ(uint32 idx);
		const double& ptZ(uint32 idx) const;
		
		double* getMs();
		const double* getMs() const;
		double&       ptM(uint32 idx);
		const double& ptM(uint32 idx) const;

	private:
		simple_alloc_t m_alloc;
		alloc_t *m_pAlloc;

		TVecParts m_vecParts;
		TVecGeoPt m_vecPoints;
		TVecZ	  m_vecZs;
		TVecM	  m_vecMs;
		TVecPartTypes  m_vecPartTypes;

		eShapeType m_type;
		eShapeType m_general_type;
		bbox m_bbox;
		friend class ShapeCompressor;

	};

	typedef IRefCntPtr<CGeoShape> IGeoShapePtr;

}

#endif