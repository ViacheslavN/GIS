#ifndef _LIB_COMMON_GEO_SHAPE_H_
#define _LIB_COMMON_GEO_SHAPE_H_
#include "IGeoShape.h"
 
namespace CommonLib
{
	
	struct segment_circular_arc_t
	{
		union
		{
			GeoPt   centerPoint;
			double angles[2];
		};

		circular_arc_flags flags;
	};

	struct segment_bezier_curve_t
	{
		GeoPt controlPoints[2];
	};

	struct segment_elliptic_arc_t
	{
		union
		{
			GeoPt center;
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


	class CGeoShape
	{
		public:
			CGeoShape(alloc_t *pAlloc = NULL);
			CGeoShape(const CGeoShape& geoShp);
			~CGeoShape();


			void create(size_t nSize);
			void create(eShapeType shapeType);
			void create(eShapeType shapeType, size_t npoints, size_t nparts = 1, size_t ncurves = 0, size_t mpatchSpecificSize = 0);
			void create(unsigned char* pBuf, size_t nSize, eShapeType shapeType, size_t npoints, size_t nparts = 1, size_t ncurves = 0);
			void import(const unsigned char* extBuf, size_t extBufSize);
			void attach(unsigned char* extBuf, size_t extBufSize);
			unsigned char* detach();
			bool validate();
			void setNull(eShapeType shapeType = shape_type_null);


			uint32     size() const;
			eShapeType type() const;
			eShapeType generalType() const;
			bool       hasZs() const;
			bool       hasMs() const;
			bool       hasCurves() const;
			bool       hasIDs() const;

			uint32  getPartCount() const;
			uint32  getPartSize(size_t idx) const;

			static bool isTypeSimple(eShapeType shapeType);
			static size_t calcSize(eShapeType shapeType, size_t npoints = 1, size_t nparts = 1, size_t ncurves = 0, size_t mpatchSpecificSize = 0);
			static void getTypeParams(eShapeType shapeType, eShapeType* genType, bool* has_z, bool* has_m, bool* has_curve, bool* has_id);
		private:
			simple_alloc_t m_alloc;
			alloc_t *m_pAlloc;
			byte* m_pBuffer;
			GeoPt *m_pPoints;
			uint32 m_nBufSize;
			bool m_bAttach;
	};
}
#endif