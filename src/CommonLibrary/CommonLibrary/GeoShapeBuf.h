#ifndef _LIB_COMMON_GEO_SHAPE_BUF_H_
#define _LIB_COMMON_GEO_SHAPE_BUF_H_
#include "IGeoShape.h"
 
namespace CommonLib
{
	
 
	class CGeoShapeBuf 
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


			virtual double* getZs();
			virtual const double* getZs() const;

			virtual double* getMs();
			virtual const double* getMs() const;


			void create(uint32 nSize);
			virtual bool create(eShapeType shapeType);
			virtual bool  create(eShapeType shapeType, uint32 npoints, uint32 nparts = 1, uint32 ncurves = 0, uint32 mpatchSpecificSize = 0);
			void create(unsigned char* pBuf, uint32 nSize, eShapeType shapeType, uint32 npoints, uint32 nparts = 1, uint32 ncurves = 0);
			void import(const unsigned char* extBuf, uint32 extBufSize);
			void attach(unsigned char* extBuf, uint32 extBufSize);
			unsigned char* detach();
			bool validate();
			void setNull(eShapeType shapeType = shape_type_null);


			static void getTypeParams(eShapeType shapeType, eShapeType* pGenType, bool* has_z, bool* has_m, bool* has_curve, bool* has_id);


			uint32  size() const;
			eShapeType generalType() const;
			bool       hasZs() const;
			bool       hasMs() const;
			bool       hasCurves() const;
			bool       hasIDs() const;

			uint32 pointCount() const;

			static void initShapeBufferBuffer(unsigned char* buf, eShapeType shapeType, uint32 npoints, uint32 nparts, uint32 ncurves);
			static uint32 calcSize(eShapeType shapeType, uint32 npoints = 1, uint32 nparts = 1, uint32 ncurves = 0, uint32 mpatchSpecificSize = 0);

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