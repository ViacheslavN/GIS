#ifndef _LIB_COMMON_GEO_SHAPE_BUF_H_
#define _LIB_COMMON_GEO_SHAPE_BUF_H_
#include "IGeoShape.h"
#include "blob.h"
namespace CommonLib
{
	
 
	class CGeoShapeBuf 
	{
		public:
			CGeoShapeBuf(alloc_t *pAlloc = NULL);
			CGeoShapeBuf(const CGeoShapeBuf& geoShp);
			~CGeoShapeBuf();

			 CGeoShapeBuf&     operator=(const CGeoShapeBuf& shp);


			 bool IsSuccinct() const;
			 bool decode() const;
			 bool InnerEncode() const;

			 bool BeginReadSuccinct() const;
			 void EndReadSuccinct() const;
			 GisXYPoint nextPoint();
			 uint32 nextPart();


			

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
			void create(byte* pBuf, uint32 nSize, eShapeType shapeType, uint32 npoints, uint32 nparts = 1, uint32 ncurves = 0);
			void import(const byte* extBuf, uint32 extBufSize);
			void attach(byte* extBuf, uint32 extBufSize);
			byte* detach();
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

			static uint32      pointCount(const byte* buf);
			static uint32      pointCount(const byte* buf, eShapeType general_type);
			static uint32      partCount(const byte* buf);
			static uint32      partCount(const byte* buf, eShapeType general_type);
			static eShapeType  type(const byte* buf);
			static eShapeType  generalType(const unsigned char* buf);
			
			static const GisXYPoint* getXYs(const byte* buf);
			static const GisXYPoint* getXYs(const byte* buf, eShapeType  general_type, uint32 partCount);

		private:
			
		private:

			CBlob m_blob;
	
			bool m_bIsSuccinct;

			struct SuccinctContext
			{
				std::vector<uint32> m_vecParts;
			};

			struct sShapeParams
			{

				sShapeParams();

				void set(const byte* pBuffer);
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