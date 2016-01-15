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
		 typedef TUnsignedCalcNumLen<uint64, TFindMostSigBit, 64> TCalcNumLen;
		 typedef TRangeEncoder<uint32, 32> TRangeEncoder;
		 typedef TRangeDecoder<uint32, 32> TRangeDecoder;
		 typedef std::vector<Symbols> TVecFreq;
		public:

			OIDCompress();
			~OIDCompress();

			uint32 GetRowSize() const;
			void AddSymbol(int64 nDiff);
			void RemoveSymbol(int64 nDiff);

		

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