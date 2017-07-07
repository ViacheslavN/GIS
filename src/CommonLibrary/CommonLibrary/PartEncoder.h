#pragma once
#include "blob.h"
#include "UnsignedNumLenEncoder.h"
#include "FixedMemoryStream.h"
#include "FixedBitStream.h"
namespace CommonLib
{
	namespace Private
	{
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
			uint32 GetNextPart() const;



			void Encode(const uint32 *pParts, uint32 nCnt, CommonLib::IWriteStream *pStream);
		private:
			void EncodePart(const uint32 *pParts, uint32 nPartCount, CommonLib::IWriteStream* pStream);
			void WriteFlag(uint32 nFlagPos, CommonLib::IWriteStream* pStream);
			void ReadFlag(CommonLib::IReadStream* pStream);
		private:
			eCompressDataType m_nDataType;
			bool m_bNullPart;
			bool m_bCompressPart;

			uint32 m_nPartCnt;
			uint32 m_nNextPart;
 
			typedef TUnsignedNumLenEncoder<uint32, TACEncoder64, TACDecoder64, 32> TUnsignedNumLenEncoderU32;
			TUnsignedNumLenEncoderU32 m_NumLen;
			FxMemoryReadStream m_ReadStream;

			uint32 m_Parts[__no_compress_parts];

		};
	}

}
 