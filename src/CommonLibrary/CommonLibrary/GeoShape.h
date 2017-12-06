#pragma once
#pragma  once

#include "IGeoShape.h"
#include "blob.h"
#include "compressutils.h"
#include "StreamShapeEncoder.h"

namespace CommonLib
{


	class CGeoShape : public AutoRefCounter
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





		CGeoShape(alloc_t *pAlloc = NULL);
		CGeoShape(const CGeoShape& geoShp);
		~CGeoShape();
			void clear();

		CGeoShape&   operator=(const CGeoShape& shp);

 
		CGeoShape& operator += (const CGeoShape& shp)
		{
			return *this;
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

		bool operator ==(const CGeoShape& shp) const
		{
			return false;
		}
		bool operator !=(const CGeoShape& shp) const
		{
			if (type() != shp.type())
				return true;

			if (getPartCount() != shp.getPartCount())
				return true;

			if (getPointCnt() != shp.getPointCnt())
				return true;

			return m_blob != shp.m_blob;
 
		}

		bool write(IWriteStream *pStream) const;
		bool read(IReadStream *pStream);

		bool IsSuccinct() const;
		void InnerDecode(CWriteMemoryStream *pCacheStream = nullptr, shape_compress_params *pCompParams = nullptr);
		void InnerEncode(CWriteMemoryStream *pCacheStream = nullptr, shape_compress_params *pCompParams = nullptr);

		bool BeginReadSuccinct(shape_compress_params *pCompParams = nullptr) const;
		void EndReadSuccinct() const;
		uint32 nextPart(uint32 nIdx) const;

		GisXYPoint nextPoint(uint32 nIdx) const;
		bool nextPoint(uint32 nIdx, GisXYPoint& pt) const;

		void  WriteCompParams(IWriteStream *pStream, eShapeType shType, shape_compress_params& comp_params);
		void  ReadCompParams(IReadStream *pStream, eShapeType shType, shape_compress_params& comp_params) const;


		virtual eShapeType type() const;


		virtual uint32  getPartCount() const;
		virtual uint32  getPart(uint32 idx) const;
		virtual const uint32*  getParts() const;
		virtual uint32*  getParts();

		virtual GisXYPoint* getPoints();
		virtual const GisXYPoint* getPoints() const;
		virtual uint32 getPointCnt() const;


		virtual double* getZs();
		virtual const double* getZs() const;

		virtual double* getMs();
		virtual const double* getMs() const;



		patch_type*       getPartsTypes();
		const patch_type* getPartsTypes() const;
		patch_type        partType(uint32 idx);
		const patch_type  partType(uint32 idx) const;


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

		static GisXYPoint* getXYs(byte* buf);
		static GisXYPoint* getXYs(byte* buf, eShapeType  general_type, uint32 partCount);


		void calcBB();

		double *getBBoxVals();
		const double *getBBoxVals() const;
		bbox getBB() const;

		byte* buffer() { return m_blob.buffer(); }
		const byte* buffer() const { return m_blob.buffer(); }
		void SetCompParams(const shape_compress_params &compParams) 
		{
			m_comp_params = compParams;
		}

	private:

		const uint32*  getParts(const byte *pBuf) const;
		uint32*  getParts(byte *pBuf);


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

	typedef IRefCntPtr<CGeoShape> IGeoShapePtr;
}
