#include "stdafx.h" 
#include "PartEncoder.h"




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
			m_nPartCnt = 0;
			m_bFlag = 0;
			m_nNextDivPart = 0;
			m_nBeginPart = 0;
		}
		void CPartEncoder::Reset()
		{
			if (IsCompressPart())
			{
				m_NumLen.Reset();
				m_nNextDivPart = m_nBeginPart;
			}
			else
				m_ReadStream.seek(0, soFromBegin);
		}

		void CPartEncoder::InitDecode(CommonLib::IReadStream *pStream)
		{
			if (m_bFlag != 0)
				return;
			
			ReadFlag(pStream);
			if (IsNullPart())
			{
				m_nPartCnt = 1;
				return;
			}

			if (!IsCompressPart())
			{
				m_nPartCnt = pStream->readByte();
				uint32 nSize = GetSizeTypeValue(GetDataType()) *(m_nPartCnt - 1);
				m_ReadStream.attach(pStream, pStream->pos(), nSize, true);
				return;
			}
				
			
			uint32 nCompSize = pStream->readIntu32();
			m_nBeginPart = pStream->readInt32();
			m_nNextDivPart = m_nBeginPart;
			m_NumLen.BeginDecoding(pStream, nCompSize - sizeof(uint32));
			m_nPartCnt = m_NumLen.count() + 1 + 1;
		}

		uint32 CPartEncoder::getPartCnt() const
		{
			return m_nPartCnt;
		}
		uint32 CPartEncoder::GetNextPart(uint32 nPos, uint32 nPointCnt) const
		{
			if (IsNullPart())
				return nPointCnt;

			if (nPos == m_nPartCnt - 1)
				return nPointCnt - m_nNextDivPart;

			if (!IsCompressPart())
			{
				uint32 nPart = ReadValue<uint32>(GetDataType(), (CommonLib::IReadStream*)&m_ReadStream);
				m_nNextDivPart += nPart;
				return nPart;
			}					 
			if (nPos == 0)
				return m_nNextDivPart;
	

			uint32 nPart = m_NumLen.decodeSymbol();
			m_nNextDivPart += nPart;
			return nPart;
		}



		void CPartEncoder::Encode(const uint32 *pParts, uint32 nPartCount, CommonLib::IWriteStream *pStream)
		{
			clear();

			uint32 nPosFlag = pStream->pos();
			pStream->write(m_bFlag);

			if (nPartCount == 1)
			{
				SetNullPart(true);
			}
			else if (nPartCount == 0 || nPartCount < __no_compress_parts)
			{
				eCompressDataType nDataType = GetCompressType(nPartCount);
				for (uint32 i = 1; i < nPartCount; i++)
				{
					uint32 nPart = pParts[i] - pParts[i - 1];
					eCompressDataType type = GetCompressType(nPart);
					if (nDataType < type)
					{
						nDataType = type;
						if (nDataType == dtType32)
						{
							break;
						}
					}
				}
				pStream->write((byte)nPartCount);
				for (uint32 i = 1; i < nPartCount; i++)
				{
					WriteValue(pParts[i] - pParts[i - 1], nDataType, pStream);
				}

				SetCompressPart(false);
				SetDataType(nDataType);
			}
			else
			{
				SetCompressPart(true);
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
			pStream->write(m_bFlag);
			pStream->seek(nEndPos, soFromBegin);


		}
		void CPartEncoder::ReadFlag(CommonLib::IReadStream* pStream)
		{
			m_bFlag = pStream->readByte();
		 
		}

	}
}