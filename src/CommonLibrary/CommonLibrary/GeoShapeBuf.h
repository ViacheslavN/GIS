#pragma  once

#include "IGeoShape.h"
#include "blob.h"
#include "compressutils.h"
#include "StreamShapeEncoder.h"

namespace CommonLib
{
	
 
	class CGeoShapeBuf 
	{

	

		public:

			enum eFlags
			{
				eNoSuccinct = 0,
				eSuccinct = 1

			};
			enum ePramsFlags
			{
				eExternParams = 0,
				eInnerParams = 1

			};



	

			CGeoShapeBuf(alloc_t *pAlloc = NULL);
			CGeoShapeBuf(const CGeoShapeBuf& geoShp);
			~CGeoShapeBuf();

			 CGeoShapeBuf&     operator=(const CGeoShapeBuf& shp);


			 bool IsSuccinct() const;
			 void decode();
			 void InnerEncode(CWriteMemoryStream *pCacheStream = nullptr, shape_compress_params *pCompParams = nullptr);

			 bool BeginReadSuccinct(shape_compress_params *pCompParams = nullptr) const;
			 void EndReadSuccinct() const;
			 uint32 nextPart(uint32 nIdx) const;

			 GisXYPoint nextPoint(uint32 nIdx);
		
			 void  WriteCompParams(IWriteStream *pStream, eShapeType shType, shape_compress_params& comp_params);
			 void  ReadCompParams(IReadStream *pStream, eShapeType shType, shape_compress_params& comp_params) const;
			

			virtual eShapeType type() const; 
	

			virtual uint32  getPartCount() const;
			virtual uint32  getPart(uint32 idx) const;
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


			static void getTypeParams(eShapeType shapeType, eShapeType* pGenType, bool* has_z = nullptr, bool* has_m = nullptr, bool* has_curve = nullptr, bool* has_id = nullptr);


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

			void calcBB();
	
			double *getBBoxVals();
			const double *getBBoxVals() const;
			bbox getBB() const;



		private:

			double *getBBoxVals(eShapeType shapeType);
			const double *getBBoxVals(eShapeType shapeType) const;

			void WriteEncodeHeader(CommonLib::IWriteStream *pStream, shape_compress_params *pCompParams);
 
		private:
			static const uint32 __minimum_point_ = 10;

			mutable CBlob m_blob;
			mutable Private::CStreamShapeEncoder m_Encoder;
			mutable shape_compress_params m_comp_params;
			mutable bool m_bInitSuccinct;

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
 