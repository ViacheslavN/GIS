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
			eCopmressNumLen

		};

		struct SymbolInfo
		{

			uint16 m_nFreq;
			uint32 m_nB;
			SymbolInfo() : m_nFreq(0), m_nB(0)
			{}
 
		};


		struct Symbols
		{

			uint16 m_nFreq;
			uint32 m_nB;
			uint64 m_nDiff;
			Symbols() : m_nFreq(0), m_nB(0), m_nDiff(0)
			{}

			Symbols(SymbolInfo si, uint64 nDiff) : m_nFreq(si.m_nFreq), m_nB(si.m_nB), m_nDiff(nDiff)
			{}

			bool operator < (const Symbols& SymInfo) const
			{
				return m_nB < SymInfo.m_nB;
			}
		};

		 typedef std::map<int64, SymbolInfo> TDiffFreq;
		 typedef TRangeEncoder<int64, 32> TRangeEncoder;
		 typedef TRangeDecoder<int64, 32> TRangeDecoder;

		 typedef TACEncoder<int64, 32> TACEncoder;
		 typedef TACDecoder<int64, 32> TACDecoder;

		 typedef TUnsignedNumLenCompressor<uint64, TFindMostSigBit, TRangeEncoder, TACEncoder, 
			 TRangeDecoder, TACDecoder, 64> TNumLenCompressor;
	

	

		 typedef std::vector<Symbols> TVecFreq;
		public:

			OIDCompress(uint32 nError = 200);
			~OIDCompress();

			void AddDiffSymbol(int64 nOid);
			void RemoveDiffSymbol(int64 nOid);
			uint32 GetComressSize() const;
		
			void compress( const embDB::TBPVector<int64>& vecLinks, CommonLib::IWriteStream *pStream);
			void decompress(uint32 nSize, embDB::TBPVector<int64>& vecLinks, CommonLib::IReadStream *pStream);
			
	private:
		uint32 GetNeedByteForDiffCompress() const;


		void compressDiffScheme(const TBPVector<int64>& oids, CommonLib::IWriteStream* pStream);
		void compressNumLen(const TBPVector<int64>& oids, CommonLib::IWriteStream* pStream);


		void readDiffScheme(uint32 nSize, TBPVector<int64>& oids, CommonLib::IReadStream* pStream);

		TDiffFreq m_DiffFreq;
		TNumLenCompressor m_NumLenComp;
		uint32 m_nCount;
		uint32 m_nError;

	};

}

#endif