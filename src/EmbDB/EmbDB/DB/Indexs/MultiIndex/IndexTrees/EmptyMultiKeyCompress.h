#ifndef  _EMBEDDED_DATABASE_EMPTY_MULTI_KEY_COMPRESS_H_
#define  _EMBEDDED_DATABASE_EMPTY_MULTI_KEY_COMPRESS_H_
#include "CompressorParams.h"
namespace embDB
{
	template <class _TKey>
	class TEmptyMultiKeyCompress
	{
	public:

		typedef _TKey TKey;
		typedef IndexTuple<TKey> TIndex;
		typedef STLAllocator<TIndex> TKeyAlloc;
		typedef std::vector<TIndex, TKeyAlloc> TLeafMemSet;

		TEmptyMultiKeyCompress(uint32 nPageSize, CommonLib::alloc_t* pAlloc,  CompressorParamsBaseImp *pParams) : m_nCount(0)
		{

		}
		~TEmptyMultiKeyCompress()
		{

		}

		template<typename _Transactions  >
		bool  init(CompressorParamsBaseImp *pParams, _Transactions *pTran)
		{

			return true;
		}


		void AddSymbol(uint32 nSize,  int nIndex, const TIndex& value, const TLeafMemSet& vecValues)
		{
			m_nCount++;
		}
		void RemoveSymbol(uint32 nSize,  int nIndex, const TIndex& value, const TLeafMemSet& vecValues)
		{
			m_nCount--;
		}

		void AddDiffSymbol(const TIndex& value, const TIndex& valuePrev)
		{
			m_nCount++;
		}

		uint32 GetCompressSize() const
		{
			return (m_nCount + 1) * sizeof(TIndex);
		}
		bool BeginEncoding(const TLeafMemSet& vecValues)
		{
			return true;
		}
		bool encode( const TLeafMemSet& vecValues, CommonLib::IWriteStream *pStream)
		{
			assert(m_nCount == vecValues.size());

			for (uint32 i = 0, sz = vecValues.size(); i < sz; ++i)
			{
				pStream->write(vecValues[i].m_key);
				pStream->write(vecValues[i].m_nRowID);
			}
			return true;
		}
		bool decode(uint32 nSize, TLeafMemSet& vecValues, CommonLib::IReadStream *pStream)
		{
			TIndex value;
			for (uint32 i = 0, sz = nSize; i < sz; ++i)
			{
				pStream->read(value.m_key);
				pStream->read(value.m_nRowID);
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

#endif