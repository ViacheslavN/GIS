#ifndef _EMBEDDED_DATABASE_BASE_VALUE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BASE_VALUE_COMPRESSOR_H_
#include <vector>
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "CompressorParams.h"
namespace embDB
{


	template<class _TValue, class _TCompressor>
	class TBaseValueCompress
	{
	public:

		typedef _TValue TValue;
		typedef _TCompressor TCompressor;
		typedef embDB::TBPVector<TValue> TValueMemSet;


		TBaseValueCompress(CommonLib::alloc_t *pAlloc, uint32 nPageSize,CompressorParamsBaseImp *pParams) 
		{

		}
		~TBaseValueCompress()
		{

		}

		void AddSymbol(uint32 nSize,  int nIndex, TValue nValue, const TValueMemSet& vecValues)
		{
			m_compressor.AddSymbol(vecValues[nIndex]); 
		}
		void RemoveSymbol(uint32 nSize,  int nIndex, TValue nValue, const TValueMemSet& vecValues)
		{
			m_compressor.RemoveSymbol(vecValues[nIndex]); 
		}
		uint32 GetCompressSize() const
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