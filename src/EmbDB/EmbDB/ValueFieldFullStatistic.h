#pragma once
#include "ValueFieldStatistic.h"
#include "NumLenCompressor2.h"
#include "WriteStreamPage.h"
#include "ReadStreamPage.h"
#include "SignCompressor2.h"
namespace embDB
{
	template<class _Type>
	class TValueFieldFullStatistic : public TValueFieldStatisticBase<_Type>
	{
	public:
		typedef _Type Type;
		static const uint32 nMaxStatisticsCount = 1000000;

		typedef std::map<Type, uint64> TMapValues;

		TValueFieldFullStatistic(CommonLib::alloc_t *pAlloc, IDBTransaction* pTransaction, uint32 nPageSize = PAGE_SIZE_65K, bool bCheckCRC = false) :
			TValueFieldStatisticBase<_Type>(pAlloc, pTransaction, bCheckCRC),
			m_nPageSize(nPageSize), m_bValid(true), m_nStreamPage(-1)
		{
			
			
			
		}

		virtual eStatisticType GetType() const { return eFullStatistic; }
		virtual bool IsValid() const { return m_bValid; }

		virtual bool InitCustom(WriteStreamPage* stream)
		{

			
			FilePagePtr pFieldInfoPage = m_pTransaction->getNewPage(m_nPageSize);
			if (!pFieldInfoPage.get())
				return false; // TO DO Error


			stream.write(pFieldInfoPage->getAddr());

			m_nStreamPage = pFieldInfoPage->getAddr();
			Init();

			return true;
		}
		virtual bool LoadCustom(ReadStreamPage* stream)
		{
			m_nStreamPage = stream.readInt64();

			return true;
		}

		virtual bool  save()
		{
			return saveImp(false);
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
				m_mapValues.insert(std:::make_pair(value.Get<Type>(), 1));
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
					m_mapValues.remove(it);
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
				m_mapValues.insert(std:::make_pair(value, 1));
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
					m_mapValues.remove(it);
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
				return;
			}
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
			m_NumLenValue.clear();

			Type nPrev = it->first;
			m_NumLenValue.PreAddSympol(it->second);
			++it;

			for (; it != end; ++it)
			{
				m_NumLenKey.PreAddSympol(it->first - nPrev);
				m_NumLenValue.PreAddSympol(it->second);

			}


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

		void ReadRowData(CommonLib::IReadStream *pStream)
		{
			Type key;
			uint64 nCount;
			for (uint32 i = 0; i < sz; ++i)
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
		TNumLen m_NumLenValue;
		TSignCompressor2 m_SignCompressor;
		uint32 m_nPageSize;
		bool m_bValid;
		int64 m_nStreamPage;
		CommonLib::alloc_t *m_pAlloc;
	};

	typedef TValueFieldFullStatistic<char>	 TCharFullStatistic;
	typedef TValueFieldFullStatistic<byte>	 TByteFullStatistic;
	typedef TValueFieldFullStatistic<uint16> TUint16FullStatistic;
	typedef TValueFieldFullStatistic<int16>  TInt16FullStatistic;
}