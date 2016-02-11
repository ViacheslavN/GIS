#ifndef _EMBEDDED_DATABASE_INTEGER_COMPRESS_H_
#define _EMBEDDED_DATABASE_INTEGER_COMPRESS_H_
#include "CommonLibrary/FixedBitStream.h"
#include <map>
#include "MathUtils.h"
#include "BPVector.h"
namespace embDB
{

template<class _TValue, 
			class _TRangeEncoder,
			class _TACEncoder,
			class _TRangeDecoder,
			class _TACDecoder>
	class TUnsignedIntegerDiffCompress
	{
		public:

			typedef _TValue TValue;
			typedef _TRangeEncoder   TRangeEncoder;
			typedef _TACEncoder		 TACEncoder;
			typedef _TRangeDecoder	 TRangeDecoder;
			typedef _TACDecoder		 TACDecoder;

			struct SymbolInfo
			{

				uint32 m_nFreq;
				uint32 m_nB;
				uint32 m_nLow;
				uint32 m_nHight;
			
				SymbolInfo() : m_nFreq(0), m_nB(0), m_nLow(0), m_nHight(0)
				{}

			};
			struct Symbols
			{

				uint32 m_nFreq;
				uint32 m_nB;
				uint64 m_nSymbol;

				uint32 m_nLow;
				uint32 m_nHight;

				Symbols() : m_nFreq(0), m_nB(0), m_nSymbol(0), m_nLow(0), m_nHight(0)
				{}

				Symbols(SymbolInfo si, uint64 nSymbol) : m_nFreq(si.m_nFreq), m_nB(si.m_nB), m_nSymbol(nSymbol)
					, m_nLow(si.m_nLow), m_nHight(si.m_nHight)
				{}

				bool operator < (const Symbols& SymInfo) const
				{
					return m_nLow< SymInfo.m_nFreq;
				}
			};

			typedef std::map<int64, SymbolInfo> TSymbolsFreq;
			typedef std::vector<Symbols> TVecFreq;


			 
			//Flags
			//diff type freq|type comp|
			//type comp      1/0 1-range coder, 0-ac-coder  1 bit
			//type freq value  1-byte, 2-short, 3-int32     2 bit
			 

			enum eTypeFreq
			{
				etfByte = 0,
				etfShort = 1,
				etfInt32 = 2
			};
	 

			TUnsignedIntegerDiffCompress(uint32 nError = 200 /*0.5%*/) : m_nCount(0),  m_nFlags(0),
						m_nTypeFreq(etfByte), m_nError(nError)
			{
				
			}

			void AddSymbol(TValue symbol)
			{
				SymbolInfo& symInfo =  m_SymbolsFreq[symbol];
				symInfo.m_nFreq++;
				m_nCount++;

				
				if(m_nTypeFreq != etfInt32)
				{
					if(symInfo.m_nFreq > 255)
						m_nTypeFreq = etfShort;
					if(symInfo.m_nFreq > 65535)
						m_nTypeFreq = etfInt32;
				}
				
			}

			void RemoveSymbol(TValue symbol)
			{
				SymbolInfo& symInfo =  m_SymbolsFreq[symbol];
				symInfo.m_nFreq--;
				m_nCount--;
			}
 
			 
			double GetCodeBitSize() const
			{
				double dBitRowSize = 0;
				for (TSymbolsFreq::const_iterator it = m_SymbolsFreq.begin(); it != m_SymbolsFreq.end(); ++it)
				{
					const SymbolInfo& info = it->second;
					double dFreq = info.m_nFreq;
					double dLog2 = -1*mathUtils::Log2(dFreq/(double)m_nCount); 
					dBitRowSize += (dFreq * dLog2);

				}
				if(dBitRowSize < 32)
					dBitRowSize = 32;
				dBitRowSize  += (dBitRowSize /m_nError); 
				return dBitRowSize;
			}
		

			uint32 GetCompressSize() const
			{
				double dRowBitsLen = GetCodeBitSize();
				uint32 nByteSize = (dRowBitsLen + 7)/8;
				uint32 nLenSize =  GetLenSymbolsFreq();



				return  sizeof(TValue) + nByteSize + 1 + nLenSize; //Type comp (rang or ac) + 4 +

			}

		

			bool compress(const TBPVector<TValue>& vecValues, TBPVector<_TValue>& vecCheck, CommonLib::IWriteStream* pStream)
			{

				
				 TBPVector<TValue> vec;
				uint32 nBeginPos = pStream->pos();
				byte nFlag = 0;
				pStream->write(nFlag);

				WriteSymbolsFreq(pStream);

				double dRowBitsLen = GetCodeBitSize();
				uint32 nByteSize = (dRowBitsLen + 7)/8;

				pStream->write(vecValues[0]);
				uint32 nBeginCompressPos = pStream->pos();
				bool bRangeCode = true;
				
				if(!CompressRangeCode(vecValues, vecCheck, pStream, nByteSize))
				{
					pStream->seek(nBeginCompressPos, CommonLib::soFromBegin);
					CompressAcCode(vecValues, pStream);
					bRangeCode = false;
				}
		

				uint32 nEndPos = pStream->pos();

				uint32 nCompressSize= nEndPos - nBeginCompressPos;
				 
				if(bRangeCode)
					nFlag |= 0x1;

				pStream->seek(nBeginPos, CommonLib::soFromBegin);

				nFlag |= (((byte)m_nTypeFreq) << 1);
				pStream->write(nFlag);
				pStream->seek(nEndPos, CommonLib::soFromBegin);
				return true;
			}
			bool decompress(TBPVector<_TValue>& vecValues, TBPVector<_TValue>& vecCheck, CommonLib::IReadStream* pStream)
			{
				
				byte nFlag = pStream->readByte();
				bool bRangeCode = nFlag & 0x01;
				m_nTypeFreq = (eTypeFreq)(nFlag>>1);

				TVecFreq vecFreq;
				ReadSymbolsFreq(pStream, vecFreq);


				TValue nBegin;
				pStream->read(nBegin);
				vecValues.push_back(nBegin);
				if(bRangeCode)
					return Decompress<TRangeDecoder>(vecValues, vecCheck, pStream, nBegin, vecFreq);
				else
					return Decompress<TACDecoder>(vecValues, vecCheck, pStream, nBegin, vecFreq);
 
			}
		private:

			bool CompressRangeCode(const TBPVector<_TValue>& vecValues, TBPVector<_TValue>& vecCheck, CommonLib::IWriteStream* pStream, 
				uint32 nMaxByteSize)
			{

				TRangeEncoder rgEncoder(pStream, nMaxByteSize);

				TSymbolsFreq checkSymbolsFreq;

				uint32 FreqPrev[257];
				for(int i = 0;i < 257; i++) 
				{
					FreqPrev[i] = 0;
				}

				std::vector<uint32> vecFreq;
			
				for(int i = 0;i < 257; i++) 
				{
					FreqPrev[i] = 0;
				}
				int32 nPrevF = 0;
				for(int i = 0;i < 256; i++) 
				{
					TSymbolsFreq::iterator it =  m_SymbolsFreq.find(i);
					uint32 nFreq = 0;
					if(it != m_SymbolsFreq.end())
					{
						nFreq = it->second.m_nFreq;
					}
					FreqPrev[i + 1] = nFreq + nPrevF;
					nPrevF = FreqPrev[i + 1];
				}
			

				

				for (size_t i = 0, sz = vecCheck.size(); i < sz; ++i)
				{
				 
					//int64 nDiff = vecValues[i] - vecValues[i - 1];
					int64 nDiff = vecCheck[i];
					/*checkSymbolsFreq[nDiff].m_nFreq++;
					int64 nCheckDiff = vecCheck[i-1];
					if(nDiff != nCheckDiff)
					{
						int dd = 0;
						dd++;
					}*/
					
					
					TSymbolsFreq::iterator it =  m_SymbolsFreq.find(nDiff);
					assert(it != m_SymbolsFreq.end());
					SymbolInfo& info = it->second;
					int32 nPrevB = info.m_nB - info.m_nFreq;

					if(nPrevB != FreqPrev[nDiff] || info.m_nB != FreqPrev[nDiff+1])
					{
						int dd = 0;
						dd++;
					}

					if(!rgEncoder.EncodeSymbol(nPrevB, info.m_nB, vecCheck.size()))
					//if(!rgEncoder.EncodeSymbol(FreqPrev[nDiff], FreqPrev[nDiff+1], vecCheck.size()))
						return false;
				}
				return rgEncoder.EncodeFinish();
			}

			void CompressAcCode(const TBPVector<_TValue>& vecValues, CommonLib::IWriteStream* pStream)
			{
				TACEncoder acEncoder(pStream);

				for (size_t i = 1, sz = vecValues.size(); i < sz; ++i)
				{
					int64 nDiff = vecValues[i] - vecValues[i - 1];
					TSymbolsFreq::iterator it =  m_SymbolsFreq.find(nDiff);
					assert(it != m_SymbolsFreq.end());
					SymbolInfo& info = it->second;
					int32 nPrevB = info.m_nB - info.m_nFreq;
					acEncoder.EncodeSymbol(nPrevB, info.m_nB, m_nCount);
				}
				acEncoder.EncodeFinish();
			}

			uint32 GetLenSymbolsFreq() const 
			{
				uint32 nSize = sizeof(uint16);

				nSize += (m_SymbolsFreq.size() * sizeof(TValue));
				switch(m_nTypeFreq)
				{
					case etfByte:
						nSize += m_SymbolsFreq.size();
						break;
					case etfShort:
						nSize +=m_SymbolsFreq.size() * sizeof(uint16);
						break;
					case etfInt32:
						nSize +=m_SymbolsFreq.size() * sizeof(uint32);
						break;
				}
				return nSize;
			}

			void WriteSymbolsFreq(CommonLib::IWriteStream* pStream)
			{

				pStream->write(uint16(m_SymbolsFreq.size()));
				int32 nPrevF = 0;
				for (TSymbolsFreq::iterator it = m_SymbolsFreq.begin(); it != m_SymbolsFreq.end(); ++it)
				{
					SymbolInfo& info = it->second;

				 
					info.m_nB = nPrevF + info.m_nFreq;
					nPrevF = info.m_nB;

					pStream->write(it->first);

					switch(m_nTypeFreq)
					{
					case etfByte:
						pStream->write((byte)info.m_nFreq);
						break;
					case etfShort:
						pStream->write((uint16)info.m_nFreq);
						break;
					case etfInt32:
						pStream->write((uint32)info.m_nFreq);
						break;
					}
				}
				
				 
				
			}


			void ReadSymbolsFreq(CommonLib::IReadStream* pStream, TVecFreq& vecFreq)
			{

				uint32 nSize = pStream->readintu16();
				uint32 nPrevF = 0;
				for (uint32 i = 0; i < nSize; ++i)
				{
					int64 nDiff = pStream->readInt64();
					uint32 nFreq = 0;

					switch(m_nTypeFreq)
					{
					case etfByte:
						nFreq = pStream->readByte();
						m_nCount += nFreq;
						break;
					case etfShort:
						nFreq = pStream->readintu16();
						m_nCount += nFreq;
						break;
					case etfInt32:
						nFreq = pStream->readIntu32();
						m_nCount += nFreq;
						break;
					}

					SymbolInfo info;
					info.m_nFreq = nFreq;
					info.m_nB = nPrevF + info.m_nFreq;
				

					info.m_nLow = nPrevF;
					info.m_nHight = info.m_nLow + nFreq;

					nPrevF = info.m_nB;
					m_SymbolsFreq.insert(std::make_pair(nDiff, info));

					vecFreq.push_back(Symbols(info, nDiff));
				}
				std::sort(vecFreq.begin(), vecFreq.end());
			}

			

			template<class TDecoder>
			bool Decompress(TBPVector<_TValue>& vecValues, TBPVector<_TValue>& vecCheck, CommonLib::IReadStream* pStream, TValue nBegin, TVecFreq& vecFreq)
			{
			

				TDecoder decoder(pStream);
				decoder.StartDecode();
				Symbols sysInfo;


				uint32 FreqPrev[257];
				for(int i = 0;i < 257; i++) 
				{
					FreqPrev[i] = 0;
				}


				for(int i = 0;i < 257; i++) 
				{
					FreqPrev[i] = 0;
				}
				int32 nPrevF = 0;
				for(int i = 0;i < 256; i++) 
				{
					TSymbolsFreq::iterator it =  m_SymbolsFreq.find(i);
					uint32 nFreq = 0;
					if(it != m_SymbolsFreq.end())
					{
						nFreq = it->second.m_nFreq;
					}
					FreqPrev[i + 1] = nFreq + nPrevF;
					nPrevF = FreqPrev[i + 1];
				}
				std::vector<uint32> vecFreq1;
				for(int i = 0;i < 256; i++) 
				{
					vecFreq1.push_back(FreqPrev[i]);
				}
				for (size_t i = 0; i < m_nCount; ++i)
				{ 
					uint32 freq = decoder.GetFreq(m_nCount);

					sysInfo.m_nFreq = freq;
					TVecFreq::iterator it = std::lower_bound(vecFreq.begin(), vecFreq.end(), sysInfo);
					if(it == vecFreq.end())
						it = vecFreq.end() -1;
					else if(it != vecFreq.begin())
					{
						if(it->m_nLow > freq)
							it--;
					}



					int64 nSymbolFind = 0;

					for (TSymbolsFreq::iterator it = m_SymbolsFreq.begin(); it != m_SymbolsFreq.end(); ++it)
					{
						SymbolInfo& info = it->second;
						if(info.m_nLow <= freq && freq < info.m_nHight)
						{
							nSymbolFind = it->first;
						}

					}


					std::vector<uint32>::iterator it1  = std::lower_bound(vecFreq1.begin(), vecFreq1.end(), freq); 
				
				 

					int32 Symbol;
					for(Symbol = 255;FreqPrev[Symbol] > freq;Symbol--);

					//m_pWriteStream.write(Symbol);
				//	coder.DecodeSymbol(FreqPrev[Symbol], FreqPrev[Symbol+1], nFileSize);
				 
					if( it->m_nSymbol != Symbol)
					{
						int dd = 0;
						dd++;
					}
					 
					vecCheck.push_back(Symbol);
					nBegin += it->m_nSymbol;
					vecValues.push_back(nBegin);

					uint32 nPrevB = it->m_nB - it->m_nFreq;
					if(nPrevB != FreqPrev[Symbol] || it->m_nB != FreqPrev[Symbol+1])
					{
						int dd = 0;
						dd++;
					}
					decoder.DecodeSymbol( nPrevB, it->m_nB, m_nCount);
				//	decoder.DecodeSymbol( FreqPrev[Symbol], FreqPrev[Symbol+1], m_nCount);

				}

				return true;
			}

		private:	

	

			TSymbolsFreq m_SymbolsFreq;
			uint32 m_nError;
			uint32 m_nCount;
			uint16 m_nFlags;
			eTypeFreq m_nTypeFreq;
	};
}

#endif