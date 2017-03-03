#ifndef _LIB_COMMON_GEO_SHAPE_PART_COMPRESSOR_H_
#define _LIB_COMMON_GEO_SHAPE_PART_COMPRESSOR_H_
#include "NumLenCompressor.h"
#include "BitStream.h"
#include "FixedMemoryStream.h"
namespace CommonLib
{


	typedef TNumLemCompressor<uint32, TFindMostSigBit, 8> TPartNumLen8;
	typedef TNumLemCompressor<uint32, TFindMostSigBit, 16> TPartNumLen16;
	typedef TNumLemCompressor<uint32, TFindMostSigBit, 32> TPartNumLen32;


	class IPartComressor
	{
	public:
		virtual ~IPartComressor() {}
		IPartComressor(){}


		virtual eCompressDataType GetCompressDataType() const = 0;

		virtual void PreCompress(const uint32 *pParts, uint32 nCount) = 0;
		virtual uint32 GetCompressSize() const = 0;
		virtual uint32 GetCount() const = 0;
		virtual uint32 GetPartCount() const = 0;
		virtual void WriteHeader(IWriteStream *pStream) = 0;
		virtual bool  compress(const uint32 *pParts, uint32 nCount, IWriteStream *pStream) = 0;

		virtual void ReadHeader(IReadStream *pStream) = 0;
		virtual bool  decompress(uint32 *pParts, uint32 nCount, IReadStream *pStream) = 0;
		virtual void clear() = 0;
	};



	template <class _TNumLenCompressor>
	class TPartCompressor : public IPartComressor
	{
	public:
		typedef _TNumLenCompressor TNumLenCompressor;
	 

		TPartCompressor(eCompressDataType dateType) : m_dateType(dateType)
		{
			clear();
		}
		~TPartCompressor(){}

		virtual eCompressDataType GetCompressDataType() const {return m_dateType;}

		virtual void PreCompress(const uint32 *pParts, uint32 nCount)
		{
			for (uint32 i = 2; i < nCount; ++i)
			{
				assert(pParts[i] >= pParts[i - 1]);
				uint32 nDiff = pParts[i] - pParts[i - 1];

				m_Compressor.PreAddSympol(nDiff);
			}

		}

		virtual void WriteHeader(IWriteStream *pStream)
		{
			m_Compressor.WriteHeader(pStream);
		}
		virtual bool compress(const uint32 *pParts, uint32 nCount, IWriteStream *pStream)
		{
			uint32 nBitLen = m_Compressor.GetBitsLen();
			uint32 nByteSize = (nBitLen + 7)/8;
			CommonLib::FxBitWriteStream bitStream;
			
		
			bitStream.attach(pStream, pStream->pos(), nByteSize);
			pStream->seek(nByteSize, soFromCurrent);


			uint32 nSize = 0;		
			WriteValue(pParts[1], m_dateType, pStream);
			uint32 nBeginPos = pStream->pos();
			pStream->write(nSize);
			
			m_Compressor.BeginCompress(pStream);
			for (uint32 i = 2; i < nCount; ++i)
			{
				assert(pParts[i] >= pParts[i - 1]);
				uint32 nDiff = pParts[i] - pParts[i - 1];

				m_Compressor.EncodeSymbol(nDiff, &bitStream);
			}

			m_Compressor.EncodeFinish();

			uint32 nEndPos = pStream->pos();
			nSize = pStream->pos() - nBeginPos - sizeof(uint32);
			pStream->seek(nBeginPos, soFromBegin);
			pStream->write(nSize);
			pStream->seek(nEndPos, soFromBegin);
			return true;
		}

		virtual uint32 GetCompressSize() const
		{
			  return m_Compressor.GetCompressSize() + (m_Compressor.GetBitsLen() +7)/8 + GetSizeTypeValue(m_dateType) + sizeof(uint32);
		}

		virtual void clear()
		{
		 
			m_Compressor.clear();
		}


		 
		virtual uint32 GetCount() const
		{
			return m_Compressor.GetCount();
		}
		virtual uint32 GetPartCount() const
		{
			return m_Compressor.GetCount() + 2;
		}
		virtual void ReadHeader(IReadStream *pStream)
		{
			m_Compressor.ReadHeader(pStream);
		}
		virtual bool decompress(uint32 *pParts, uint32 nCount, IReadStream *pStream)
		{
			uint32 nBitLen = m_Compressor.GetBitsLen();
			uint32 nByteSize = (nBitLen + 7)/8;
			CommonLib::FxBitReadStream bitStream;

			
			bitStream.attach(pStream, pStream->pos(), nByteSize);
			pStream->seek(nByteSize, soFromCurrent);
			pParts[0] = 0;
			pParts[1] = ReadValue<uint32>(m_dateType, pStream);

			uint32 nBitPart = 0;
			uint32 nPartDiff = 0;
			
			uint32 nCompSize = pStream->readIntu32();

			FxMemoryReadStream compStream;
			compStream.attach(pStream, pStream->pos(), nCompSize);
			pStream->seek(nCompSize, soFromCurrent);
			m_Compressor.StartDecode(&compStream);

			for (uint32 i = 2; i < nCount; ++i)
			{

				m_Compressor.DecodeSymbol(nPartDiff);

				nBitPart = nPartDiff;

				if(nBitPart > 1)
				{

					nBitPart = 0;
					bitStream.readBits(nBitPart, nPartDiff - 1);
					nBitPart |= (1 << (nPartDiff- 1));
				}
				
				pParts[i] = pParts[i - 1] + nBitPart;

		
			}

			return true;
		}
	private:

		 TNumLenCompressor m_Compressor;
		 eCompressDataType m_dateType;
	};

	typedef TPartCompressor<TPartNumLen8> TPartCompressor8;
	typedef TPartCompressor<TPartNumLen16> TPartCompressor16;
	typedef TPartCompressor<TPartNumLen32> TPartCompressor32;
}

#endif