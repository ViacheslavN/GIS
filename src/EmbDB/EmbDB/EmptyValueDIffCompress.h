#ifndef _EMBEDDED_DATABASE_EMPTY_VALUE_DIFF_COMPRESS_H_
#define _EMBEDDED_DATABASE_EMPTY_VALUE_DIFF_COMPRESS_H_
#include "CompressorParams.h"
namespace embDB
{
	template <class _TValue>
	class TEmptyValueCompress
	{
	public:

		template _TValue TValue;
		TEmptyValueCompress(CommonLib::alloc_t* pAlloc, CompressorParamsBaseImp *pParams) : m_nCount(0)
		{
			
		}
		~TEmptyValueCompress()
		{

		}

		void AddSymbol(uint32 nSize,  int nIndex, const TValue& value, const embDB::TBPVector<TValue>& vecValues)
		{
			m_nCount++;
		}
		void RemoveSymbol(uint32 nSize,  int nIndex, const TValue& value, const embDB::TBPVector<TValue>& vecValues)
		{
			m_nCount++;
		}
		void RemoveDiffSymbol(const TValue& value)
		{
			m_nCount--;
		}
		void AddDiffSymbol(const TValue& value)
		{
			m_nCount++;
		}

		uint32 GetComressSize() const
		{
			return (m_nCount + 1) * sizeof(TValue);
		}

		bool compress( const embDB::TBPVector<TValue>& vecValues, CommonLib::IWriteStream *pStream)
		{
			for (uint32 i = 0, sz = vecValues.size(); i < sz; ++i)
			{
				pStream->write(vecValues[i])
			}
		}
		bool decompress(uint32 nSize, embDB::TBPVector<TValue>& vecValues, CommonLib::IReadStream *pStream)
		{
			TValue value;
			for (uint32 i = 0, sz = vecValues.size(); i < sz; ++i)
			{
				pStream->read(value);
				vecValues.push_back(value);
			}
			m_nCount = nSize - 1;
		}
		void clear()
		{
			m_nCount = 0;
		}
	private:
		uint32 m_nCount;
		
	};
}


#endif