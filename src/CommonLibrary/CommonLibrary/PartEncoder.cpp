#include "stdafx.h" 
#include "PartEncoder.h"



#define BIT_OFFSET_PARTS_NULL 3
#define BIT_OFFSET_PARTS_COMPRESS 4

namespace CommonLib
{
	namespace Private
	{
		CPartEncoder::CPartEncoder(CommonLib::alloc_t *pAlloc) : m_NumLen(pAlloc)
		{
			clear();
		}
		CPartEncoder::~CPartEncoder()
		{

		}


		void CPartEncoder::clear()
		{
			m_NumLen.clear();
			m_bNullPart = false;
			m_bCompressPart = false;
			m_nPartCnt = 0;
			m_nDataType = dtType64;
			memset(m_Parts, 0, sizeof(m_Parts));
			m_nNextPart = 0;
		}
		void CPartEncoder::Reset()
		{
			m_nNextPart = 0;
			if (m_bCompressPart)
				m_NumLen.Reset();
		}

		void CPartEncoder::InitDecode(CommonLib::IReadStream *pStream)
		{
			ReadFlag(pStream);
			if (m_bNullPart)
				return;

			if (!m_bCompressPart)
			{
				m_nPartCnt = ReadValue<uint32>(m_nDataType, pStream);
			 	for (uint32 i = 1; i < m_nPartCnt; ++i)
				{
					m_Parts[i] = (uint32)ReadValue<uint32>(m_nDataType, pStream) + m_Parts[i - 1];
				}

				return;
			}

			uint32 nCompSize = pStream->readIntu32();

			
		}

		uint32 CPartEncoder::getPartCnt() const
		{
			
		}
		uint32 CPartEncoder::GetNextPart() const
		{

		}



		void CPartEncoder::Encode(const uint32 *pParts, uint32 nPartCount, CommonLib::IWriteStream *pStream)
		{
			clear();

			byte nFlag = 0;
			uint32 nPosFlag = pStream->pos();
			pStream->write(nFlag);

			if (nPartCount == 1)
			{
				m_bNullPart = true;
			}
			else if (nPartCount == 0 || nPartCount < __no_compress_parts)
			{
				m_nDataType = GetCompressType(nPartCount);
				for (uint32 i = 1; i < nPartCount; i++)
				{
					uint32 nPart = pParts[i] - pParts[i - 1];
					eCompressDataType type = GetCompressType(nPart);
					if (m_nDataType < type)
					{
						m_nDataType = type;
						if (m_nDataType == dtType32)
						{
							break;
						}
					}
				}
				m_bCompressPart = false;
				WriteValue(nPartCount, m_nDataType, pStream);
				for (uint32 i = 1; i < nPartCount; i++)
				{
					WriteValue(pParts[i] - pParts[i - 1], m_nDataType, pStream);
				}
			}
			else
			{
				m_bCompressPart = true;
				uint32 nSizePos = pStream->pos();
				pStream->write(uint32(0));
				uint32 nBeginPos = pStream->pos();
				EncodePart(pParts, nPartCount, pStream);

				uint32 nEndPos = pStream->pos();
				pStream->seek(nSizePos, soFromBegin);
				pStream->write(nEndPos - nBeginPos);
				pStream->seek(nEndPos, soFromBegin);

			}
			WriteFlag(nPosFlag, pStream);
		}


		void CPartEncoder::EncodePart(const uint32 *pParts, uint32 nPartCount, CommonLib::IWriteStream* pStream)
		{
			m_NumLen.clear();
			

			for (uint32 i = 2; i < nPartCount; ++i)
			{
				assert(pParts[i] >= pParts[i - 1]);
				uint32 nDiff = pParts[i] - pParts[i - 1];

				m_NumLen.AddSymbol(nDiff);
			}
			pStream->write((uint32)pParts[1]);
			m_NumLen.BeginEncoding(pStream);
			
		 
			for (uint32 i = 2; i < nPartCount; ++i)
			{
				assert(pParts[i] >= pParts[i - 1]);
				uint32 nDiff = pParts[i] - pParts[i - 1];

				m_NumLen.encodeSymbol(nDiff);
			}
			m_NumLen.FinishEncoding(pStream);
		}
		void CPartEncoder::WriteFlag(uint32 nFlagPos, CommonLib::IWriteStream* pStream)
		{
			uint32 nEndPos = pStream->pos();
			pStream->seek(nFlagPos, soFromBegin);

			byte nFlag = (byte)m_nDataType;
			if (m_bNullPart)
				nFlag |= (((uint32)1) << BIT_OFFSET_PARTS_NULL);
			else if (m_bCompressPart)
				nFlag |= (((uint32)1) << BIT_OFFSET_PARTS_COMPRESS);


			pStream->write(nFlag);
			pStream->seek(nEndPos, soFromBegin);


		}
		void CPartEncoder::ReadFlag(CommonLib::IReadStream* pStream)
		{
			byte nFlag = pStream->readByte();
			m_nDataType = (eCompressDataType)(nFlag & 3);
			m_bNullPart = ((nFlag >> BIT_OFFSET_PARTS_NULL) & 1) ? true : false;
			m_bCompressPart = ((nFlag >> BIT_OFFSET_PARTS_COMPRESS) & 1) ? true : false;
		}

	}
}