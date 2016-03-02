#ifndef _EMBEDDED_DATABASE_UNSIGNED_INTEGER_NUMLEN_COMPRESS_H_
#define _EMBEDDED_DATABASE_UNSIGNED_INTEGER_NUMLEN_COMPRESS_H_
#include <map>
#include <vector>
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "BPVector.h"
#include "CommonLibrary/stream.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "CommonLibrary/RangeCoder.h"
#include "NumLenCompress.h"
#include "CompressorParams.h"
namespace embDB
{

	template<class _TStringVal>
	class TStringACCompressor
	{
		public:
			typedef _TStringVal TStringVal;
			typedef embDB::TBPVector<TStringVal> TValueMemSet;

			typedef CommonLib::TRangeEncoder<uint64, 64> TRangeEncoder;
			typedef CommonLib::TRangeDecoder<uint64, 64> TRangeDecoder;

			typedef CommonLib::TACEncoder<uint64, 32> TACEncoder;
			typedef CommonLib::TACDecoder<uint64, 32> TACDecoder;

			enum eTypeFreq
			{
				etfByte = 0,
				etfShort = 1,
				etfInt32 = 2
			};

			TStringACCompressor(CommonLib::alloc_t *pAlloc, CompressorParamsBaseImp *pParams, uint32 nError = 200 /*0.5%*/, bool bOnlineCalcSize = false) : m_nDiffs(0), m_dBitRowSize(0),
				m_nError(nError), m_bOnlineCalcSize(bOnlineCalcSize), m_nTypeFreq(etfByte), m_nCount(0), m_nFlags(0)
			{
				memset(m_nCharFreq, 0, sizeof(m_nCharFreq));
			}

			~TStringACCompressor()
			{

			}

			void AddSymbol(uint32 nSize,  int nIndex, const TStringVal& stringValue, const TValueMemSet& vecValues)
			{
				byte *pBuf = stringValue.m_pBuf;
				for (uint32 i = 0; i < stringValue.m_nLen; ++i)
				{
					AddByte(pBuf[i]);
				}
				AddByte('\0');

			}

			void RemoveSymbol(uint32 nSize,  int nIndex, const TStringVal& stringValue, const TValueMemSet& vecValues)
			{
				byte *pBuf = stringValue.m_pBuf;
				for (uint32 i = 0; i < stringValue.m_nLen; ++i)
				{
					RemoveByte(pBuf[i]);
				}
				RemoveByte('\0');
			}

			void AddByte(byte ch)
			{				
				if(++m_nCharFreq[ch] == 0)
					m_nLetters++;


				m_nCount++;

				uint32 nNewCount = m_nCharFreq[ch];
				if(m_nTypeFreq != etfInt32)
				{
					if(nNewCount > 255)
						m_nTypeFreq = etfShort;
					if(nNewCount > 65535)
						m_nTypeFreq = etfInt32;
				}

				if(!m_bOnlineCalcSize)
					return;

				
				uint32 nOldCount = nNewCount - 1;

				if(m_nLetters > 1)
				{
	
					m_dBitRowSize += (nNewCount * mathUtils::Log2((double)m_nCount/nNewCount));
					if(nOldCount > 0)
						m_dBitRowSize -= nOldCount* mathUtils::Log2((double)(m_nCount - 1)/nOldCount);
					m_dBitRowSize += (m_nCount - nNewCount) * mathUtils::Log2((double)m_nCount/(m_nCount - 1));
				}
				else
					m_dBitRowSize = 0;

			}

			void RemoveByte(byte ch)
			{
				assert(m_nCharFreq[ch] > 0);
				if(--m_nCharFreq[ch] == 1)
					m_nLetters--;

				m_nCount--;

				if(!m_bOnlineCalcSize)
					return;

				uint32 nNewFreq = m_nCharFreq[ch];
				uint32 nOldFreq = nNewFreq + 1;

				if(m_nLetters > 1)
				{

					m_dBitRowSize -= (nOldFreq* mathUtils::Log2((double)(m_nCount + 1)/(nOldFreq)));
					if(nNewFreq > 0)
						m_dBitRowSize += (nNewFreq* mathUtils::Log2((double)(m_nCount)/(nNewFreq)));

					m_dBitRowSize -= (m_nCount - nNewFreq) * mathUtils::Log2((double)(m_nCount + 1)/(m_nCount));

				}
				else
				{
					m_dBitRowSize = 0;

				}

			}


			double GetCodeBitSize() const
			{

				double dBitRowSize = m_bOnlineCalcSize ? m_dBitRowSize :  CalcRowBitSize();

				dBitRowSize  += (dBitRowSize /m_nError)  + 64; 
				return dBitRowSize;
			}

			double CalcRowBitSize() const
			{
				double dBitRowSize =0;
				if(m_nLetters > 1)
				{
					for (uint32 i = 0; i < 256; ++i)
					{	
						if(m_nCharFreq[i] == 0)
							continue;
						double dFreq = m_nCharFreq[i];
						double dLog2 =  mathUtils::Log2((double)m_nCount/dFreq); 
						dBitRowSize  += (dFreq * dLog2);
					}
				}
				return dBitRowSize;

			}
			uint32 GetLenSymbolsFreq() const 
			{
				uint32 nSize = 32; //bit mask

				
				switch(m_nTypeFreq)
				{
				case etfByte:
					nSize += m_nLetters;
					break;
				case etfShort:
					nSize += m_nLetters * sizeof(uint16);
					break;
				case etfInt32:
					nSize += m_nLetters * sizeof(uint32);
					break;
				}
				return nSize;
			}


			uint32 GetCompressSize() const
			{
				double dRowBitsLen = GetCodeBitSize();
				uint32 nByteSize = (dRowBitsLen + 7)/8;
				uint32 nLenSize =  GetLenSymbolsFreq();
				return 1 /*flag*/ + nByteSize + nLenSize;
			}

			bool compress(const TBPVector<TValue>& vecValues, CommonLib::IWriteStream* pStream)
			{


				TBPVector<TValue> vec;
				uint32 nBeginPos = pStream->pos();
				byte nFlag = 0;
				pStream->write(nFlag);

				WriteSymbolsFreq(pStream);

				double dRowBitsLen = GetCodeBitSize();
				uint32 nByteSize = (dRowBitsLen + 7)/8;

				uint32 nBeginCompressPos = pStream->pos();
				bool bRangeCode = true;
				if(m_nLetters > 1)
				{
					if(!CompressRangeCode(vecValues, pStream, nByteSize))
					{
						pStream->seek(nBeginCompressPos, CommonLib::soFromBegin);
						CompressAcCode(vecValues, pStream);
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

	protected:
		void WriteSymbolsFreq(CommonLib::IWriteStream* pStream)
		{

			byte mask[32];

			memset(mask, 0, sizeof(mask));

			for (uint32 i = 0; i < 32 * 8; ++i)
			{
				uint32 nByte = i/8;
				uint32 nBit =  i - (nByte * 8);
				if(m_nCharFreq[i] != 0)
					mask[nByte] |= (0x01 << nBit);
			}

			for (uint32 i = 0; i < 32; ++i)
			{
				pStream->write(mask[i]);
			}

			for (uint32 i = 0; i < 256; ++i)
			{
				 if(m_nCharFreq[i] == 0)
					 continue;
				switch(m_nTypeFreq)
				{
				case etfByte:
					pStream->write((byte)m_nCharFreq[i]);
					break;
				case etfShort:
					pStream->write((uint16)m_nCharFreq[i]);
					break;
				case etfInt32:
					pStream->write((uint32)m_nCharFreq[i]);
					break;
				}
			}
		}


		template <class TEncoder>
		bool Compress(TEncoder& encoder, const TBPVector<TStringVal>& vecValues, CommonLib::IWriteStream* pStream, uint32 *FreqPrev)
		{		 

			assert(vecValues.size() == m_nCount);
			for (size_t i = 1, sz = vecValues.size(); i < sz; ++i)
			{

				const TStringVal& strVal = vecValues[i];

				for (uint32 j = 0; j < strVal.m_nLen; ++j)
				{
					byte ch = strVal.m_pBuf[j];
					if(!encoder.EncodeSymbol(FreqPrev[ch], FreqPrev[ch + 1], m_nCount))
						return false;
				}
							
			}
			return encoder.EncodeFinish();
		}



	protected:
			uint64 m_nCharFreq[257];
			uint32 m_nLetters;
			double m_dBitRowSize;
			uint32 m_nError;
			bool m_bOnlineCalcSize;
			uint32 m_nCount;
			uint16 m_nFlags;
			eTypeFreq m_nTypeFreq;


	};

}
#endif