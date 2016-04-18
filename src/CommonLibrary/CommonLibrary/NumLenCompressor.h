#ifndef _LIB_COMMON_GEO_SHAPE_NUMLEN_COMPRESSOR_H_
#define _LIB_COMMON_GEO_SHAPE_NUMLEN_COMPRESSOR_H_
#include "stream.h"
#include "RangeCoder.h"
#include "compressutils.h"
#include "MathUtils.h"
#include "FixedBitStream.h"
#include "algorithm.h"
#include "ArithmeticCoder.h"
namespace CommonLib
{


	template <class _TValue, class _TFindBit, uint32 _nMaxBitsLens>
	class TNumLemCompressor
	{
		public:
			typedef _TValue TValue;
			typedef _TFindBit TFindBit;

			typedef TRangeEncoder64 TEncoder;
			typedef TRangeDecoder64 TDecoder;

			//typedef TACEncoder64 TEncoder;
			//typedef TACDecoder64 TDecoder;

			TNumLemCompressor(uint32 nError = 100)  : m_nError(nError)
			{
				 clear();
			}
			~TNumLemCompressor(){}

			uint16 PreAddSympol(TValue value)
			{

				uint16 nBitLen =  0;
				if(value == 0)
					nBitLen = 0;
				else if(value == 1)
					nBitLen = 1;
				else  nBitLen = m_FindBit.FMSB(value);
 

			 

				assert(nBitLen < _nMaxBitsLens);
				m_BitsLensFreq[nBitLen] += 1;

				if(m_FreqType != dtType32)
				{
					if(m_BitsLensFreq[nBitLen] > 255)
						m_FreqType = dtType16;
					if(m_BitsLensFreq[nBitLen] > 65535)
						m_FreqType = dtType32;
				}

				m_nBitLen += nBitLen > 1 ? nBitLen - 1 : 0;
				m_nCount += 1;
				return nBitLen;

			}

			void WriteHeader(CommonLib::IWriteStream* pStream)
			{
				byte nFlag = m_FreqType;
				pStream->write(nFlag);
				byte LensMask[(_nMaxBitsLens + 8)/8];

				memset(LensMask, 0, _nMaxBitsLens/8);
				for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
				{
					uint32 nByte = i/8;
					uint32 nBit =  i - (nByte * 8);
					if(m_BitsLensFreq[i] != 0)
						LensMask[nByte] |= (0x01 << nBit);
				}
				for (uint32 i = 0; i < (_nMaxBitsLens + 8)/8; ++i)
				{
					pStream->write((byte)LensMask[i]);
				}
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{
					if(m_BitsLensFreq[i] == 0)
						continue;

					switch(m_FreqType)
					{
					case dtType8:
						pStream->write((byte)m_BitsLensFreq[i]);
						break;
					case dtType16:
						pStream->write((uint16)m_BitsLensFreq[i]);
						break;
					case dtType32:
						pStream->write((uint32)m_BitsLensFreq[i]);
						break;
					}

				}
				memset(m_FreqPrev, 0, sizeof(m_FreqPrev));
				int32 nPrevF = 0;
				for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
				{
					m_FreqPrev[i + 1] = m_BitsLensFreq[i] + nPrevF;
					nPrevF = m_FreqPrev[i + 1];
				}
			}

			
			void BeginCompreess(CommonLib::IWriteStream* pStream)
			{
				m_pEncoder.reset(new TEncoder(pStream));
			}

			uint32 EncodeSymbol(TValue value, CommonLib::FxBitWriteStream *pBitStream)
			{
				uint16 nBitLen =  0;
				if(value == 0)
					nBitLen = 0;
				else if(value == 1)
					nBitLen = 1;
				else  nBitLen = m_FindBit.FMSB(value);

				assert(m_BitsLensFreq[nBitLen] != 0);
				assert(m_pEncoder.get());
				
				m_pEncoder->EncodeSymbol(m_FreqPrev[nBitLen], m_FreqPrev[nBitLen + 1],  m_nCount);
				if(nBitLen > 1)
					pBitStream->writeBits(value, nBitLen - 1);

				return nBitLen;

			}

			void EncodeFinish()
			{
				m_pEncoder->EncodeFinish();
			}


			void ReadHeader(CommonLib::IReadStream* pStream)
			{
				clear();
				byte nFlag = pStream->readByte();

				m_FreqType = (eCompressDataType)nFlag;

				byte LensMask[(_nMaxBitsLens + 8)/8];

				for (uint32 i = 0; i < (_nMaxBitsLens + 8)/8; ++i)
				{
					LensMask[i] = pStream->readByte();
				}

				for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
				{

					uint32 nByte = i/8;
					uint32 nBit  =  i - (nByte * 8);

					if(!(LensMask[nByte] & (0x01 << nBit)))
					{
						m_BitsLensFreq[i] = 0;
						continue;
					}
					switch(m_FreqType)
					{
					case dtType8:
						m_BitsLensFreq[i] = pStream->readByte();
						m_nCount += m_BitsLensFreq[i];
						break;
					case dtType16:
						m_BitsLensFreq[i] = pStream->readintu16();
						m_nCount += m_BitsLensFreq[i];
						break;
					case dtType32:
						m_BitsLensFreq[i] = pStream->readIntu32();
						m_nCount += m_BitsLensFreq[i];
						break;
					}
					if(i > 1)
					 m_nBitLen +=m_BitsLensFreq[i] * (i - 1);
				}

				int32 nPrevF = 0;
				for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
				{

					m_FreqPrev[i + 1] = m_BitsLensFreq[i] + nPrevF;
					nPrevF = m_FreqPrev[i + 1];
				}

			
			}

			void StartDecode(CommonLib::IReadStream* pStream)
			{
				m_pDecoder.reset(new TDecoder(pStream));
				m_pDecoder->StartDecode();
			}

			bool DecodeSymbol(uint32& value)
			{
				uint32 freq = (uint32)m_pDecoder->GetFreq(m_nCount);
				value = CommonLib::upper_bound(m_FreqPrev, _nMaxBitsLens, freq);
				if(value != 0)
					value--;


				m_pDecoder->DecodeSymbol(m_FreqPrev[value], m_FreqPrev[value+1], m_nCount);

				//value += 1;
				return true;
			}

			void clear()
			{
				memset(m_BitsLensFreq, 0, sizeof(m_BitsLensFreq));
				memset(m_FreqPrev, 0, sizeof(m_FreqPrev));
				m_nCount = 0;
				m_FreqType = dtType8;
				m_nBitLen = 0;
			}

		
			uint32 GetCount() const
			{
				return m_nCount;
			}

			double CalcRowBitSize() const
			{
				double dBitRowSize  = 0;
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{
					if(m_BitsLensFreq[i] == 0)
						continue;
					double dFreq = m_BitsLensFreq[i];
					double dLog2 = mathUtils::Log2((double)m_nCount/dFreq); 
					dBitRowSize += (dFreq* dLog2);
				}
				dBitRowSize += 64;
				dBitRowSize += (dBitRowSize/m_nError);

				return dBitRowSize;
			}
			uint32 GetHeaderSize() const
			{
				uint32 nSize = _nMaxBitsLens/8  + 1;
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{
					if(m_BitsLensFreq[i] != 0)
						nSize += GetSizeTypeValue(m_FreqType);
				}
				return nSize;
			}
			uint32 GetBitsLen() const
			{
				return m_nBitLen;
			}
			uint32 GetCompressSize() const
			{
			
				double dBitRowSize = CalcRowBitSize();
				uint32 nHeaderSize = GetHeaderSize();
				return  (uint32)(dBitRowSize +7)/8  + nHeaderSize;
			}
	private:

		uint32 m_BitsLensFreq[_nMaxBitsLens + 1];
		uint32 m_FreqPrev[_nMaxBitsLens + 1];
		TFindBit m_FindBit;
		uint32 m_nCount;
		uint32 m_nBitLen;
		eCompressDataType m_FreqType;
		typedef std::auto_ptr<TEncoder> TEncoderPtr;
		typedef std::auto_ptr<TDecoder> TDecoderPtr;

		TEncoderPtr m_pEncoder;
		TDecoderPtr m_pDecoder;
		uint32 m_nError;
	};

	
}

#endif