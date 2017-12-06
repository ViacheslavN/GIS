#pragma once


#include "stream.h"
#include "MemoryStream.h"
#include "blob.h"
#include "IGeoShape.h"
 



#define _PART_ENCODER_

#ifdef _PART_ENCODER_
	#include "PartEncoder.h"
#endif
#include "PointEncoder.h"
namespace CommonLib
{

	class CGeoShape;
	namespace Private
	{

	

		class CStreamShapeEncoder
		{

		public:
			CStreamShapeEncoder(CommonLib::alloc_t* pAlloc = nullptr);
			~CStreamShapeEncoder();


			bool IsSuccinct() const;

			void clear();

			bool Encode(const CGeoShape* pShape, IWriteStream *pStream, shape_compress_params *pParams);

			bool BeginDecode(IReadStream *pStream, shape_compress_params *pParams);
			void ResetDecode(shape_compress_params *pParams);

			uint32 cntParts() const;
			uint32 cntPoints() const;

			uint32 GetNextPart(int nIdx) const;
			GisXYPoint GetNextPoint(int nIdx, shape_compress_params *pParams) const;
			bool GetNextPoint(GisXYPoint& pt,int nIdx, shape_compress_params *pParams) const;

		private:

		

			CommonLib::alloc_t* m_pAlloc;
			CommonLib::simple_alloc_t m_alloc;
			CPointEncoder m_PointEncoder;
#ifdef _PART_ENCODER_
			CPartEncoder  m_PartEncoder;
#else
			uint32 m_nPartCnt;
			FxMemoryReadStream m_PartReadStream;
#endif
		};
	}


}