#pragma once

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "../CompressorParams.h"
#include "../Utils/alloc/STLAlloc.h"

namespace embDB
{


	template<class _TValue, class _TEncoder, class _TCompressorParams = CompressorParamsBaseImp>
	class TBaseValueEncoder
	{
	public:

		typedef _TValue TValue;
		typedef STLAllocator<TKey> TAlloc;
		typedef std::vector<TValue, TAlloc> TValueMemSet;
		typedef _TCompressorParams TCompressorParams;
		typedef _TEncoder TEncoder;


		TBaseValueEncoder(CommonLib::alloc_t *pAlloc, uint32 nPageSize, CompressorParamsBaseImp *pParams) :
			m_compressor(pParams)
		{}

		~TBaseValueEncoder()
		{}

		void AddSymbol(uint32 nSize, int nIndex, TValue nValue, const TValueMemSet& vecValues)
		{
			m_encoder.AddSymbol(vecValues[nIndex]);
		}
		 
		void RemoveSymbol(uint32 nSize, int nIndex, TValue nValue, const TValueMemSet& vecValues)
		{
			m_encoder.RemoveSymbol(vecValues[nIndex]);
		}
 
		uint32 GetCompressSize() const
		{
			return m_encoder.GetCompressSize();
		}

		void encode(const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream)
		{
			assert(m_encoder.count() == vecValues.size())
			m_encoder.BeginEncoding(pStream);

			for (size_t i = 0; i < vecValues.size(); ++i)
			{
				m_encoder.encodeSymbol(TSignValue(vecValues[i]), pStream);
			}

			m_encoder.FinishEncoding(pStream);
		}
		void decode(uint32 nCount, TValueMemSet& vecValues, CommonLib::IReadStream *pStream)
		{
			m_encoder.BeginDecoding(pStream);
			assert(m_encoder.count() == nCount);	
			for (size_t i = 0; i < nCount; ++i)
			{
				vecValues.push_back(m_encoder.decodeSymbol(pStream));
			}

			m_encoder.FinishDecoding();
		}
		void clear()
		{
			m_encoder.clear();
		}
	private:
		TEncoder m_encoder;
	};
}

