#pragma once
#include "../../CompressorParams.h"
#include "../../Utils/alloc/STLAlloc.h"
namespace embDB
{
	template <class _TValue, class _TCompressorParams = CompressorParamsBaseImp>
	class TEmptyValueEncoder
	{
	public:

		typedef _TValue TValue;
		typedef STLAllocator<TValue> TAlloc;
		typedef std::vector<TValue, TAlloc> TValueMemSet;
		typedef _TCompressorParams TCompressorParams;
		TEmptyValueEncoder(uint32 nPageSize, CommonLib::alloc_t* pAlloc,  TCompressorParams *pParams) : m_nCount(0)
		{

		}
		~TEmptyValueEncoder()
		{

		}


		template<typename _Transactions  >
		bool  init(_TCompressorParams *pParams, _Transactions *pTran)
		{

			return true;
		}


		void AddSymbol(uint32 nSize, int nIndex, const TValue& value, const TValueMemSet& vecValues)
		{
			m_nCount++;
		}
		void RemoveSymbol(uint32 nSize, int nIndex, const TValue& value, const TValueMemSet& vecValues)
		{
			m_nCount--;
		}

		void UpdateSymbol(int nIndex, TValue& newValue, const TValue& oldValue, const TValueMemSet& vecValues)
		{
			 
		}
		

		uint32 GetCompressSize() const
		{
			return m_nCount * sizeof(TValue);
		}
		bool BeginEncoding(const TValueMemSet& vecValues)
		{
			return true;
		}
		bool encode(const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream)
		{
			assert(m_nCount == vecValues.size());

			for (uint32 i = 0, sz = vecValues.size(); i < sz; ++i)
			{
				pStream->write(vecValues[i]);
			}
			return true;
		}
		bool decode(uint32 nSize, TValueMemSet& vecValues, CommonLib::IReadStream *pStream)
		{
			TValue value;
			for (uint32 i = 0, sz = nSize; i < sz; ++i)
			{
				pStream->read(value);
				vecValues.push_back(value);
			}

			m_nCount = nSize;
			return true;
		}
		void clear()
		{
			m_nCount = 0;
		}

		uint32 count() const
		{
			return m_nCount;
		}
	protected:
		uint32 m_nCount;

	};
}
