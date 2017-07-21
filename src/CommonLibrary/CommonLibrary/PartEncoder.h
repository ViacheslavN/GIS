#pragma once
#include "blob.h"
#include "UnsignedNumLenEncoder.h"
#include "FixedMemoryStream.h"
#include "FixedBitStream.h"
namespace CommonLib
{
	namespace Private
	{


#define BIT_OFFSET_PARTS_NULL 3
#define BIT_OFFSET_PARTS_COMPRESS 4

		class CPartEncoder
		{
		public:
			CPartEncoder(CommonLib::alloc_t *pAlloc = nullptr);
			~CPartEncoder();

			static const uint32 __no_compress_parts = 10;

			void clear();
			void Reset();

			void InitDecode(CommonLib::IReadStream *pStream);

			uint32 getPartCnt() const;
			uint32 GetNextPart(uint32 nPos) const;



			void Encode(const uint32 *pParts, uint32 nCnt, CommonLib::IWriteStream *pStream);
		private:
			void EncodePart(const uint32 *pParts, uint32 nPartCount, CommonLib::IWriteStream* pStream);
			void WriteFlag(uint32 nFlagPos, CommonLib::IWriteStream* pStream);
			void ReadFlag(CommonLib::IReadStream* pStream);

			eCompressDataType GetDataType() const{return (eCompressDataType)(m_bFlag & 3); }
			bool IsNullPart() const {return ((m_bFlag >> BIT_OFFSET_PARTS_NULL) & 1) ? true : false;}
			bool IsCompressPart() const {return  ((m_bFlag >> BIT_OFFSET_PARTS_COMPRESS) & 1) ? true : false;	}


			void SetNullPart(bool bNull) 
			{
				if(bNull)
					m_bFlag |= (((uint32)1) << BIT_OFFSET_PARTS_NULL);
				else
					m_bFlag &= ~(((uint32)1) << BIT_OFFSET_PARTS_NULL);
			}
			void SetCompressPart(bool bCompress)
			{
				if(bCompress)
					m_bFlag |= (((uint32)1) << BIT_OFFSET_PARTS_COMPRESS);
				else
					m_bFlag &= ~(((uint32)1) << BIT_OFFSET_PARTS_COMPRESS);
			}
			void SetDataType(eCompressDataType type)
			{
				m_bFlag |= (type & 3);
			}
		private:
			uint32 m_nPartCnt;
			uint32 m_nBeginPart;
			mutable uint32 m_nNextDivPart;
 
			typedef TUnsignedNumLenEncoder<uint32, TACEncoder64, TACDecoder64, 32> TUnsignedNumLenEncoderU32;
			mutable TUnsignedNumLenEncoderU32 m_NumLen;
			FxMemoryReadStream m_ReadStream;
			byte m_bFlag;

		};
	}

}
 