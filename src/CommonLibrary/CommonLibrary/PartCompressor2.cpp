#include "stdafx.h"
#include "PartCompressor2.h"
#include "GeoShape.h"


#define BIT_OFFSET_PARTS_NULL 3
#define BIT_OFFSET_PARTS_COMPRESS 4

namespace CommonLib
{

	CPartCompressor::CPartCompressor()
	{
		clear();
	}
	CPartCompressor::~CPartCompressor()
	{

	}
	void CPartCompressor::clear()
	{
		m_NumLen.clear();
		m_bNullPart = false;
		m_bCompressPart = false;
		m_nDataType = dtType8;
	}
	void CPartCompressor::compress(const CGeoShape *pShp, CommonLib::IWriteStream* pStream)
	{
		byte nFlag = 0;
		uint32 nPosFlag = pStream->pos();
		pStream->write(nFlag);

		uint32 nPartCount = pShp->getPartCount();
		
		if(nPartCount == 1)
		{
			m_bNullPart = true;
		}
		else if(nPartCount == 0 || nPartCount < 10)
		{
			const uint32 *pParts = pShp->getParts();
			m_nDataType =  GetCompressType(nPartCount);
			for (uint32 i = 1; i < nPartCount; i++ )
			{
				uint32 nPart = pParts[i] - pParts[i - 1];
				eCompressDataType type  = GetCompressType(nPart);
				if(m_nDataType < type)
				{
					m_nDataType = type;
					if(m_nDataType == dtType32)
					{				 
						break;
					}
				}
			}

			m_bCompressPart = false;
			WriteValue(pShp->getPartCount(), m_nDataType, pStream);
			for (uint32 i = 1; i < nPartCount; i++ )
			{

				WriteValue(pParts[i] - pParts[i - 1], m_nDataType, pStream);
			}
		}
		else
		{
			m_bCompressPart = true;
			EncodePart(pShp, pStream);
		}
		WriteFlag(nPosFlag, pStream);
	
	}
	void CPartCompressor::EncodePart(const CGeoShape *pShp, CommonLib::IWriteStream* pStream)
	{
		m_NumLen.clear();
		m_NumLen.BeginEncode(pStream);
		uint32 nPartCount = pShp->getPartCount();
		const uint32 *pParts = pShp->getParts();

		for (uint32 i = 2; i < nPartCount; ++i)
		{
			assert(pParts[i] >= pParts[i - 1]);
			uint32 nDiff = pParts[i] - pParts[i - 1];

			m_NumLen.PreAddSympol(nDiff);
		}
		m_nDataType = GetTypeFromBitsLen(m_NumLen.GetMaxBitLen());

		m_NumLen.WriteHeader(pStream);
		pStream->write((uint32)pParts[1]);
		uint32 nBitLen = m_NumLen.GetBitsLen();
		uint32 nByteSize = (nBitLen + 7)/8;
		CommonLib::FxBitWriteStream bitStream;
		bitStream.attach(pStream, pStream->pos(), nByteSize);
		pStream->seek(nByteSize, soFromCurrent);


		for (uint32 i = 2; i < nPartCount; ++i)
		{
			assert(pParts[i] >= pParts[i - 1]);
			uint32 nDiff = pParts[i] - pParts[i - 1];

			m_NumLen.EncodeSymbol(nDiff, &bitStream);
		}

		m_NumLen.EncodeFinish();
		


	}
	void CPartCompressor::WriteFlag(uint32 nFlagPos, CommonLib::IWriteStream* pStream)
	{
		uint32 nEndPos = pStream->pos();
		pStream->seek(nFlagPos, soFromBegin);
		 
		 byte nFlag = (byte)m_nDataType;
		 if(m_bNullPart)
			 nFlag |= (((uint32)1) << BIT_OFFSET_PARTS_NULL);
		 else if(m_bCompressPart)
			 nFlag |= (((uint32)1) << BIT_OFFSET_PARTS_COMPRESS);
	

		pStream->write(nFlag);
		pStream->seek(nEndPos, soFromBegin);
	}
	void CPartCompressor::ReadFlag(CommonLib::IReadStream* pStream)
	{
		 byte nFlag = pStream->readByte();
		 m_nDataType = (eCompressDataType)(nFlag & 3);
		 m_bNullPart = ((nFlag >> BIT_OFFSET_PARTS_NULL) & 1) ? true : false;
		 m_bCompressPart = ((nFlag >> BIT_OFFSET_PARTS_COMPRESS) & 1) ? true : false;
	}
	void CPartCompressor::decompress(CGeoShape *pShp, CommonLib::IReadStream* pStream)
	{
		ReadFlag(pStream);

		int nParts = 0;

		if(m_bNullPart)
		{
		 
			 pShp->m_vecParts.resize(1);
			 pShp->getParts()[0] = 0;
			
		}
		else if(!m_bCompressPart)
		{
			nParts = ReadValue<uint32>(m_nDataType, pStream);
			pShp->m_vecParts.resize(nParts);
			pShp->getParts()[0] = 0; 
			for (uint32 i = 1; i < nParts; ++i)
			{
				pShp->getParts()[i] = (uint32)ReadValue<uint32>(m_nDataType, pStream) + pShp->getParts()[i - 1];
			}
		}
		else
		{
			DecodePart(pShp, pStream);
		}
	}

	void CPartCompressor::DecodePart(CGeoShape *pShp, CommonLib::IReadStream* pStream)
	{
		m_NumLen.Init(pStream);
		uint32 nPartCount = m_NumLen.GetCount() + 2;
		uint32 nBitsLen = m_NumLen.GetBitsLen();

		FxBitReadStream bitStream;
		

		pShp->m_vecParts.resize(nPartCount + 1);
		pShp->m_vecParts[0] = 0;
		pShp->m_vecParts[1]= pStream->readIntu32();

		uint32 nByteSize = (nBitsLen + 7)/8;
		bitStream.attach(pStream, pStream->pos(), nByteSize);
		pStream->seek(nByteSize, soFromCurrent);

		uint32 nBitPart = 0;
		uint32 nPartDiff = 0;


		m_NumLen.StartDecode();
		for (uint32 i = 2; i < nPartCount; ++i)
		{

			m_NumLen.DecodeSymbol(nPartDiff);

			nBitPart = nPartDiff;

			if(nBitPart > 1)
			{

				nBitPart = 0;
				bitStream.readBits(nBitPart, nPartDiff - 1);
				nBitPart |= (1 << nPartDiff- 1);
			}

			pShp->m_vecParts[i] = pShp->m_vecParts[i - 1] + nBitPart;
		}
	}

}