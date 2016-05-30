#ifndef _EMBEDDED_DATABASE_INTEGER_DIFF_COMPRESS_H_
#define _EMBEDDED_DATABASE_INTEGER_DIFF_COMPRESS_H_
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
				uint32 m_nLow;
				uint32 m_nHight;
			
				SymbolInfo() : m_nFreq(0), m_nLow(0), m_nHight(0)
				{}

			};
			struct Symbol
			{

				uint64 m_nSymbol;
				uint32 m_nLow;
				uint32 m_nHight;

				Symbol() :  m_nSymbol(0), m_nLow(0), m_nHight(0)
				{}

				Symbol(SymbolInfo si, uint64 nSymbol) :  m_nSymbol(nSymbol)
					, m_nLow(si.m_nLow), m_nHight(si.m_nHight)
				{}

				bool operator < (const Symbol& SymInfo) const
				{
					return m_nLow < SymInfo.m_nLow;
				}
			};

			typedef std::map<int64, SymbolInfo> TSymbolsFreq;
			typedef std::vector<Symbol> TVecFreq;


			 
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
	 

			TUnsignedIntegerDiffCompress(uint32 nError = 10 /*0.5%*/, bool bOnlineCalcSize = false) : m_nCount(0),  m_nFlags(0),
						m_nTypeFreq(etfByte), m_nError(nError), m_dBitRowSize(0), m_bOnlineCalcSize(bOnlineCalcSize)
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
				
				
				if(!m_bOnlineCalcSize)
					return;

				uint32 nOldCount = symInfo.m_nFreq - 1;
				uint32 nNewCount = nOldCount + 1;


				if(m_SymbolsFreq.size() > 1)
				{
					//m_dBitRowSize += (nNewCount * mathUtils::Log2((double)m_nCount/nNewCount) -  
					//	nOldCount* mathUtils::Log2((double)(m_nCount - 1)/nOldCount) + (m_nCount - nNewCount) * mathUtils::Log2((double)m_nCount/(m_nCount - 1)));

					m_dBitRowSize += (nNewCount * mathUtils::Log2((double)m_nCount/nNewCount));
					if(nOldCount > 0)
						m_dBitRowSize -= nOldCount* mathUtils::Log2((double)(m_nCount - 1)/nOldCount);

					m_dBitRowSize += (m_nCount - nNewCount) * mathUtils::Log2((double)m_nCount/(m_nCount - 1));

				}


			/*	double dBitRowSize = CalcRowBitSize();
				if(fabs(m_dBitRowSize - dBitRowSize) > 0.00000001)
				{
					int dd = 0;
					dd++;
				}*/

			}

			void RemoveSymbol(TValue symbol)
			{

				typename TSymbolsFreq::iterator it = m_SymbolsFreq.find(symbol);
				assert(it != m_SymbolsFreq.end());
				SymbolInfo& symInfo =  it->second;
				symInfo.m_nFreq--;
				m_nCount--;


				if(symInfo.m_nFreq == 0)
					m_SymbolsFreq.erase(it);


				if(!m_bOnlineCalcSize)
					return;
				
				uint32 nNewCount = symInfo.m_nFreq;
				uint32 nOldCount = nNewCount + 1;
				if(m_SymbolsFreq.size() > 1)
				{

					m_dBitRowSize -= (nOldCount* mathUtils::Log2((double)(m_nCount + 1)/(nOldCount)));
					if(nNewCount > 0)
						m_dBitRowSize += (nNewCount* mathUtils::Log2((double)(m_nCount)/(nNewCount)));

					m_dBitRowSize -= (m_nCount - nNewCount) * mathUtils::Log2((double)(m_nCount + 1)/(m_nCount));

				}
				else
				{
					m_dBitRowSize = 0;

				}

				/*	double dBitRowSize = CalcRowBitSize();
				if(fabs(m_dBitRowSize - dBitRowSize) > 0.00000001)
				{
					int dd = 0;
					dd++;
				}*/
			}
 
			 
			double GetCodeBitSize() const
			{
				
				double dBitRowSize = m_bOnlineCalcSize ? m_dBitRowSize :  CalcRowBitSize();
				
				//dBitRowSize  += (dBitRowSize /m_nError)  + 64; 
				dBitRowSize += 64;
				return dBitRowSize;
			}
		
			double CalcRowBitSize() const
			{
				double dBitRowSize =0;
				if(m_SymbolsFreq.size() > 1)
				{
					for (typename TSymbolsFreq::const_iterator it = m_SymbolsFreq.begin(); it != m_SymbolsFreq.end(); ++it)
					{
						const SymbolInfo& info = it->second;
						double dFreq = info.m_nFreq;
						double dLog2 =  mathUtils::Log2((double)m_nCount/dFreq); 
						dBitRowSize  += (dFreq * dLog2);
					}
				}
				return dBitRowSize;
				
			}
			uint32 GetCompressSize() const
			{
				double dRowBitsLen = GetCodeBitSize();
				uint32 nByteSize = (dRowBitsLen + 7)/8;
				uint32 nLenSize =  GetLenSymbolsFreq();



				return  sizeof(TValue) + nByteSize + 1 + nLenSize; //Type comp (rang or ac) + 4 +

			}

		

			bool compress(const TBPVector<TValue>& vecValues, CommonLib::IWriteStream* pStream)
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
				/*bool bRangeCode = true;
				
				if(m_SymbolsFreq.size() > 1)
				{
					if(!CompressRangeCode(vecValues, pStream, nByteSize))
					{
						pStream->seek(nBeginCompressPos, CommonLib::soFromBegin);
						CompressAcCode(vecValues, pStream);
						bRangeCode = false;
					}
				}*/

				bool bRangeCode = false;
				CompressAcCode(vecValues, pStream);

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
			bool decompress(uint32 nSize, TBPVector<_TValue>& vecValues, CommonLib::IReadStream* pStream)
			{
				
				clear();
				byte nFlag = pStream->readByte();
				bool bRangeCode = nFlag & 0x01;
				m_nTypeFreq = (eTypeFreq)(nFlag>>1);

				TVecFreq vecFreq;
				ReadSymbolsFreq(pStream, vecFreq);

				if(m_bOnlineCalcSize)
					m_dBitRowSize = CalcRowBitSize();

				TValue nBegin;
				pStream->read(nBegin);
				vecValues.push_back(nBegin);

				if(m_SymbolsFreq.size() == 1)
				{
					int64 nDiff = m_SymbolsFreq.begin()->first;
					for (uint32 i = 0; i < m_nCount; ++i)
					{ 
						nBegin += nDiff;
						vecValues.push_back(nBegin);
					}
					return true;
				}


				if(bRangeCode)
					return Decompress<TRangeDecoder>(vecValues, pStream, nBegin, vecFreq);
				else
					return Decompress<TACDecoder>(vecValues, pStream, nBegin, vecFreq);
 
			}
			void clear()
			{

				m_nCount = 0;
				m_nTypeFreq = etfByte;
				m_SymbolsFreq.clear();
				m_nFlags = 0;
				m_dBitRowSize = 0;
			}
		private:

			bool CompressRangeCode(const TBPVector<_TValue>& vecValues, CommonLib::IWriteStream* pStream, 
				uint32 nMaxByteSize)
			{

				TRangeEncoder rgEncoder(pStream, nMaxByteSize);

				TSymbolsFreq checkSymbolsFreq;


				assert((vecValues.size() - 1) == m_nCount);
								

				for (uint32 i = 1, sz = vecValues.size(); i < sz; ++i)
				{
				 
					TValue nDiff = vecValues[i] - vecValues[i - 1];
					typename TSymbolsFreq::iterator it =  m_SymbolsFreq.find(nDiff);
					assert(it != m_SymbolsFreq.end());
					SymbolInfo& info = it->second;
					

					if(!rgEncoder.EncodeSymbol(info.m_nLow, info.m_nHight, m_nCount))
						return false;
				}
				return rgEncoder.EncodeFinish();
			}

			void CompressAcCode(const TBPVector<_TValue>& vecValues, CommonLib::IWriteStream* pStream)
			{
				TACEncoder acEncoder(pStream);

				assert((vecValues.size() - 1) == m_nCount);

				for (uint32 i = 1, sz = vecValues.size(); i < sz; ++i)
				{
					int64 nDiff = vecValues[i] - vecValues[i - 1];
					typename TSymbolsFreq::iterator it =  m_SymbolsFreq.find(nDiff);
					assert(it != m_SymbolsFreq.end());
					SymbolInfo& info = it->second;
					acEncoder.EncodeSymbol(info.m_nLow, info.m_nHight, m_nCount);
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
				for (typename TSymbolsFreq::iterator it = m_SymbolsFreq.begin(); it != m_SymbolsFreq.end(); ++it)
				{
					SymbolInfo& info = it->second;


					info.m_nLow = nPrevF;
					info.m_nHight = info.m_nLow + info.m_nFreq;
					nPrevF = info.m_nHight;

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
		
					info.m_nLow = nPrevF;
					info.m_nHight = info.m_nLow + nFreq;
					info.m_nFreq = nFreq;

					nPrevF = info.m_nHight;
					m_SymbolsFreq.insert(std::make_pair(nDiff, info));

					vecFreq.push_back(Symbol(info, nDiff));
				}
			//	std::sort(vecFreq.begin(), vecFreq.end());
			}

			

			template<class TDecoder>
			bool Decompress(TBPVector<_TValue>& vecValues,  CommonLib::IReadStream* pStream, TValue nBegin, TVecFreq& vecFreq)
			{						

				TDecoder decoder(pStream);
				decoder.StartDecode();
				Symbol sysInfo;
								
				for (uint32 i = 0; i < m_nCount; ++i)
				{ 
					uint32 freq = decoder.GetFreq(m_nCount);

					sysInfo.m_nLow = freq;
					typename TVecFreq::iterator it = std::lower_bound(vecFreq.begin(), vecFreq.end(), sysInfo);
					if(it == vecFreq.end())
						it = vecFreq.end() -1;
					else if(it != vecFreq.begin())
					{
						if(it->m_nLow > freq)
							it--;
					}		
					nBegin += it->m_nSymbol;
					vecValues.push_back(nBegin);
					decoder.DecodeSymbol( it->m_nLow, it->m_nHight, m_nCount);
				}

				return true;
			}
		
		private:	

	

			TSymbolsFreq m_SymbolsFreq;
			uint32 m_nError;
			uint32 m_nCount;
			uint16 m_nFlags;
			eTypeFreq m_nTypeFreq;
			mutable double m_dBitRowSize;
			bool m_bOnlineCalcSize;
	};
}

#endif