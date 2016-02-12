#ifndef _EMBEDDED_DATABASE_LINK_COMPRESS_H_
#define _EMBEDDED_DATABASE_LINK_COMPRESS_H_
#include <map>
#include <vector>
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "BPVector.h"
#include "CommonLibrary/stream.h"
#include "ArithmeticCoder.h"
#include "RangeCoder.h"
#include "NumLenCompress.h"
namespace embDB
{

	class InnerLinkCompress
	{
		public:

			typedef TRangeEncoder<uint64, 64> TRangeEncoder;
			typedef TRangeDecoder<uint64, 64> TRangeDecoder;

			typedef TACEncoder<uint64, 32> TACEncoder;
			typedef TACDecoder<uint64, 32> TACDecoder;

			typedef TUnsignedNumLenCompressor<int64, TFindMostSigBit, TRangeEncoder, TACEncoder, 
				TRangeDecoder, TACDecoder, 64> TCompressor;


			InnerLinkCompress();
			~InnerLinkCompress();

			void AddLink(int64 nLink);
			void RemoveLink(int64 nLink);
			uint32 GetComressSize() const;



			void compress( const embDB::TBPVector<int64>& vecLinks, CommonLib::IWriteStream *pStream);
			void decompress(uint32 nSize, embDB::TBPVector<int64>& vecLinks, CommonLib::IReadStream *pStream);
			void clear();
	private:
			TCompressor m_compressor;
	};
}


#endif