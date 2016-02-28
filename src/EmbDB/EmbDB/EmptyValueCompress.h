#ifndef _EMBEDDED_DATABASE_EMPTY_VALUE_DIFF_COMPRESS_H_
#define _EMBEDDED_DATABASE_EMPTY_VALUE_DIFF_COMPRESS_H_

namespace embDB
{
	template <class _TValue>
	class TEmptyValueCompress
	{
	public:

		typedef _TValue TValue;
		typedef  TBPVector<TValue> TValueMemSet;
		TEmptyValueCompress() : m_nCount(0)
		{

		}
		~TEmptyValueCompress()
		{

		}

		void AddSymbol(uint32 nSize,  int nIndex, const TValue& value, const TValueMemSet& vecValues)
		{
			m_nCount++;
		}
		void RemoveSymbol(uint32 nSize,  int nIndex, const TValue& value, const TValueMemSet& vecValues)
		{
			m_nCount--;
		}
		 


		uint32 GetComressSize() const
		{
			return m_nCount * sizeof(TValue);
		}

		bool compress( const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream)
		{
			assert(m_nCount == vecValues.size());

			for (uint32 i = 0, sz = vecValues.size(); i < sz; ++i)
			{
				pStream->write(vecValues[i]);
			}
			return true;
		}
		bool decompress(uint32 nSize, TValueMemSet& vecValues, CommonLib::IReadStream *pStream)
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
	private:
		uint32 m_nCount;

	};
}


#endif