#ifndef _LIB_COMMON_GEO_SHAPE_H_
#define _LIB_COMMON_GEO_SHAPE_H_
#include "IGeoShape.h"
#include "PodVector.h"
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



		enum eDataType
		{
			ptType8 =  0,
			ptType16 = 1,
			ptType32 = 2,
			ptType64 = 3
		};

		struct compress_params
		{

			compress_params() : m_PointType(ptType64), m_dOffsetX(0), m_dOffsetY(0), m_dScaleX(0.00000001), m_dScaleY(0.00000001)
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

		/*
		CGeoShape(eShapeType shapeType, alloc_t *pAlloc = 0);
		CGeoShape(eShapeType shapeType, size_t npoints, size_t nparts = 1, size_t ncurves = 0, size_t mpatchSpecificSize = 0, alloc_t *pAlloc = 0);
		 */

		~CGeoShape();
		
		bool create(eShapeType shapeType);
		bool create(eShapeType shapeType, uint32 npoints, uint32 nparts = 1, uint32 ncurves = 0, uint32 mpatchSpecificSize = 0);
		
		bool write(IWriteStream *pStream) const;
		bool read(IReadStream *pStream);

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