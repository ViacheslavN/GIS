#ifndef _EMBEDDED_DATABASE_LINK_COMPRESS_H_
#define _EMBEDDED_DATABASE_LINK_COMPRESS_H_
#include <map>
#include <vector>
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "BPVector.h"
#include "CommonLibrary/stream.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "CommonLibrary/RangeCoder.h"
#include "NumLenDiffCompress.h"
#include "CompressorParams.h"
namespace embDB
{

	class InnerLinkCompress
	{
		public:

			/*typedef CommonLib::TRangeEncoder<uint64, 64> TRangeEncoder;
			typedef CommonLib::TRangeDecoder<uint64, 64> TRangeDecoder;

			typedef CommonLib::TACEncoder<uint64, 32> TACEncoder;
			typedef CommonLib::TACDecoder<uint64, 32> TACDecoder;

			typedef TUnsignedNumLenCompressor<int64, TFindMostSigBit, TRangeEncoder, TACEncoder, 
				TRangeDecoder, TACDecoder, 64> TCompressor;*/


			InnerLinkCompress(CommonLib::alloc_t *pAlloc, CompressorParamsBaseImp *pParams);
			~InnerLinkCompress();

			void AddSymbol(uint32 nSize,  int nIndex, int64 nLink, const embDB::TBPVector<int64>& vecLinks);
			void RemoveSymbol(uint32 nSize,  int nIndex, int64 nLink, const embDB::TBPVector<int64>& vecLinks);
			 
			uint32 GetComressSize() const;



			void compress( const embDB::TBPVector<int64>& vecLinks, CommonLib::IWriteStream *pStream);
			void decompress(uint32 nSize, embDB::TBPVector<int64>& vecLinks, CommonLib::IReadStream *pStream);
			void clear();
	private:
			UnsignedNumLenCompressor64i m_compressor;
	};
}


#endif