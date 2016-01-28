#ifndef _EMBEDDED_DATABASE_NUM_LEN_COMPRESS_H_
#define _EMBEDDED_DATABASE_NUM_LEN_COMPRESS_H_

#include "CommonLibrary/FixedBitStream.h"
#include <map>
#include "MathUtils.h"
#include "BPVector.h"
namespace embDB
{
	

	static int bits_lens[] = {0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3};
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
		int FMSB(uint16 val16)
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


		int FMSB(uint32 val32)
		{
			int bits = 0;
		 
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


		int FMSB(uint64 val64)
		{
			int bits = 0;
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



	};

	template<class _TValue, class _TFindMostSigBit, 
			class _TRangeCoder,
			class _TACCoder,
			class _TRangeEncoder,
			class _TACEncoder,
		
			uint32 _nMaxBitsLens>
	class TUnsignedCalcNumLen
	{
		public:

			typedef _TValue TValue;
			typedef _TFindMostSigBit TFindBit;
			typedef _TRangeCoder TRangeCoder;
			typedef _TACCoder TACCoder;

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
	 

			TUnsignedCalcNumLen() : m_nLenBitSize(0), m_nCount(0), m_nDiffsLen(0), m_nFlags(0),
						m_nTypeFreq(etfByte)
			{

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

				m_BitsLensFreq[nBitLen] -= 1;
				if(!m_BitsLensFreq[nBitLen])
					m_nDiffsLen--;
			}
 
			 
			double GetCodeBitSize() const
			{
				double dBitRowSize = 0;
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{
					double dFreq = m_BitsLensFreq[i];
					double dLog2 = -1*mathUtils::Log2(dFreq/m_nCount); 
					dBitRowSize += dFreq* dLog2;

				}

				if(dBitRowSize < 32)
					dBitRowSize = 32;

				dBitRowSize += dBitRowSize/200;  //0.5% error

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



				return nByteSize + 1 + (_nMaxBitsLens)/8 + GetLenForDiffLen(); //Type comp (rang or ac) + 4 +

			}

		

			bool compress(const TBPVector<TValue>& vecValues, CommonLib::IWriteStream* pStream)
			{

				uint16 ModelLens[_nMaxBitsLens];

				uint32 nBeginPos = pStream->pos();
				WriteDiffsLens(pStream);

				double dRowBitsLen = GetCodeBitSize();
				uint32 nByteSize = (dRowBitsLen + 7)/8;


				 

				return true;
			}
			bool decompress(TBPVector<_TValue>& vecValues, CommonLib::IWriteStream* pStream)
			{
				return true;
			}
		private:
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
			}

			void WriteDiffsLens(CommonLib::IWriteStream* pStream)
			{

				byte LensMask[(_nMaxBitsLens)/8];

				memset(LensMask, 0, _nMaxBitsLens/8);
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{
					uint32 nByte = i/8;
					uint32 nBit =  i - (nByte * 8);
					if(m_BitsLensFreq[_nMaxBitsLens] != 0)
						LensMask[nByte] |= (0x01 << nBit);
				}
				for (uint32 i = 0; i < _nMaxBitsLens/8; ++i)
				{
					pStream->write((byte)LensMask[i]);
				}
				
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{
					if(m_BitsLensFreq[_nMaxBitsLens] == 0)
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

		private:	

			TFindBit    m_FindBit;
	  
			uint32 m_nLenBitSize;
			uint32 m_nCount;

			uint32 m_nDiffsLen;

			uint32  m_BitsLensFreq[_nMaxBitsLens];
			uint16 m_nFlags;
			eTypeFreq m_nTypeFreq;
	};
}
#endif