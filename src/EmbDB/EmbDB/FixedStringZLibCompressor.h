#ifndef _EMBEDDED_DATABASE_FIXED_STRING_Z_LIB_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_FIXED_STRING_Z_LIB_COMPRESSOR_H_
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
#include "SignedNumLenDiffCompressor2.h"
#include "StringVal.h"

namespace embDB
{

 
	class TFixedStringZlibCompressor
	{
	public:

		static const uint32 ___nNullTerminatedSymbol = 256;
 
		typedef embDB::TBPVector<sFixedStringVal> TValueMemSet;

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

		TFixedStringZlibCompressor(CommonLib::alloc_t *pAlloc, uint32 nPageSize, CompressorParamsBaseImp *pParams, uint32 nError = 200 ): 
			m_nStrings(0), m_nPageSize(nPageSize)
		{


		}

		~TFixedStringZlibCompressor()
		{

		}
		void init(TValueMemSet* pVecValues)
		{

		}


		void AddSymbol(uint32 nSize,  int nIndex, const sFixedStringVal& nValue, const TValueMemSet& vecValues)
		{
			if(nSize > 1)
			{
				if(nIndex == 0)
				{
					AddDiffSymbol(vecValues[nIndex + 1].m_nLen - nValue.m_nLen); 
				}
				else
				{
					const sFixedStringVal& nPrev =  vecValues[nIndex - 1];
					if(nIndex == nSize - 1)
					{
						AddDiffSymbol(nValue.m_nLen - nPrev.m_nLen); 
					}
					else
					{
						const sFixedStringVal& nNext =  vecValues[nIndex + 1];
						int32 nOldLen = nNext.m_nLen - nPrev.m_nLen;

						RemoveDiffSymbol(nOldLen);


						AddDiffSymbol(nValue.m_nLen - nPrev.m_nLen); 
						AddDiffSymbol(nNext.m_nLen - nValue.m_nLen); 
					}
				}
			}

			AddString(nValue);
		}


		void RemoveSymbol(uint32 nSize,  int nIndex, const sFixedStringVal& nValue, const TValueMemSet& vecValues)
		{
			if(vecValues.size() > 1)
			{
				if(nIndex == 0)
				{
					RemoveDiffSymbol(vecValues[nIndex + 1].m_nLen - nValue.m_nLen); 
				}
				else
				{

					if(nIndex == vecValues.size() - 1)
					{

						RemoveDiffSymbol(nValue.m_nLen - vecValues[nIndex - 1].m_nLen); 
					}
					else
					{
						const sFixedStringVal& nPrev =  vecValues[nIndex - 1];

						const sFixedStringVal& nNext =  vecValues[nIndex + 1];
						int32 nNewSymbol = nNext.m_nLen - nPrev.m_nLen;

						AddDiffSymbol(nNewSymbol);


						RemoveDiffSymbol(nValue.m_nLen - nPrev.m_nLen); 
						RemoveDiffSymbol(nNext.m_nLen - nValue.m_nLen); 
					}
				}
			}

			RemoveString(nValue);
		}
		void RemoveDiffSymbol(int32 nLen)
		{
			 m_lenCompressor.RemoveSymbol(nLen);
		}
		void AddDiffSymbol(int32 nLen)
		{
			  m_lenCompressor.AddSymbol(nLen);
		}

		void AddString(const sFixedStringVal& string)
		{

		}


		void RemoveString(const sFixedStringVal& string)
		{

		}

	
		uint32 GetComressSize() const
		{
		 
		}
		void Free()
		{

		}
		void clear()
		{

		}
		bool compress(const TValueMemSet& vecValues, CommonLib::IWriteStream* pStream)
		{			
			return true;
		}
		bool decompress(uint32 nSize, TValueMemSet& vecValues, CommonLib::FxMemoryReadStream *pStream)
		{

			return true;
		}
 	protected:
		uint32 m_nStrings;
		uint32 m_nPageSize;
		SignedDiffNumLenCompressor232i   m_lenCompressor;
		

		struct sStringBloc
		{
			uint32 m_nRowSize;
			uint32 m_nBeginIndex;
			uint32 m_nCount;

			CommonLib::CWriteMemoryStream m_compressBloc;
		};


		sStringBloc *m_pCurrBloc;

	};

}
#endif