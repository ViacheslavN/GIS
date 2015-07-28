#ifndef _LIB_COMMON_GEO_SHAPE_BUF_H_
#define _LIB_COMMON_GEO_SHAPE_BUF_H_
#include "IGeoShape.h"
 
namespace CommonLib
{
	
	struct segment_circular_arc_t
	{
		union
		{
			GisXYPoint   centerPoint;
			double angles[2];
		};

		circular_arc_flags flags;
	};

	struct segment_bezier_curve_t
	{
		GisXYPoint controlPoints[2];
	};

	struct segment_elliptic_arc_t
	{
		union
		{
			GisXYPoint center;
			double vs[2];
		};

		union
		{
			double rotation;
			double fromV;
		};

		double semiMajor;

		union
		{
			double minorMajorRatio;
		};

		elliptic_arc_flags flags;
	};

	struct segment_modifier_t
	{
		long         fromPoint;
		segment_type segmentType;
		union
		{
			segment_circular_arc_t circularArc;
			segment_bezier_curve_t bezierCurve;
			segment_elliptic_arc_t ellipticArc;
		} segmentParams;
	};


	class CGeoShapeBuf : public IGeoShape
	{
		public:
			CGeoShapeBuf(alloc_t *pAlloc = NULL);
			CGeoShapeBuf(const CGeoShapeBuf& geoShp);
			~CGeoShapeBuf();

			 CGeoShapeBuf&     operator=(const CGeoShapeBuf& shp);


			virtual eShapeType type() const; 
	

			virtual uint32  getPartCount() const;
			virtual uint32  getPartSize(uint32 idx) const;
			virtual const uint32*  getParts() const ;
			virtual uint32*  getParts();

			virtual GisXYPoint* getPoints();
			virtual const GisXYPoint* getPoints() const;
			virtual uint32 getPointCnt() const;


			void create(size_t nSize);
			void create(eShapeType shapeType);
			void create(eShapeType shapeType, size_t npoints, size_t nparts = 1, size_t ncurves = 0, size_t mpatchSpecificSize = 0);
			void create(unsigned char* pBuf, size_t nSize, eShapeType shapeType, size_t npoints, size_t nparts = 1, size_t ncurves = 0);
			void import(const unsigned char* extBuf, size_t extBufSize);
			void attach(unsigned char* extBuf, size_t extBufSize);
			unsigned char* detach();
			bool validate();
			void setNull(eShapeType shapeType = shape_type_null);


			uint32  size() const;
			eShapeType generalType() const;
			bool       hasZs() const;
			bool       hasMs() const;
			bool       hasCurves() const;
			bool       hasIDs() const;

			uint32 pointCount() const;

			static void initShapeBufferBuffer(unsigned char* buf, eShapeType shapeType, size_t npoints, size_t nparts, size_t ncurves);
			static size_t calcSize(eShapeType shapeType, size_t npoints = 1, size_t nparts = 1, size_t ncurves = 0, size_t mpatchSpecificSize = 0);

			static uint32      pointCount(const unsigned char* buf);
			static uint32      pointCount(const unsigned char* buf, eShapeType general_type);
			static uint32      partCount(const unsigned char* buf);
			static uint32      partCount(const unsigned char* buf, eShapeType general_type);
			static eShapeType  type(const unsigned char* buf);
			static eShapeType  generalType(const unsigned char* buf);
			
			static const GisXYPoint* getXYs(const unsigned char* buf);
			static const GisXYPoint* getXYs(const unsigned char* buf, eShapeType  general_type, uint32 partCount);

		private:
			
		private:
			simple_alloc_t m_alloc;
			alloc_t *m_pAlloc;
			byte* m_pBuffer;
			uint32 m_nBufSize;
			bool m_bAttach;

			struct sShapeParams
			{

				sShapeParams();

				void set(const unsigned char* pBuffer);
				void reset();


				eShapeType m_type;
				eShapeType m_general_type;
				uint32 m_nPointCount;
				uint32 m_nPartCount;
				const GisXYPoint *m_pPoints;
				bool m_bIsValid;
			};

			sShapeParams m_params;
			
	};
}
#endif