#ifndef _EMBEDDED_DATABASE_FIELD_OID_COMPRESS_H_
#define _EMBEDDED_DATABASE_FIELD_OID_COMPRESS_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include <set>
#include <vector>
#include <map>
#include "utils/compress/NumLenCompress.h"
#include "utils/compress/NumLenDiffCompress.h"
#include "utils/compress/IntegerDiffCompress.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "CommonLibrary/RangeCoder.h"
#include "BPVector.h"
#include "CompressorParams.h"
namespace embDB
{
	class OIDCompressor
	{
 
		enum eSchemeCompress
		{
			eCompressDiff = 1,
			eCopmressNumLen

		};

  
		 typedef CommonLib::TRangeEncoder<uint64, 64> TRangeEncoder;
		 typedef CommonLib::TRangeDecoder<uint64, 64> TRangeDecoder;

		 typedef CommonLib::TACEncoder<uint64, 32> TACEncoder;
		 typedef CommonLib::TACDecoder<uint64, 32> TACDecoder;

		 typedef TUnsignedDiffNumLenCompressor<int64, TFindMostSigBit, TRangeEncoder, TACEncoder, 
			 TRangeDecoder, TACDecoder, 64> TNumLenCompressor;
	
		 typedef embDB::TUnsignedIntegerDiffCompress<int64, TRangeEncoder, TACEncoder, 
			 TRangeDecoder, TACDecoder> TDiffCompressor;

	 
		public:

			OIDCompressor(CommonLib::alloc_t *pAlloc, uint32 nPageSize, CompressorParamsBaseImp *pParams , uint32 nError = 200);
			~OIDCompressor();

			void AddSymbol(uint32 nSize,  int nIndex, int64 nOID, const embDB::TBPVector<int64>& vecOIDs);
			void RemoveSymbol(uint32 nSize,  int nIndex, int64 nOID, const embDB::TBPVector<int64>& vecOIDs);
			void RemoveDiffSymbol(int64 nOid);
			uint32 GetCompressSize() const;
		
			bool compress( const embDB::TBPVector<int64>& vecOIDs, CommonLib::IWriteStream *pStream);
			bool decompress(uint32 nSize, embDB::TBPVector<int64>& vecOIDs, CommonLib::IReadStream *pStream);
			void clear();

			void AddDiffSymbol(int64 nDiff);
	private:
	
 

		TDiffCompressor	  m_DiffComp;
		TNumLenCompressor m_NumLenComp;
		 

	};

}

#endif