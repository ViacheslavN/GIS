#pragma once
#include "ValueFieldStatistic.h"
#include "NumLenCompressor2.h"
namespace embDB
{
	template<class Type>
	class CValueFieldFullStatistic : public CValueFieldStatisticBase<Type>
	{
	public:

		typedef std::map<Type, uint64> TMapValues;

		CValueFieldFullStatistic(IDBTransaction* pTransaction, uint32 nPageSize = PAGE_SIZE_65K) : CValueFieldStatisticBase<Type>(pTransaction),
			m_nPageSize(nPageSize), m_bValid(false)
		{}

		virtual eStatisticType GetType() const { return eFullStatistic; }
		virtual bool IsValid() const { return m_bValid; }

		virtual bool InitCustom(WriteStreamPage* stream)
		{

			
			FilePagePtr pFieldInfoPage = m_pTransaction->getNewPage(m_nPageSize);
			if (!pFieldInfoPage.get())
				return false; // TO DO Error


			stream.write(pFieldInfoPage->getAddr());


			return true;
		}
		virtual bool LoadCustom(ReadStreamPage* stream)
		{
			return true;
		}



		virtual void AddVarValue(const CommonLib::CVariant& value)
		{

		}
		virtual void RemoveVarValue(const CommonLib::CVariant& value)
		{

		}

		virtual void AddVarValue(const Type& value)
		{

		}
		virtual void RemoveVarValue(const Type& value)
		{

		}


		uint64 GetCount(const Type& value) const
		{

		}
		uint64 GetCount(const CommonLib::CVariant& value) const
		{

		}

	private:
		TMapValues m_mapValues;
		typedef TNumLemCompressor2<uint64, TFindMostSigBit, 64> TNumLen;
		TNumLen m_NumLen;
		uint32 m_nPageSize;
		bool m_bValid;
	};
}