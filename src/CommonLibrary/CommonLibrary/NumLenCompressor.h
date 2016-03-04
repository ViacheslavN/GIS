#ifndef _LIB_COMMON_GEO_SHAPE_COMPRESSOR_H_
#define _LIB_COMMON_GEO_SHAPE_COMPRESSOR_H_
#include "stream.h"
#include "RangeCoder.h"

namespace CommonLib
{


	template <class TValue, uint32 _nMaxBitLens>
	class TNumLemCompressor
	{
		public:
			TNumLemCompressor(){}
			~TNumLemCompressor(){}

			void PreAddSympol(TValue value)
			{

			}


			void BeginCompreess()
			{

			}

			void EncodeSymbol(TValue value)
			{

			}

			void BeginDecode()
			{

			}

			bool DecodeSymbol(TValue& value)
			{

			}


	private:

		uint32 m_nFreqLen[_nMaxBitLens];

	};
}