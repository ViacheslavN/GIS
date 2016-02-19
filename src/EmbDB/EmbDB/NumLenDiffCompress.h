#ifndef _EMBEDDED_DATABASE_NUM_LEN_DIFF_COMPRESS_H_
#define _EMBEDDED_DATABASE_NUM_LEN_DIFF_COMPRESS_H_

#include "CommonLibrary/FixedBitStream.h"
#include <map>
#include "MathUtils.h"
#include "BPVector.h"
namespace embDB
{
	

	template<class _TValue, class _TFindMostSigBit, 
			class _TRangeEncoder,
			class _TACEncoder,
			class _TRangeDecoder,
			class _TACDecoder,		
			uint32 _nMaxBitsLens>
	class TUnsignedDiffNumLenCompressor
	{
		public:

			typedef _TValue TValue;
			typedef _TFindMostSigBit TFindBit;
			typedef _TRangeEncoder   TRangeEncoder;
			typedef _TACEncoder		 TACEncoder;
			typedef _TRangeDecoder	 TRangeDecoder;
			typedef _TACDecoder		 TACDecoder;

			//Flags
			//diff lens|type freq|type comp|
			//type comp      1/0 1-range coder, 0-ac-coder  1 bit
			//type freq value  1-byte, 2-short, 3-int32     2 bit
			 

			enum eTypeFreq
			{
				etfByte = 0,
				etfShort = 1,
				etfInt32 = 2
			};
	 

			TUnsignedDiffNumLenCompressor(uint32 nError = 200 /*0.5%*/) : m_nLenBitSize(0), m_nCount(0), m_nDiffsLen(0), m_nFlags(0),
						m_nTypeFreq(etfByte), m_nError(nError)
			{
				 memset(m_BitsLensFreq, 0, sizeof(m_BitsLensFreq));
			}

			uint16 AddSymbol(TValue symbol)
			{
				uint16 nBitLen =  m_FindBit.FMSB(symbol);

				assert(nBitLen < _nMaxBitsLens);

				m_nLenBitSize += nBitLen;
				m_nCount++;

				if(!m_BitsLensFreq[nBitLen])
					m_nDiffsLen++;

				m_BitsLensFreq[nBitLen] += 1;

				if(m_nTypeFreq != etfInt32)
				{
					if(m_BitsLensFreq[nBitLen] > 255)
						m_nTypeFreq = etfShort;
					if(m_BitsLensFreq[nBitLen] > 65535)
						m_nTypeFreq = etfInt32;
				}
				
				return nBitLen;
			}

			void RemoveSymbol(TValue symbol)
			{
				uint16 nBitLen =  m_FindBit.FMSB(symbol);
				m_nLenBitSize -= nBitLen;
				m_nCount--;

				assert(m_BitsLensFreq[nBitLen] );
			

				m_BitsLensFreq[nBitLen] -= 1;
				if(!m_BitsLensFreq[nBitLen])
					m_nDiffsLen--;
			}
 
			 
			double GetCodeBitSize() const
			{
				double dBitRowSize = 0;
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{
					if(m_BitsLensFreq[i] == 0)
						continue;
					double dFreq = m_BitsLensFreq[i];
					double dLog2 = mathUtils::Log2((double)m_nCount/dFreq); 
					dBitRowSize += (dFreq* dLog2);

				}
				if(dBitRowSize < 64)
					dBitRowSize = 64;

				dBitRowSize += (dBitRowSize/m_nError); 

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



				return nByteSize + 1 + (_nMaxBitsLens)/8 + GetLenForDiffLen() + (m_nLenBitSize +7)/8 + sizeof(uint16) + sizeof(TValue); //Type comp (rang or ac) + 4 +

			}

		

			bool compress(const TBPVector<TValue>& vecValues, CommonLib::IWriteStream* pStream)
			{

				assert(m_nCount == (vecValues.size() - 1));
				 TBPVector<TValue> vec;
				uint32 nBeginPos = pStream->pos();
				byte nFlag = 0;
				pStream->write(nFlag);

				WriteDiffsLens(pStream);

				double dRowBitsLen = GetCodeBitSize();
				uint32 nByteSize = (dRowBitsLen + 7)/8;

				

				uint32 FreqPrev[_nMaxBitsLens + 1];
				memset(&FreqPrev, 0, sizeof(uint32) * _nMaxBitsLens);

				int32 nPrevF = 0;
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{

					FreqPrev[i + 1] = m_BitsLensFreq[i] + nPrevF;
					nPrevF = FreqPrev[i + 1];
				}

				CommonLib::FxBitWriteStream bitStream;
				uint32 nBitSize = (m_nLenBitSize +7)/8;
				
				pStream->write(vecValues[0]);
				pStream->write((uint16)nBitSize);
				bitStream.attach(pStream, pStream->pos(), nBitSize);
				pStream->seek(nBitSize, CommonLib::soFromCurrent);
				uint32 nBeginCompressPos = pStream->pos();
				bool bRangeCode = true;
				
				if(!CompressRangeCode(vecValues, pStream, FreqPrev, nByteSize, &bitStream))
				{
					bitStream.seek(0, CommonLib::soFromBegin);
					pStream->seek(nBeginCompressPos, CommonLib::soFromBegin);
					CompressAcCode(vecValues, pStream,  FreqPrev,  &bitStream);
					bRangeCode = false;
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
			bool decompress(TBPVector<_TValue>& vecValues, CommonLib::IReadStream* pStream)
			{
				
				byte nFlag = pStream->readByte();
				bool bRangeCode = nFlag & 0x01;
				m_nTypeFreq = (eTypeFreq)(nFlag>>1);
				ReadDiffsLens(pStream);


		 

				uint32 FreqPrev[_nMaxBitsLens + 1];
				memset(&FreqPrev, 0, sizeof(uint32) * _nMaxBitsLens);

				int32 nPrevF = 0;
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{

					FreqPrev[i + 1] = m_BitsLensFreq[i] + nPrevF;
					nPrevF = FreqPrev[i + 1];
				}

				TValue nBegin = 0;
				pStream->read(nBegin);
				vecValues.push_back(nBegin);
				uint16 nBitSize = pStream->readintu16();

				CommonLib::FxBitReadStream bitStream;

		 
				bitStream.attach(pStream, pStream->pos(), nBitSize);
				pStream->seek(nBitSize, CommonLib::soFromCurrent);

				if(bRangeCode)
					return Decompress<TRangeDecoder>(vecValues, pStream, FreqPrev, &bitStream, nBegin);
				else
					return Decompress<TACDecoder>(vecValues, pStream, FreqPrev, &bitStream, nBegin);

				//DecompressRangeCode(vecValues, pStream, FreqPrev, &bitStream);
		 
			}
			void clear()
			{
				m_nLenBitSize = 0;
				m_nCount = 0;
				m_nDiffsLen = 0;
				m_nTypeFreq = etfByte;
				m_nFlags = 0;
				memset(m_BitsLensFreq, 0, sizeof(m_BitsLensFreq));
			}
		private:

			bool CompressRangeCode(const TBPVector<_TValue>& vecValues, CommonLib::IWriteStream* pStream, uint32 *FreqPrev, uint32 nMaxByteSize,
				CommonLib::FxBitWriteStream *pBitStream)
			{

				TRangeEncoder rgEncoder(pStream, nMaxByteSize);

				for (uint32 i = 1, sz = vecValues.size(); i< sz; ++i)
				{
				
					_TValue nDiff = vecValues[i] - vecValues[i - 1];
					uint16 nBitLen =  m_FindBit.FMSB(nDiff);

					assert(m_BitsLensFreq[nBitLen] != 0);


					pBitStream->writeBits(nDiff, nBitLen);
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
					_TValue nDiff = vecValues[i] - vecValues[i - 1];
					uint16 nBitLen =  m_FindBit.FMSB(nDiff);
			 

					pBitStream->writeBits(nDiff, nBitLen);
					acEncoder.EncodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen + 1], m_nCount);
						
				}


				acEncoder.EncodeFinish();
			}

			uint32 GetLenForDiffLen() const 
			{
				switch(m_nTypeFreq)
				{
					case etfByte:
						return m_nDiffsLen;
						break;
					case etfShort:
						return m_nDiffsLen * sizeof(uint16);
						break;
					case etfInt32:
						return m_nDiffsLen * sizeof(uint32);
						break;
				}
				assert(false);
				return m_nDiffsLen;
			}

			void WriteDiffsLens(CommonLib::IWriteStream* pStream)
			{

				byte LensMask[(_nMaxBitsLens)/8];

				memset(LensMask, 0, _nMaxBitsLens/8);
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{
					uint32 nByte = i/8;
					uint32 nBit =  i - (nByte * 8);
					if(m_BitsLensFreq[i] != 0)
						LensMask[nByte] |= (0x01 << nBit);
				}
				for (uint32 i = 0; i < _nMaxBitsLens/8; ++i)
				{
					pStream->write((byte)LensMask[i]);
				}
				
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{
					if(m_BitsLensFreq[i] == 0)
						continue;

					switch(m_nTypeFreq)
					{
					case etfByte:
						pStream->write((byte)m_BitsLensFreq[i]);
						break;
					case etfShort:
						pStream->write((uint16)m_BitsLensFreq[i]);
						break;
					case etfInt32:
						pStream->write((uint32)m_BitsLensFreq[i]);
						break;
					}

				}
				
			}


			void ReadDiffsLens(CommonLib::IReadStream* pStream)
			{

				byte LensMask[(_nMaxBitsLens)/8];

			/*	memset(LensMask, 0, _nMaxBitsLens/8);
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{
					uint32 nByte = i/8;
					uint32 nBit =  i - (nByte * 8);
					if(m_BitsLensFreq[i] != 0)
						LensMask[nByte] |= (0x01 << nBit);
				}*/
				for (uint32 i = 0; i < _nMaxBitsLens/8; ++i)
				{
					LensMask[i] = pStream->readByte();

					
				}

				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{

					uint32 nByte = i/8;
					uint32 nBit  =  i - (nByte * 8);

					if(!(LensMask[nByte] & (0x01 << nBit)))
					{
						m_BitsLensFreq[i] = 0;
						continue;
					}

					switch(m_nTypeFreq)
					{
					case etfByte:
						m_BitsLensFreq[i] = pStream->readByte();
						m_nCount += m_BitsLensFreq[i];
						break;
					case etfShort:
						m_BitsLensFreq[i] = pStream->readintu16();
						m_nCount += m_BitsLensFreq[i];
						break;
					case etfInt32:
						m_BitsLensFreq[i] = pStream->readIntu32();
						m_nCount += m_BitsLensFreq[i];
						break;
					}

				}

			}

			

			template<class TDecoder>
			bool Decompress(TBPVector<_TValue>& vecValues, CommonLib::IReadStream* pStream, uint32 *FreqPrev, 
				CommonLib::FxBitReadStream *pBitStream, TValue nBegin)
			{


				TDecoder decoder(pStream);
				decoder.StartDecode();
				TValue value = 0;
				for (size_t i = 0; i < m_nCount; ++i)
				{
					uint32 freq = decoder.GetFreq(m_nCount);

					//uint32 nBitLen;
					//for(nBitLen = _nMaxBitsLens;FreqPrev[nBitLen] > freq;nBitLen--);
					int32 nBitLen = CommonLib::upper_bound(FreqPrev, _nMaxBitsLens, freq);
					if(nBitLen != 0)
						nBitLen--;

					
					pBitStream->readBits(value, nBitLen);
					nBegin += value;
					vecValues.push_back(nBegin);

					decoder.DecodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen+1], m_nCount);

				}

				return true;
			}


			

		private:	

			TFindBit    m_FindBit;
			uint32 m_nError;
	  
			uint32 m_nLenBitSize;
			uint32 m_nCount;

			uint32 m_nDiffsLen;

			uint32  m_BitsLensFreq[_nMaxBitsLens];
			uint16 m_nFlags;
			eTypeFreq m_nTypeFreq;
	};
}
#endif