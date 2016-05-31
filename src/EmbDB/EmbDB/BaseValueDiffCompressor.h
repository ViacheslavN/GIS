#ifndef _EMBEDDED_DATABASE_BASE_VALUE_DIFF_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BASE_VALUE_DIFF_COMPRESSOR_H_
#include <vector>
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "CompressorParams.h"
namespace embDB
{


	template<class _TValue, class _TSignTValue, class _TCompressor>
	class TBaseValueDiffCompress
	{
	public:

		typedef _TValue TValue;
		typedef _TSignTValue TSignTValue;
		typedef _TCompressor TCompressor;
		typedef embDB::TBPVector<TValue> TValueMemSet;
	 

		TBaseValueDiffCompress(CommonLib::alloc_t *pAlloc, uint32 nPageSize,CompressorParamsBaseImp *pParams) :
			m_compressor(pParams->m_compressType, pParams->m_nErrorCalc, pParams->m_bCalcOnlineSize)
		{

		}
		~TBaseValueDiffCompress()
		{

		}

		void AddSymbol(uint32 nSize,  int nIndex, TValue nValue, const TValueMemSet& vecValues)
		{
			if(nSize > 1)
			{

				if(nIndex == 0)
				{
					AddDiffSymbol(vecValues[nIndex + 1] - nValue); 
				}
				else
				{
					TValue nPrev =  vecValues[nIndex - 1];
					if(nIndex == nSize - 1)
					{
						AddDiffSymbol(nValue - nPrev); 
					}
					else
					{
						TValue nNext =  vecValues[nIndex + 1];
						TValue nOldSymbol = nNext - nPrev;

						RemoveDiffSymbol(nOldSymbol);


						AddDiffSymbol(nValue - nPrev); 
						AddDiffSymbol(nNext - nValue); 
					}
				}
			}
		}
		void AddDiffSymbol(TSignTValue nValue)
		{
			m_compressor.AddSymbol(nValue); 
		}
		void RemoveSymbol(uint32 nSize,  int nIndex, TValue nValue, const TValueMemSet& vecValues)
		{
			if(vecValues.size() > 1)
			{
				if(nIndex == 0)
				{
					RemoveDiffSymbol(vecValues[nIndex + 1] - nValue); 
				}
				else
				{

					if(nIndex == vecValues.size() - 1)
					{

						RemoveDiffSymbol(nValue - vecValues[nIndex - 1]); 
					}
					else
					{
						TValue nPrev =  vecValues[nIndex - 1];

						TValue nNext =  vecValues[nIndex + 1];
						TValue nNewSymbol = nNext - nPrev;

						AddDiffSymbol(nNewSymbol);


						RemoveDiffSymbol(nValue - nPrev); 
						RemoveDiffSymbol(nNext - nValue); 
					}
				}
			}
		}
		void RemoveDiffSymbol(TSignTValue nValue)
		{
			m_compressor.RemoveSymbol(nValue); 
		}

		uint32 GetComressSize() const
		{
			return m_compressor.GetCompressSize();
		}



		void compress( const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream)
		{
			m_compressor.compress(vecValues, pStream);
		}
		void decompress(uint32 nSize, TValueMemSet& vecValues, CommonLib::IReadStream *pStream)
		{
			m_compressor.decompress(nSize, vecValues, pStream);
		}
		void clear()
		{
			m_compressor.clear();
		}
	private:
		TCompressor m_compressor;
	};
}


#endif