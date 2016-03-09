#ifndef _LIB_COMMON_GEO_SHAPE_PART_COMPRESSOR_H_
#define _LIB_COMMON_GEO_SHAPE_PART_COMPRESSOR_H_
#include "NumLenCompressor.h"
#include "BitStream.h"
namespace CommonLib
{


	typedef TNumLemCompressor<uint32, TFindMostSigBit, 8> TPartNumLen8;
	typedef TNumLemCompressor<uint32, TFindMostSigBit, 16> TPartNumLen16;
	typedef TNumLemCompressor<uint32, TFindMostSigBit, 32> TPartNumLen32;

	template <class _TNumLenCompressor>
	class TPartCompressor
	{
	public:
		typedef _TNumLenCompressor TNumLenCompressor;
	

		TPartCompressor(eDataType dateType) : m_dateType(dateType)
		{
			clear();
		}
		~TPartCompressor(){}

		void PreCompressPart(uint32 *pParts, uint32 nCount)
		{
			for (uint32 i = 1; i < nCount; ++i)
			{
				assert(pParts[i] >= pParts[i - 1]);
				uint32 nDiff = pParts[i] - pParts[i - 1];

				m_Compressor.PreAddSympol(nDiff);
			}
		}


		bool compress(uint32 *pParts, uint32 nCount, IWriteStream *pStream)
		{
			uint32 nBitLen = m_Compressor.GetLenBits();
			uint32 nByteSize = (nBitLen + 7)/8;
			CommonLib::FxBitWriteStream bitStream;
			
			m_Compressor.BeginCompreess(pStream);
			bitStream.attach(pStream, pStream->pos(), nByteSize);
			pStream->seek(soFromCurrent, nByteSize);

			WriteValue(pParts[0], m_dateType, pStream);

			for (uint32 i = 1; i < nCount; ++i)
			{
				assert(pParts[i] >= pParts[i - 1]);
				uint32 nDiff = pParts[i] - pParts[i - 1];

				m_Compressor.EncodeSymbol(nDiff, &bitStream);
			}

			m_Compressor.EncodeFinish();
		}

		uint32 GetCompressSize() const
		{
			m_Compressor.GetCompressSize();
		}

		void clear()
		{
			m_Compressor.clear();
		}


		bool BeginDecompress(IWriteStream *pStream)
		{
			m_Compressor.BeginDecode(pStream);
			return true;
		}

		uint32 GetPartCount() const
		{
			return m_Compressor.count();
		}
		bool decompress(uint32 *pParts, uint32 nCount, IWriteStream *pStream)
		{
			uint32 nBitLen = m_Compressor.GetLenBits();
			uint32 nByteSize = (nBitLen + 7)/8;
			CommonLib::FxBitReadStream bitStream;

			m_Compressor.BeginCompreess(pStream);
			bitStream.attach(pStream, pStream->pos(), nByteSize);
			pStream->seek(soFromCurrent, nByteSize);

			pParts[0] = ReadValue<uint32>(m_dateType, pStream);

			uint32 nBitPart = 0;
			uint32 nPartDiff = 0;
			for (uint32 i = 1; i < nCount; ++i)
			{

				m_Compressor.DecodeSymbol(nPartDiff);
				bitStream.readBits(nBitPart, nPartDiff);

				pParts[i] = pParts[i - 1] + nBitPart;

		
			}
		}
	private:

		 TNumLenCompressor m_Compressor;
		 eDataType m_dateType;


	};

	typedef TPartCompressor<TPartNumLen8> TPartCompressor8;
	typedef TPartCompressor<TPartNumLen16> TPartCompressor16;
	typedef TPartCompressor<TPartNumLen32> TPartCompressor32;
}

#endif