#ifndef _EMBEDDED_DATABASE_NUM_LEN_COMPRESS_H_
#define _EMBEDDED_DATABASE_NUM_LEN_COMPRESS_H_

#include "CommonLibrary/FixedBitStream.h"
#include <map>
#include "../MathUtils.h"
#include "../../BPVector.h"
#include "CommonLibrary/algorithm.h"
#include "CommonLibrary/RangeCoder.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "CompressUtils.h"
#include "../../embDB.h"
namespace embDB
{
	
	//						    0	 1  2  3  4  5  6  7  8....................15
	//static int bits_lens[] = {0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};
	  static int bits_lens[] = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};
	struct TFindMostSigBit
	{

		
/*#ifdef _WIN32
		uint16 FMSB(uint32 x)
		{
			int32 y;
			__asm
			{
				bsr eax, x
				mov y, eax
			}
			return (uint16)y;
		}
#endif*/
		static uint32 FMSB(uint16 val16)
		{
			int bits = 0;
			if(val16 > 0xff){
				bits += 8;
				val16 >>= 8;
			}
			if(val16 > 0xf){
				bits += 4;
				val16 >>= 4;
			}
			bits += bits_lens[val16];
			return bits;
		}


		static uint32 FMSB(uint32 val32)
		{
			uint32 bits = 0;
		 
			if(val32 > 0xffff){
				bits = 16;
				val32 >>= 16;
			}
			if(val32 > 0xff){
				bits += 8;
				val32 >>= 8;
			}
			if(val32 > 0xf){
				bits += 4;
				val32 >>= 4;
			}
			bits += bits_lens[val32];
			return bits;
		}


		static uint32 FMSB(uint64 val64)
		{
			uint32 bits = 0;
			uint32 val32;

			if(val64 > 0xffffffff)
			{
				val32 = (uint32)(val64 >> 32);
				bits = 32;
			}
			else
				val32 = (unsigned int)val64;
			bits += FMSB(val32);
			return bits;
		}

		static uint32 FMSB(int16 val16)
		{
			return FMSB((uint16)abs(val16) );
		}

		static uint32 FMSB(int32 val32)
		{
			return FMSB((uint32)abs(val32) );
		}

		static uint32 FMSB(int64 val64)
		{
			 return FMSB((uint64)abs(val64) );
		}

	};

	template<class _TValue, class _TFindMostSigBit, 
			class _TRangeEncoder,
			class _TACEncoder,
			class _TRangeDecoder,
			class _TACDecoder,		
			uint32 _nMaxBitsLens>
	class TUnsignedNumLenCompressor
	{
		public:

			typedef _TValue TValue;
			typedef _TFindMostSigBit TFindBit;
			typedef _TRangeEncoder   TRangeEncoder;
			typedef _TACEncoder		 TACEncoder;
			typedef _TRangeDecoder	 TRangeDecoder;
			typedef _TACDecoder		 TACDecoder;
		 

			//static const uint32 m_nMaxBitsLens = _nMaxBitsLens + 1;
			//static const uint32 m_nByteSize = (_nMaxBitsLens + 1 +7)/8;

			//Flags
			//diff lens|type freq|type comp|
			//type comp      1/0 1-range coder, 0-ac-coder  1 bit
			//type freq value  1-byte, 2-short, 3-int32     2 bit
			 

			
	 

			TUnsignedNumLenCompressor(EncoderType nType, uint32 nError = 200, bool bOnlineCalcSize = false) : m_nLenBitSize(0), m_nCount(0), m_nDiffsLen(0), m_nFlags(0),
						m_nTypeFreq(ectByte), m_nError(nError), m_dBitRowSize(0.), m_bOnlineCalcSize(bOnlineCalcSize), m_nType(nType)
			{
				 memset(m_BitsLensFreq, 0, sizeof(m_BitsLensFreq));
			}

			uint16 AddSymbol(TValue symbol)
			{


				uint16 nBitLen =  0;
				if(symbol == 0)
					nBitLen = 0;
				else if(symbol == 1)
					nBitLen = 1;
				else  nBitLen = m_FindBit.FMSB(symbol);

				assert(nBitLen < _nMaxBitsLens + 1);

				m_nLenBitSize +=  nBitLen > 1 ? nBitLen - 1 : 0;
				m_nCount++;

				if(!m_BitsLensFreq[nBitLen])
					m_nDiffsLen++;
				
				m_BitsLensFreq[nBitLen] += 1;



				if(m_nTypeFreq != ectUInt32)
				{
					if(m_BitsLensFreq[nBitLen] > 255)
						m_nTypeFreq = ectUInt16;
					if(m_BitsLensFreq[nBitLen] > 65535)
						m_nTypeFreq = ectUInt32;
				}
				
				if(!m_bOnlineCalcSize)
					return nBitLen;

				uint32 nNewCount = m_BitsLensFreq[nBitLen];
				uint32 nOldCount = nNewCount - 1;
				
				if(m_nDiffsLen > 1)
				{
					//m_dBitRowSize += (nNewCount * mathUtils::Log2((double)m_nCount/nNewCount) -  
					//	nOldCount* mathUtils::Log2((double)(m_nCount - 1)/nOldCount) + (m_nCount - nNewCount) * mathUtils::Log2((double)m_nCount/(m_nCount - 1)));
				
					m_dBitRowSize += (nNewCount * mathUtils::Log2((double)m_nCount/nNewCount));
					if(nOldCount > 0)
						m_dBitRowSize -= nOldCount* mathUtils::Log2((double)(m_nCount - 1)/nOldCount);

					m_dBitRowSize += (m_nCount - nNewCount) * mathUtils::Log2((double)m_nCount/(m_nCount - 1));
				
				}

			/*	double dBitRowSize = CalcRowBitSize();
				if(fabs(m_dBitRowSize - dBitRowSize) > 0.00000001)
				{
					int dd = 0;
					dd++;
				}*/

				return nBitLen;
			}

			void RemoveSymbol(TValue symbol)
			{
				uint16 nBitLen =  0;
				if(symbol == 0)
					nBitLen = 0;
				else if(symbol == 1)
					nBitLen = 1;
				else  
				{
					nBitLen = m_FindBit.FMSB(symbol);
					assert(m_nLenBitSize >= nBitLen - 1);
				}



			
				m_nLenBitSize -= nBitLen > 1 ? nBitLen - 1 : 0;
				m_nCount--;

				assert(m_BitsLensFreq[nBitLen] );				

				m_BitsLensFreq[nBitLen] -= 1;
				if(m_BitsLensFreq[nBitLen] == 65535 || m_BitsLensFreq[nBitLen] == 255)
				{

					m_nTypeFreq = ectByte;
					for (uint32 i = 0; i < _nMaxBitsLens; ++i)
					{
						if(m_BitsLensFreq[i] > 65535)
						{
							m_nTypeFreq = ectUInt32;
							break;
						}
						if(m_nTypeFreq != ectUInt32)
						{
							if(m_BitsLensFreq[nBitLen] > 255)
								m_nTypeFreq = ectUInt16;
						}
					}
				}
				if(!m_BitsLensFreq[nBitLen])
					m_nDiffsLen--;
				





				if(!m_bOnlineCalcSize)
					return;

				uint32 nNewCount = m_BitsLensFreq[nBitLen];
				uint32 nOldCount = nNewCount + 1;

				if(m_nDiffsLen > 1)
				{

					m_dBitRowSize -= (nOldCount* mathUtils::Log2((double)(m_nCount + 1)/(nOldCount)));
					if(nNewCount > 0)
						m_dBitRowSize += (nNewCount* mathUtils::Log2((double)(m_nCount)/(nNewCount)));

					m_dBitRowSize -= (m_nCount - nNewCount) * mathUtils::Log2((double)(m_nCount + 1)/(m_nCount));

				}
				else
				{
					m_dBitRowSize = 0;
			
				}


				/*	double dBitRowSize = CalcRowBitSize();
				if(fabs(m_dBitRowSize - dBitRowSize) > 0.00000001)
				{
					int dd = 0;
					dd++;
				}*/

			}
 
			 
			double GetCodeBitSize() const
			{
				double dBitRowSize = m_bOnlineCalcSize ? m_dBitRowSize :  CalcRowBitSize<uint32>(m_BitsLensFreq, _nMaxBitsLens + 1, m_nDiffsLen, m_nCount);
				if(m_nType == ACCoding)
					dBitRowSize += 64;
				else				
					dBitRowSize += (dBitRowSize/m_nError)   + 64 /*code  finish*/; 
				
				return dBitRowSize;
			}
			int GetBitLenSize() const
			{
				return m_nLenBitSize;
			}

			uint32 GetCompressSize() const
			{
				double dRowBitsLen = GetCodeBitSize();
				uint32 nByteSize = (dRowBitsLen + 7)/8;



				return nByteSize + 1 + (_nMaxBitsLens + 1 + 7)/8 + GetLenForDiffLen(m_nTypeFreq, m_nDiffsLen) + (m_nLenBitSize +7)/8; //Type comp (rang or ac) + 4 +

			}

			 
		
			bool compress(const TBPVector<TValue>& vecValues, CommonLib::IWriteStream* pStream)
			{

				assert(m_nCount == vecValues.size());
				 TBPVector<TValue> vec;
				uint32 nBeginPos = pStream->pos();
				byte nFlag = 0;
				pStream->write(nFlag);

				WriteDiffsLens(pStream);

				double dRowBitsLen = GetCodeBitSize();
				uint32 nByteSize = (dRowBitsLen + 7)/8;

				

				uint32 FreqPrev[_nMaxBitsLens + 1 + 1];
				memset(FreqPrev, 0, sizeof(FreqPrev));

				int32 nPrevF = 0;
				for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
				{

					FreqPrev[i + 1] = m_BitsLensFreq[i] + nPrevF;
					nPrevF = FreqPrev[i + 1];
				}

				CommonLib::FxBitWriteStream bitStream;
				uint32 nBitSize = (m_nLenBitSize +7)/8;
			 
				bitStream.attach(pStream, pStream->pos(), nBitSize);
				pStream->seek(nBitSize, CommonLib::soFromCurrent);
				uint32 nBeginCompressPos = pStream->pos();
				bool bRangeCode = true;


				if(m_nType == ACCoding)
				{
					bRangeCode = false;
					CompressAcCode(vecValues, pStream,  FreqPrev,  &bitStream);
				}
				else
				{
					if(!CompressRangeCode(vecValues, pStream, FreqPrev, nByteSize, &bitStream))
					{
						bitStream.seek(0, CommonLib::soFromBegin);
						pStream->seek(nBeginCompressPos, CommonLib::soFromBegin);
						CompressAcCode(vecValues, pStream,  FreqPrev,  &bitStream);
						bRangeCode = false;
					}

				}
			


				uint32 nEndPos = pStream->pos();

				uint32 nCompressSize= nEndPos - nBeginCompressPos;
				 
				if(bRangeCode)
					nFlag |= 0x1;

				pStream->seek(nBeginPos, CommonLib::soFromBegin);

				nFlag |= (((byte)m_nTypeFreq) << 1);
				pStream->write(nFlag);
				pStream->seek(nEndPos, CommonLib::soFromBegin);
				return true;
			}
			bool decompress(uint32 nSize,  TBPVector<_TValue>& vecValues, CommonLib::IReadStream* pStream)
			{
				clear();
				byte nFlag = pStream->readByte();
				bool bRangeCode = nFlag & 0x01;
				m_nTypeFreq = (eCompressDataType)(nFlag>>1);
				ReadDiffsLens(pStream);

				if(m_bOnlineCalcSize)
					m_dBitRowSize = CalcRowBitSize<uint32>(m_BitsLensFreq, _nMaxBitsLens + 1, m_nDiffsLen, m_nCount);

				
		 

				uint32 FreqPrev[_nMaxBitsLens + 1 + 1];
				memset(FreqPrev, 0, sizeof(FreqPrev));

				int32 nPrevF = 0;
				for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
				{

					FreqPrev[i + 1] = m_BitsLensFreq[i] + nPrevF;
					nPrevF = FreqPrev[i + 1];
				}
			//	uint16 nBitSize = pStream->readintu16();
				uint32 nBitSize = (m_nLenBitSize +7)/8;
				CommonLib::FxBitReadStream bitStream;

		 
				bitStream.attach(pStream, pStream->pos(), nBitSize);
				pStream->seek(nBitSize, CommonLib::soFromCurrent);

				if(bRangeCode)
					return Decompress<TRangeDecoder>(vecValues, pStream, FreqPrev, &bitStream);
				else
					return Decompress<TACDecoder>(vecValues, pStream, FreqPrev, &bitStream);

				//DecompressRangeCode(vecValues, pStream, FreqPrev, &bitStream);
		 
			}
			void clear()
			{
				m_nLenBitSize = 0;
				m_nCount = 0;
				m_nDiffsLen = 0;
				m_nTypeFreq = ectByte;
				m_nFlags = 0;
				m_dBitRowSize = 0.;
				memset(m_BitsLensFreq, 0, sizeof(m_BitsLensFreq));
			}
		protected:

			bool CompressRangeCode(const TBPVector<_TValue>& vecValues, CommonLib::IWriteStream* pStream, uint32 *FreqPrev, uint32 nMaxByteSize,
				CommonLib::FxBitWriteStream *pBitStream)
			{

				TRangeEncoder rgEncoder(pStream, nMaxByteSize);

				for (uint32 i = 0, sz = vecValues.size(); i< sz; ++i)
				{
				
					_TValue value = vecValues[i];
					uint16 nBitLen =  m_FindBit.FMSB(value);

					assert(m_BitsLensFreq[nBitLen] != 0);

					if(nBitLen > 1)
						pBitStream->writeBits(value, nBitLen - 1);
					if(!rgEncoder.EncodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen + 1], m_nCount))
						return false;
				}
				

				return rgEncoder.EncodeFinish();
			}

			void CompressAcCode(const TBPVector<_TValue>& vecValues, CommonLib::IWriteStream* pStream,
				uint32 *FreqPrev, CommonLib::FxBitWriteStream *pBitStream)
			{
				TACEncoder acEncoder(pStream);

				for (uint32 i = 0, sz = vecValues.size(); i< sz; ++i)
				{
					TValue value = vecValues[i];
					uint16 nBitLen =  m_FindBit.FMSB(value);
					if(nBitLen > 1)
						pBitStream->writeBits(value, nBitLen - 1);
					acEncoder.EncodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen + 1], m_nCount);
						
				}


				acEncoder.EncodeFinish();
			}

			void WriteDiffsLens(CommonLib::IWriteStream* pStream)
			{
				uint32 nCount = (_nMaxBitsLens + 1 +7)/8;
				byte LensMask[(_nMaxBitsLens + 1 +7)/8];

				memset(LensMask, 0, sizeof(LensMask));
				for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
				{
					uint32 nByte = i/8;
					uint32 nBit =  i - (nByte * 8);
					if(m_BitsLensFreq[i] != 0)
						LensMask[nByte] |= (0x01 << nBit);
				}
				for (uint32 i = 0; i < nCount; ++i)
				{
					pStream->write((byte)LensMask[i]);
				}
				
				for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
				{
					if(m_BitsLensFreq[i] == 0)
						continue;

					switch(m_nTypeFreq)
					{
					case ectByte:
						pStream->write((byte)m_BitsLensFreq[i]);
						break;
					case ectUInt16:
						pStream->write((uint16)m_BitsLensFreq[i]);
						break;
					case ectUInt32:
						pStream->write((uint32)m_BitsLensFreq[i]);
						break;
					}

				}
				
			}


			void ReadDiffsLens(CommonLib::IReadStream* pStream)
			{

			   	uint32 nCount = (_nMaxBitsLens + 1 +7)/8;
				byte LensMask[ (_nMaxBitsLens + 1 +7)/8];

			/*	memset(LensMask, 0, _nMaxBitsLens/8);
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{
					uint32 nByte = i/8;
					uint32 nBit =  i - (nByte * 8);
					if(m_BitsLensFreq[i] != 0)
						LensMask[nByte] |= (0x01 << nBit);
				}*/
				for (uint32 i = 0; i < nCount; ++i)
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
					m_nDiffsLen++;
					switch(m_nTypeFreq)
					{
					case ectByte:
						m_BitsLensFreq[i] = pStream->readByte();
						m_nCount += m_BitsLensFreq[i];
						break;
					case ectUInt16:
						m_BitsLensFreq[i] = pStream->readintu16();
						m_nCount += m_BitsLensFreq[i];
						break;
					case ectUInt32:
						m_BitsLensFreq[i] = pStream->readIntu32();
						m_nCount += m_BitsLensFreq[i];
						break;
					}

					if(i > 1)
						m_nLenBitSize += (m_BitsLensFreq[i]) * (i - 1);
				}

			}

			

			template<class TDecoder>
			bool Decompress(TBPVector<_TValue>& vecValues, CommonLib::IReadStream* pStream, uint32 *FreqPrev, 
				CommonLib::FxBitReadStream *pBitStream)
			{


				TDecoder decoder(pStream);
				decoder.StartDecode();

				for (uint32 i = 0; i < m_nCount; ++i)
				{
					unsigned int freq = decoder.GetFreq(m_nCount);

					//uint32 nBitLen;
					//for(nBitLen = _nMaxBitsLens;FreqPrev[nBitLen] > freq;nBitLen--);
					int32 nBitLen = CommonLib::upper_bound(FreqPrev, _nMaxBitsLens + 1, freq);
					if(nBitLen != 0)
						nBitLen--;

					TValue value = 0;
					if(nBitLen == 0)
						value = 0;
					else if(nBitLen == 1)
						value = 1;
					else
					{
						pBitStream->readBits(value, nBitLen - 1);
						value |= ((TValue)1 << (nBitLen - 1));
					}

					vecValues.push_back(value);

					decoder.DecodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen+1], m_nCount);

				}

				return true;
			}


			

		protected:	

			TFindBit    m_FindBit;
			uint32 m_nError;
	  
			uint32 m_nLenBitSize;
			uint32 m_nCount;

			uint32 m_nDiffsLen;

			uint32  m_BitsLensFreq[_nMaxBitsLens + 1];
			uint16 m_nFlags;
			eCompressDataType m_nTypeFreq;

			mutable double m_dBitRowSize;
			bool m_bOnlineCalcSize;
			EncoderType m_nType;

	};

	typedef TUnsignedNumLenCompressor<uint64, TFindMostSigBit, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder64, 64> UnsignedNumLenCompressor64;

	typedef TUnsignedNumLenCompressor<uint32, TFindMostSigBit, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder64, 32> UnsignedNumLenCompressor32;


	typedef TUnsignedNumLenCompressor<int64, TFindMostSigBit, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder64, 64> UnsignedNumLenCompressor64i;

	typedef TUnsignedNumLenCompressor<int32, TFindMostSigBit, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder64, 32> UnsignedNumLenCompressor32i;
}
#endif