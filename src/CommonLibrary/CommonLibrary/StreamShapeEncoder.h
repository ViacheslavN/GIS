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

	class CGeoShapeBuf;
	namespace Private
	{

	

		class CStreamShapeEncoder
		{

		public:
			CStreamShapeEncoder(CommonLib::alloc_t* pAlloc = nullptr);
			~CStreamShapeEncoder();


			bool IsSuccinct() const;

			void clear();

			bool Encode(const CGeoShapeBuf* pShape, IWriteStream *pStream, CGeoShape::compress_params *pParams);

			bool BeginDecode(const CBlob& blob);

			uint32 cntParts() const;
			uint32 cntPoints() const;

			uint32 GetNextPart(int nIdx);
			GisXYPoint GetNextPoint(int nIdx, CGeoShape::compress_params *pParams);


		private:

		

			CommonLib::alloc_t* m_pAlloc;
			CommonLib::simple_alloc_t m_alloc;
			bool m_bInit;
			byte m_Flag;
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