#ifndef _EMBEDDED_DATABASE_DOUBLE_COMPRESS_H_
#define _EMBEDDED_DATABASE_DOUBLE_COMPRESS_H_
#include "CommonLibrary/FixedBitStream.h"
#include <map>
#include "../MathUtils.h"
#include "BPVector.h"
#include "SignCompressor.h"
#include "DoublePartCompress.h"
#include "CompressDoubleDiff.h"
#include "NumLenCompress.h"
namespace embDB
{


	template <class _TDoubleValue, class _TDoubleCast, class _TSignedValue, uint32 _nMantisaLen, uint32 _nExponentLen>
	class TDoubleCompress
	{
	public:

		typedef _TDoubleValue TValue ;
		typedef _TDoubleCast TDoubleCast;
		typedef _TSignedValue TSignedValue;
		typedef  TBPVector<TValue > TValueMemSet;

		TDoubleCompress() : m_MantisaCompressor(ACCoding), m_ExponentCompressor(ACCoding)
		{

		}

		void AddSymbol(uint32 nSize,  int nIndex, TValue nValue, const TValueMemSet& vecValues)
		{
			 
			TDoubleCast dVal; 
			dVal.val = nValue;


			m_MantisaCompressor.AddSymbol(dVal.parts.mantisa);
			m_ExponentCompressor.AddSymbol(dVal.parts.exponent);

			m_SignCompressor.AddSymbol(nValue < 0);

		}
		void RemoveSymbol(uint32 nSize,  int nIndex, TValue nValue, const TValueMemSet& vecValues)
		{
			TDoubleCast dVal; 
			dVal.val = nValue;
			m_MantisaCompressor.RemoveSymbol(dVal.parts.mantisa);
			m_ExponentCompressor.RemoveSymbol(dVal.parts.exponent);
			m_SignCompressor.RemoveSymbol(nValue < 0);
		}

	 


		uint32 GetCompressSize() const
		{

			uint32 nSignSize = m_SignCompressor.GetCompressSize();
			uint32 nExpSize = m_ExponentCompressor.GetCompressSize();
			uint32 nManSize = m_MantisaCompressor.GetCompressSize();
			return nSignSize + nExpSize + nManSize;
		}
	private:
	 

		typedef TUnsignedNumLenCompressor<uint16, TFindMostSigBit, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
			CommonLib::TRangeDecoder64, CommonLib::TACDecoder64, 16> TExponentCompressor;

		UnsignedNumLenCompressor64 m_MantisaCompressor;
		TExponentCompressor  m_ExponentCompressor;

		TSignCompressor m_SignCompressor;

	};


	typedef TDoubleCompress<double, double_cast, int64, 52, 11> TDoubleCompreessor;
	typedef TDoubleCompress<float, float_cast, int32, 23, 8> TFloatCompreessor;
}

#endif
