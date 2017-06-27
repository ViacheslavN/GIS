#pragma once

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "../../CompressorParams.h"
#include "../../Utils/alloc/STLAlloc.h"
 
namespace embDB
{



	template<class _TValue, class _TSignValue,  class _TEncoder, class _TCompressorParams = CompressorParamsBaseImp>
	class TBaseValueDiffEncoder
	{
	public:

		typedef _TValue TValue;
		typedef STLAllocator<TValue> TAlloc;
		typedef std::vector<TValue, TAlloc> TValueMemSet;
		typedef _TCompressorParams TCompressorParams;
		typedef _TEncoder TEncoder;
		typedef /*typename TDefSign<TValue>::TSignType*/_TSignValue TSignValue;


		TBaseValueDiffEncoder(uint32 nPageSize, CommonLib::alloc_t *pAlloc, CompressorParamsBaseImp *pParams) :
			m_encoder(nPageSize, pAlloc, pParams)
		{}

		~TBaseValueDiffEncoder()
		{}


		template<typename _Transactions  >
		bool  init(TCompressorParams *pParams, _Transactions *pTran)
		{

			return true;
		}


		virtual void Write(const TValue& value, CommonLib::IWriteStream *pStream) = 0;
		virtual void Read(TValue& value, CommonLib::IReadStream *pStream) = 0;
		virtual uint32 GetValueSize() const = 0;
		

		void AddSymbol(uint32 nSize, int nIndex, const TValue& nValue, const TValueMemSet& vecValues)
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
		void RemoveSymbol(uint32 nSize, int nIndex, const TValue& nValue, const TValueMemSet& vecValues)
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

		void UpdateSymbol(uint32 nIndex, TValue& newValue, const TValue& OldValue, const TValueMemSet& vecValues)
		{
			RemoveSymbol(vecValues.size(), nIndex, OldValue, vecValues);
			AddSymbol(vecValues.size(), nIndex, newValue, vecValues);
		}

		uint32 GetCompressSize() const
		{
			return m_encoder.GetCompressSize() + GetValueSize();
		}

		bool BeginEncoding(const TValueMemSet& vecValues)
		{
			return true;
		}

		bool encode(const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream)
		{
			assert(m_encoder.count() == vecValues.size() - 1);
			//pStream->write(vecValues[0]);
			Write(vecValues[0], pStream);
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
			//pStream->read(val);
			Read(val, pStream);
			m_encoder.BeginDecoding(pStream);

			assert(m_encoder.count() == nCount - 1);
			vecValues.push_back(val);

			TSignValue sym;
			for (size_t i = 1; i < nCount; ++i)
			{
				m_encoder.decodeSymbol(sym);
				vecValues.push_back(sym + (TSignValue)vecValues[i - 1]);
			}

			m_encoder.FinishDecoding();
		}
		void clear()
		{
			m_encoder.clear();
		}
	protected:
		TEncoder m_encoder;
	};


	template<class _TValue, class _TSignValue, class _TEncoder, class _TCompressorParams = CompressorParamsBaseImp>
	class TValueDiffEncoder : public TBaseValueDiffEncoder<_TValue, _TSignValue, _TEncoder, _TCompressorParams>
	{
	public:
		typedef TBaseValueDiffEncoder<_TValue, _TSignValue, _TEncoder, _TCompressorParams> TBase;

		TValueDiffEncoder(uint32 nPageSize, CommonLib::alloc_t *pAlloc, CompressorParamsBaseImp *pParams) : TBase(nPageSize, pAlloc, pParams)
		{}

		virtual void Write(const TValue& value, CommonLib::IWriteStream *pStream)
		{
			pStream->write(value);
		}

		virtual void Read(TValue& value, CommonLib::IReadStream *pStream)
		{
			pStream->read(value);
		}
		virtual uint32 GetValueSize() const
		{
			return sizeof(TValue);
		}
	};
}
