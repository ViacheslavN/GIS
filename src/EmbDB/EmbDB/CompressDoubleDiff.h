#ifndef _EMBEDDED_DATABASE_DOUBLE_DIFF_COMPRESS_H_
#define _EMBEDDED_DATABASE_DOUBLE_DIFF_COMPRESS_H_
#include "CommonLibrary/FixedBitStream.h"
#include <map>
#include "MathUtils.h"
#include "BPVector.h"
#include "SignCompressor.h"
#include "DoublePartCompress.h"

namespace embDB
{
	typedef union {
		float val;
		struct {
#ifdef __BIG_ENDIAN__
			uint32 sign : 1;
			uint32 exponent : 8;
			uint32 mantisa : 23;
#else
			uint32 mantisa : 23;
			uint32 exponent : 8;
			uint32 sign : 1;
#endif			
		} parts;
	} float_cast;


	typedef union {
		double val;
		struct {

#ifdef __BIG_ENDIAN__
			uint64 sign : 1;
			uint64 exponent : 11;
			uint64 mantisa : 52;
#else
			uint64 mantisa : 52;
			uint64 exponent : 11;
			uint64 sign : 1;
#endif			
		
		} parts;
	} double_cast;

	template <class _TDoubleValue, class _TDoubleCast, class _TSignedValue, uint32 _nMantisaLen, uint32 _nExponentLen>
	class TDoubleCompressDiff
	{
		public:

			typedef _TDoubleValue TValue ;
			typedef _TDoubleCast TDoubleCast;
			typedef _TSignedValue TSignedValue;
			typedef  TBPVector<TValue > TValueMemSet;

			void AddSymbol(uint32 nSize,  int nIndex, TValue nValue, const TValueMemSet& vecValues)
			{
				if(nSize > 1)
				{

					if(nIndex == 0)
					{
						AddDiffSymbol(vecValues[nIndex + 1], nValue); 
					}
					else
					{
						TValue nPrev =  vecValues[nIndex - 1];
						if(nIndex == nSize - 1)
						{
							AddDiffSymbol(nValue, nPrev); 
						}
						else
						{
							TValue nNext =  vecValues[nIndex + 1];
						

							RemoveDiffSymbol(nNext , nPrev);


							AddDiffSymbol(nValue, nPrev); 
							AddDiffSymbol(nNext,  nValue); 
						}
					}
				}


				/*TDoubleCast dVal; 
				dVal.val = nValue;

				m_MantisaCompressor2.AddSymbol(dVal.parts.mantisa);*/
				m_SignCompressor.AddSymbol(nValue < 0);

			}
			void RemoveSymbol(uint32 nSize,  int nIndex, TValue nValue, const TValueMemSet& vecValues)
			{
				if(vecValues.size() > 1)
				{
					if(nIndex == 0)
					{
						RemoveDiffSymbol(vecValues[nIndex + 1] , nValue); 
					}
					else
					{

						if(nIndex == vecValues.size() - 1)
						{

							RemoveDiffSymbol(nValue , vecValues[nIndex - 1]); 
						}
						else
						{
							int64 nPrev =  vecValues[nIndex - 1];

							int64 nNext =  vecValues[nIndex + 1];

							AddDiffSymbol(nNext, nPrev);


							RemoveDiffSymbol(nValue, nPrev); 
							RemoveDiffSymbol(nNext, nValue); 
						}
					}
				}

				m_SignCompressor.RemoveSymbol(value < 0);
			}

			void AddDiffSymbol(TValue nNext, TValue nPrev)
			{
				TDoubleCast dNext, dPrev;

				dNext.val = nNext;
				dPrev.val = nPrev;

				TSignedValue dMantisaNext = (TSignedValue)dNext.parts.mantisa;
				TSignedValue dMantisaPrev = (TSignedValue)dPrev.parts.mantisa;


				TSignedValue dExponentNext = (TSignedValue)dNext.parts.exponent;
				TSignedValue dExponentPrev = (TSignedValue)dPrev.parts.exponent;


				m_MantisaCompressor.AddSymbol(dMantisaNext - dMantisaPrev);
				m_ExponentCompressor.AddSymbol(dExponentNext - dExponentPrev);

			}

			void RemoveDiffSymbol(TValue nNext , TValue nPrev)
			{

				TDoubleCast dNext, dPrev;

				dNext.val = nNext;
				dPrev.val = nPrev;

				TSignedValue dMantisaNext = (TSignedValue)dNext.parts.mantisa/((TSignedValue)2 << 52);
				TSignedValue dMantisaPrev = (TSignedValue)dPrev.parts.mantisa/((TSignedValue)2 << 52);


				TSignedValue dExponentNext = (TSignedValue)dNext.parts.exponent;
				TSignedValue dExponentPrev = (TSignedValue)dPrev.parts.exponent;


				m_MantisaCompressor.RemoveSymbol(dMantisaNext - dMantisaPrev);
				m_ExponentCompressor.RemoveSymbol(dExponentNext - dExponentPrev);
			}



			uint32 GetComressSize() const
			{

				uint32 nSignSize = m_SignCompressor.GetCompressSize();
				uint32 nExpSize = m_ExponentCompressor.GetCompressSize();
				uint32 nManSize = m_MantisaCompressor.GetCompressSize();

			//	uint32 nManSize2 = m_MantisaCompressor2.GetCompressSize();

				return nSignSize + nExpSize + nManSize;
			}
		private:
			typedef TDoublePartCompress<TSignedValue, _nMantisaLen> TMantisaCompressor;
			typedef TDoublePartCompress<TSignedValue, _nExponentLen> TExponentCompressor;

			TMantisaCompressor m_MantisaCompressor;
		//	UnsignedNumLenCompressor64 m_MantisaCompressor2;
			TExponentCompressor m_ExponentCompressor;


			TSignCompressor m_SignCompressor;
			
	};


	typedef TDoubleCompressDiff<double, double_cast, int64, 52, 11> TDoubleDiffCompreessor;
	typedef TDoubleCompressDiff<float, float_cast, int32, 23, 8> TFloatDiffCompreessor;
}

#endif
 