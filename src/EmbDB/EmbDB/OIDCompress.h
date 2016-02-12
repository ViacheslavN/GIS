#ifndef _EMBEDDED_DATABASE_FIELD_OID_COMPRESS_H_
#define _EMBEDDED_DATABASE_FIELD_OID_COMPRESS_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include <set>
#include <vector>
#include <map>
#include "NumLenCompress.h"
#include "NumLenDiffCompress.h"
#include "IntegerDiffCompress.h"
#include "ArithmeticCoder.h"
#include "RangeCoder.h"
#include "BPVector.h"
namespace embDB
{
	class OIDCompress
	{
 
		enum eSchemeCompress
		{
			eCompressDiff = 1,
			eCopmressNumLen

		};

  
		 typedef TRangeEncoder<uint64, 64> TRangeEncoder;
		 typedef TRangeDecoder<uint64, 64> TRangeDecoder;

		 typedef TACEncoder<uint64, 32> TACEncoder;
		 typedef TACDecoder<uint64, 32> TACDecoder;

		 typedef TUnsignedDiffNumLenCompressor<int64, TFindMostSigBit, TRangeEncoder, TACEncoder, 
			 TRangeDecoder, TACDecoder, 64> TNumLenCompressor;
	
		 typedef embDB::TUnsignedIntegerDiffCompress<int64, TRangeEncoder, TACEncoder, 
			 TRangeDecoder, TACDecoder> TDiffCompressor;

	 
		public:

			OIDCompress(uint32 nError = 200);
			~OIDCompress();

			void AddSymbol(uint32 nSize,  int nIndex, int64 nOID, const embDB::TBPVector<int64>& vecOIDs);
			void RemoveSymbol(uint32 nSize,  int nIndex, int64 nOID, const embDB::TBPVector<int64>& vecOIDs);
			void RemoveDiffSymbol(int64 nOid);
			uint32 GetComressSize() const;
		
			void compress( const embDB::TBPVector<int64>& vecOIDs, CommonLib::IWriteStream *pStream);
			void decompress(uint32 nSize, embDB::TBPVector<int64>& vecOIDs, CommonLib::IReadStream *pStream);
			void clear();

			void AddDiffSymbol(int64 nDiff);
	private:
	
 

		TDiffCompressor	  m_DiffComp;
		TNumLenCompressor m_NumLenComp;
		 

	};

}

#endif