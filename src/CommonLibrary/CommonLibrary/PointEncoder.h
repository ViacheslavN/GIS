#pragma once
#include "UnsignedNumLenEncoder.h"
#include "SignEncoder.h"
#include "MemoryStream.h"
#include "GeoShape.h"

namespace CommonLib
{
	namespace Private
	{
		class CPointEncoder
		{

 
		public:
			CPointEncoder(CommonLib::alloc_t *pAlloc = nullptr);
			~CPointEncoder();

			void clear();

			void encode(const  GisXYPoint* pPts, uint32 nCnts, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream);
			void Reset();
			void InitDecode(CommonLib::IReadStream *pStream, CGeoShape::compress_params *pParams);

			uint32 getPointCnts() const;
			GisXYPoint GetNextPoint(uint32 nPos, CGeoShape::compress_params *pParams) const;


		private:
			
			void calc(const  GisXYPoint* pPts, uint32 nCnts, CGeoShape::compress_params *pParams);
			void compressImpl(const  GisXYPoint* pPts, uint32 nCnts, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream);


			void PreAddCoord(uint32 nPos, uint64 prev, uint64 next, TSignEncoder& signCpmrpessor);
			void CompreessCoord(uint32 nPos, uint64 prev, uint64 next,	TSignEncoder &signCpmrpessor);

			typedef TUnsignedNumLenEncoder<uint64, TACEncoder64, TACDecoder64, 64> TUnsignedNumLenEncoderU64;
		private:
 

			mutable TSignEncoder m_SignX;
			mutable TSignEncoder m_SignY;
			mutable TUnsignedNumLenEncoderU64  m_Points;

			int64 m_X;
			int64 m_Y;

			mutable int64 m_XPrev;
			mutable int64 m_YPrev;
		};
	}
}