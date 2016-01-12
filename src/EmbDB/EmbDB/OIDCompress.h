#ifndef _EMBEDDED_DATABASE_FIELD_OID_COMPRESS_H_
#define _EMBEDDED_DATABASE_FIELD_OID_COMPRESS_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include <set>
#include <vector>
#include <map>
#include "NumLenCompress.h"
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
			eCopmress

		};

		struct SymbolInfo
		{

			uint16 m_nFreq;
			uint32 m_nB;
			SymbolInfo() : m_nFreq(0), m_nB(0)
			{}
		};

		 typedef std::map<int64, SymbolInfo> TDiffFreq;
		 typedef TUnsignedCalcNumLen<uint64, TFindMostSigBit> TCalcNumLen;
		 typedef TRangeEncoder<uint64, 64> TRangeEncoder;
		 typedef TRangeDecoder<uint64, 64> TRangeDecoder;
		public:

			OIDCompress();
			~OIDCompress();

			uint32 GetRowSize() const;
			void AddSymbol(int64 nDiff);
			void RemoveSymbol(int64 nDiff);

			double Log2( double n )  const
			{  

				return log( n ) / log( (double)2 );  
			}


			void compress(TBPVector<int64>& oids, CommonLib::IWriteStream* pStream);
			void read(uint32 nSize, TBPVector<int64>& oids, CommonLib::IReadStream* pStream);
			double GetRowBitSize() const;
			
	private:
		uint32 GetNeedByteForDiffCompress() const;
		uint32 GetNeedByteForNumLen() const;

		void compressDiffScheme(TBPVector<int64>& oids, CommonLib::IWriteStream* pStream);
		void compressNumLen(TBPVector<int64>& oids, CommonLib::IWriteStream* pStream);


		void readDiffScheme(uint32 nSize, TBPVector<int64>& oids, CommonLib::IReadStream* pStream);

		TDiffFreq m_DiffFreq;
		TCalcNumLen m_CalcNum;
		uint32 m_nCount;

	};

}

#endif