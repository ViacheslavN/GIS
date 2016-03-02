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

	template<class _TValue, uint32 _nBitCount>
	class TUnsignedNumLenNodeCompressor
	{
	public:
		typedef _TValue TValue;
		typedef embDB::TBPVector<TValue> TValueMemSet;
		typedef CommonLib::TRangeEncoder<uint64, 64> TRangeEncoder;
		typedef CommonLib::TRangeDecoder<uint64, 64> TRangeDecoder;

		typedef CommonLib::TACEncoder<uint64, 32> TACEncoder;
		typedef CommonLib::TACDecoder<uint64, 32> TACDecoder;

		typedef TUnsignedNumLenCompressor<TValue, TFindMostSigBit, TRangeEncoder, TACEncoder, 
			TRangeDecoder, TACDecoder, _nBitCount> TCompressor;


		TUnsignedNumLenNodeCompressor(CommonLib::alloc_t *pAlloc, CompressorParamsBaseImp *pParams){}
		~TUnsignedNumLenNodeCompressor(){}

		void AddSymbol(uint32 nSize,  int nIndex, const TValue& value, const TValueMemSet& vecValues)
		{
			m_compressor.AddSymbol(value);
		}
		void RemoveSymbol(uint32 nSize,  int nIndex, const TValue& value, const TValueMemSet& vecValues)
		{
			m_compressor.RemoveSymbol(value);
		}
		uint32 GetComressSize() const
		{
			return m_compressor.GetCompressSize();
		}



		void compress( const TBPVector<TValue>& vecValues, CommonLib::IWriteStream *pStream)
		{
			m_compressor.compress(vecValues, pStream);
		}
		void decompress(uint32 nSize, TBPVector<TValue>& vecValues, CommonLib::IReadStream *pStream)
		{
			m_compressor.decompress(nSize, vecValues, pStream);
		}
		void clear()
		{
			m_compressor.clear();
		}
	private:
		TCompressor m_compressor;
	};
}


#endif