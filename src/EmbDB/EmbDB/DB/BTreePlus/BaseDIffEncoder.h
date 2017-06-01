#pragma once

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "../../CompressorParams.h"
#include "../../Utils/alloc/STLAlloc.h"
 
namespace embDB
{



	template<class _TValue,   class _TEncoder, class _TCompressorParams = CompressorParamsBaseImp>
	class TBaseValueDiffEncoder
	{
	public:

		typedef _TValue TValue;
		typedef STLAllocator<TValue> TAlloc;
		typedef std::vector<TValue, TAlloc> TValueMemSet;
		typedef _TCompressorParams TCompressorParams;
		typedef _TEncoder TEncoder;
		typedef typename TDefSign<TValue>::TSignType TSignValue;


		TBaseValueDiffEncoder(uint32 nPageSize, CommonLib::alloc_t *pAlloc, CompressorParamsBaseImp *pParams) :
			m_encoder(nPageSize, pAlloc, pParams)
		{}

		~TBaseValueDiffEncoder()
		{}

		void AddSymbol(uint32 nSize, int nIndex, TValue nValue, const TValueMemSet& vecValues)
		{
			if (nSize > 1)
			{

				if (nIndex == 0)
				{
					AddDiffSymbol(vecValues[nIndex + 1] - nValue);
				}
				else
				{
					TValue nPrev = vecValues[nIndex - 1];
					if (nIndex == nSize - 1)
					{
						AddDiffSymbol(nValue - nPrev);
					}
					else
					{
						TValue nNext = vecValues[nIndex + 1];
						TValue nOldSymbol = nNext - nPrev;

						RemoveDiffSymbol(nOldSymbol);


						AddDiffSymbol(nValue - nPrev);
						AddDiffSymbol(nNext - nValue);
					}
				}
			}
		}
		void AddDiffSymbol(TSignValue nValue)
		{
			m_encoder.AddSymbol(nValue);
		}
		void RemoveSymbol(uint32 nSize, int nIndex, TValue nValue, const TValueMemSet& vecValues)
		{
			if (vecValues.size() > 1)
			{
				if (nIndex == 0)
				{
					RemoveDiffSymbol(vecValues[nIndex + 1] - nValue);
				}
				else
				{

					if (nIndex == vecValues.size() - 1)
					{

						RemoveDiffSymbol(nValue - vecValues[nIndex - 1]);
					}
					else
					{
						TValue nPrev = vecValues[nIndex - 1];
						TValue nNext = vecValues[nIndex + 1];
						TValue nNewSymbol = nNext - nPrev;

						AddDiffSymbol(nNewSymbol);

						RemoveDiffSymbol(nValue - nPrev);
						RemoveDiffSymbol(nNext - nValue);
					}
				}
			}
		}
		void RemoveDiffSymbol(TSignValue nValue)
		{
			m_encoder.RemoveSymbol(nValue);
		}

		uint32 GetCompressSize() const
		{
			return m_encoder.GetCompressSize() + sizeof(TValue);
		}

		bool encode(const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream)
		{
			assert(m_encoder.count() == vecValues.size() - 1);
			pStream->write(vecValues[0]);
			if (!m_encoder.BeginEncoding(pStream))
				return false;

			for (size_t i = 1; i < vecValues.size(); ++i)
			{
				if (!m_encoder.encodeSymbol(TSignValue(vecValues[i] - vecValues[i - 1])))
					return false;
			}

			return m_encoder.FinishEncoding(pStream);
		}
		void decode(uint32 nCount, TValueMemSet& vecValues, CommonLib::IReadStream *pStream)
		{
			TValue val;
			pStream->read(val);
			m_encoder.BeginDecoding(pStream);

			assert(m_encoder.count() == nCount - 1);
			vecValues.push_back(val);

			for (size_t i = 1; i < nCount; ++i)
			{
				vecValues.push_back(m_encoder.decodeSymbol() + vecValues[i - 1]);
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
