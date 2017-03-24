#pragma once


#include "WriteStreamPage.h"
#include "ReadStreamPage.h"
#include "ValueFieldStatistic.h"
#include "SignCompressor2.h"
#include "NumLenCompressor2.h"
#include "CommonLibrary/TimeUtils.h"
#include "CommonLibrary/FixedBitStream.h"

namespace embDB
{
	template<class _Type>
	class TValueFieldFullStatistic : public TValueFieldStatisticBase<_Type>
	{
	public:
		typedef _Type Type;
		static const uint32 nMaxStatisticsCount = 1000000;

		typedef std::map<Type, uint64> TMapValues;

		TValueFieldFullStatistic(CommonLib::alloc_t *pAlloc, IDBTransaction* pTransaction, const SStatisticInfo& si, bool bCheckCRC = false, uint32 nPageSize = PAGE_SIZE_65K) :
			TValueFieldStatisticBase<_Type>(pAlloc, pTransaction, si, bCheckCRC),
			m_nPageSize(nPageSize), m_bValid(true), m_nStreamPage(-1)
		{
			
			
			
		}

		virtual eStatisticType GetType() const { return stFullStatistic; }

		virtual bool IsValid() const { return m_bValid; }

		virtual bool InitCustom(WriteStreamPage* stream)
		{

			
			FilePagePtr pFieldInfoPage = m_pTransaction->getNewPage(m_nPageSize);
			if (!pFieldInfoPage.get())
				return false; // TO DO Error


			stream->write(pFieldInfoPage->getAddr());
			m_nStreamPage = pFieldInfoPage->getAddr();
			saveImp(true);

			return true;
		}
		virtual bool LoadCustom(ReadStreamPage* stream)
		{
			m_nStreamPage = stream->readInt64();
			ReadStatistic();
			return true;
		}


		void SetTransaction(IDBTransaction* pTransaction) // TO DO fix
		{
			m_pTransaction = pTransaction;
		}

		virtual bool  save()
		{
		 
			 m_nDateUpdate = CommonLib::TimeUtils::GetCurrentDate(&m_nTimeUpdate);
			 saveImp(false);
			 return true;
		}

		virtual bool clear()
		{
			m_mapValues.clear();
			return true;
		}

		virtual void AddVarValue(const CommonLib::CVariant& value)
		{
			if (!m_bValid)
				return;

			auto it = m_mapValues.find(value.Get<Type>());
			if (it != m_mapValues.end())
				it->second += 1;
			else
			{
				m_mapValues.insert(std::make_pair(value.Get<Type>(), 1));
			}

			if (m_mapValues.size() > nMaxStatisticsCount)
			{
				m_mapValues.clear();
				m_bValid = false;
			}
		}
		virtual void RemoveVarValue(const CommonLib::CVariant& value)
		{
			if (!m_bValid)
				return;

			auto it = m_mapValues.find(value.Get<Type>());
			if (it != m_mapValues.end())
			{
				it->second -= 1;
				if (it->second == 0)
					m_mapValues.erase(it);
			}
		}

		virtual void AddVarValue(const Type& value)
		{
			if (!m_bValid)
				return;

			auto it = m_mapValues.find(value);
			if (it != m_mapValues.end())
				it->second += 1;
			else
			{
				m_mapValues.insert(std::make_pair(value, 1));
			}

			if (m_mapValues.size() > nMaxStatisticsCount)
			{
				m_mapValues.clear();
				m_bValid = false;
			}
		}
		virtual void RemoveVarValue(const Type& value)
		{
			if (!m_bValid)
				return;

			auto it = m_mapValues.find(value);
			if (it != m_mapValues.end())
			{
				it->second -= 1;
				if (it->second == 0)
					m_mapValues.erase(it);
			}
			 
		}

		uint64 GetCount(const Type& value) const
		{
			if (!m_bValid)
				return 0;

			auto it = m_mapValues.find(value);
			if (it != m_mapValues.end())
				return it->second;
			return 0;
		}
		uint64 GetCount(const CommonLib::CVariant& value) const
		{
			if (!m_bValid)
				return 0;

			auto it = m_mapValues.find(value.Get<Type>());
			if (it != m_mapValues.end())
				return it->second;
			return 0;
		}
		uint64 GetRangeCount(const CommonLib::CVariant& left, const CommonLib::CVariant& right) const
		{
			auto it_lb = m_mapValues.lower_bound(left.Get<Type>());
			auto it_ub = m_mapValues.upper_bound(right.Get<Type>());

			uint64 nCount = 0;
			for (; it_lb != it_ub; ++it_lb)
			{
				nCount += it_lb->second;
			}

			return nCount;
		}
	

		void saveImp(bool bInit)
		{
			WriteStreamPage stream(m_pTransaction.get(), m_nPageSize, m_bCheckCRC, FIELD_INFO_STATISTIC, FIELD_FULL_STATISTIC);
			stream.open(m_nStreamPage, 0, !bInit);
			stream.write(m_bValid);
			stream.write(m_nDateUpdate);
			stream.write(m_nTimeUpdate);
			 
			SaveStatistic(&stream);

			 

			stream.Save();
		}

		bool ReadStatistic()
		{
			ReadStreamPage stream(m_pTransaction.get(), m_nPageSize, m_bCheckCRC, FIELD_INFO_STATISTIC, FIELD_FULL_STATISTIC);
			stream.open(m_nStreamPage, 0);
 
			m_bValid = stream.readBool();
			m_nDateUpdate = stream.readIntu32();
			m_nTimeUpdate = stream.readIntu32();
			uint32 nCount = stream.readIntu32();
			uint32 nRowSize = GetRowSize(nCount);
			if (nRowSize < (m_nPageSize - GetHeaderSize()))
			{
				ReadRowData(nCount, &stream);
				return true;
			}

			m_NumLenKey.clear();
			m_NumLenCount.clear();

			CommonLib::CReadMemoryStream keyStream(m_pAlloc);
			CommonLib::CReadMemoryStream countStream(m_pAlloc);
			CommonLib::FxBitReadStream keyBits;
			CommonLib::FxBitReadStream countsBits;


			uint32 nKeySize = stream.readIntu32();
			uint32 nCountSize = stream.readIntu32();


			keyStream.resize(nKeySize);
			countStream.resize(nCountSize);

			stream.read(keyStream.buffer(), nKeySize);
			stream.read(countStream.buffer(), nCountSize);


			m_NumLenKey.Init(&keyStream);
			m_NumLenCount.Init(&countStream);

			uint32 nBitKeySize = (m_NumLenKey.GetBitsLen() + 7)/8;
			uint32 nBitCountSize = (m_NumLenCount.GetBitsLen() + 7) / 8;

			keyBits.attach(&keyStream, keyStream.pos(), nBitKeySize, true);
			countsBits.attach(&countStream, countStream.pos(), nBitKeySize, true);




			Type key = 0;
			Type TempKey;
			uint64 nNum;
			keyStream.read(key);

			m_NumLenKey.StartDecode();
			m_NumLenCount.StartDecode();

		 
			m_NumLenCount.DecodeSymbol(nNum);
			uint32 nBitPart = nNum;

			if (nBitPart > 1)
			{
				nNum = 0;
				countsBits.readBits(nNum, nBitPart - 1);
				nNum |= (1 << (nBitPart - 1));
			}

			m_mapValues.insert(std::make_pair(key, nNum));

			for (uint32 i = 1; i < nCount; ++i )
			{
			
				m_NumLenKey.DecodeSymbol(TempKey);
				nBitPart = TempKey;
				if (nBitPart > 1)
				{
					TempKey = 0;
					keyBits.readBits(TempKey, nBitPart - 1);
					TempKey |= (1 << (nBitPart - 1));
				}
				
				key += TempKey;

				m_NumLenCount.DecodeSymbol(nNum);
				nBitPart = nNum;
				if (nBitPart > 1)
				{
					nNum = 0;
					countsBits.readBits(nNum, nBitPart - 1);
					nNum |= (1 << (nBitPart - 1));
				}

				m_mapValues.insert(std::make_pair(key, nNum));
			}
			
			return true;
		}


		void SaveStatistic(CommonLib::IWriteStream *pStream)
		{
			pStream->write((uint32)m_mapValues.size());
			if (m_mapValues.size() == 0)
				return;

 			uint32 nRowSize = GetRowSize(m_mapValues.size());
			if (nRowSize < (m_nPageSize - GetHeaderSize()))
			{
				WriteRowData(pStream);
				return;
			}

			auto it = m_mapValues.begin();
			auto end = m_mapValues.end();

			m_NumLenKey.clear();
			m_NumLenCount.clear();

			Type nPrev = it->first;
			m_NumLenCount.PreAddSympol(it->second);
			++it;

			for (; it != end; ++it)
			{
				m_NumLenKey.PreAddSympol(it->first - nPrev);
				m_NumLenCount.PreAddSympol(it->second);
				nPrev = it->first;
			}

			uint32 nKeySize = m_NumLenKey.GetCompressSize();
			uint32 nBitKeySize =(m_NumLenKey.GetBitsLen() + 7) / 8;

			uint32 nCountSize = m_NumLenCount.GetCompressSize();
			uint32 nBitCountSize = (m_NumLenCount.GetBitsLen() + 7) / 8;

			CommonLib::CWriteMemoryStream keyStream(m_pAlloc);
			CommonLib::CWriteMemoryStream countStream(m_pAlloc);
			CommonLib::FxBitWriteStream keyBits;
			CommonLib::FxBitWriteStream countsBits;

			keyStream.resize(nKeySize * 2);
			countStream.resize(nCountSize * 2);


			m_NumLenKey.BeginEncode(&keyStream);
			m_NumLenKey.WriteHeader(&keyStream);

			m_NumLenCount.BeginEncode(&countStream);
			m_NumLenCount.WriteHeader(&countStream);

			keyBits.attach(&keyStream, keyStream.pos(), nBitKeySize, true);
			countsBits.attach(&countStream, countStream.pos(), nBitCountSize, true);

			it = m_mapValues.begin();

			keyStream.write(it->first);
			m_NumLenCount.EncodeSymbol(it->second, &countsBits);
			nPrev = it->first;
			++it;			
			for (; it != end; ++it)
			{
				m_NumLenKey.EncodeSymbol(it->first - nPrev, &keyBits);
				m_NumLenCount.EncodeSymbol(it->second, &countsBits);
				nPrev = it->first;

			}

			m_NumLenKey.EncodeFinish();
			m_NumLenCount.EncodeFinish();

			pStream->write(keyStream.pos());
			pStream->write(countStream.pos());

			pStream->write(keyStream.buffer(), keyStream.pos());		
			pStream->write(countStream.buffer(), countStream.pos());

		}

		const uint32 GetRowSize(uint32 nCount) const
		{
			return nCount * (sizeof(Type) + sizeof(uint64));
		}
		const uint32 GetHeaderSize()
		{
			return 256; //TO DO fix
		}

		void WriteRowData(CommonLib::IWriteStream *pStream)
		{
			auto it = m_mapValues.begin();
			auto end = m_mapValues.end();

			for (; it != end; ++it)
			{
				pStream->write(it->first);
				pStream->write(it->second);
			}
		}

		void ReadRowData(uint32 nNum, CommonLib::IReadStream *pStream)
		{
			Type key;
			uint64 nCount;
			for (uint32 i = 0; i < nNum; ++i)
			{
				pStream->read(key);
				pStream->read(nCount);

				m_mapValues.insert(std::make_pair(key, nCount));
			}

		}

	private:
		TMapValues m_mapValues;
		typedef TNumLemCompressor2<uint64, TFindMostSigBit, 64> TNumLen;
		TNumLen m_NumLenKey;
		TNumLen m_NumLenCount;
		TSignCompressor2 m_SignCompressor;
		uint32 m_nPageSize;
		bool m_bValid;
		int64 m_nStreamPage;
	};

	typedef TValueFieldFullStatistic<char>	 TCharFullStatistic;
	typedef TValueFieldFullStatistic<byte>	 TByteFullStatistic;
	typedef TValueFieldFullStatistic<uint16> TUint16FullStatistic;
	typedef TValueFieldFullStatistic<int16>  TInt16FullStatistic;
	typedef TValueFieldFullStatistic<uint32> TUint32FullStatistic;
	typedef TValueFieldFullStatistic<int32>  TInt32FullStatistic;
	typedef TValueFieldFullStatistic<uint64> TUint64FullStatistic;
	typedef TValueFieldFullStatistic<int64>  TInt64FullStatistic;
}