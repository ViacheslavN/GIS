#ifndef  _EMBEDDED_DATABASE_MULTI_KEY_COMPRESS_H_
#define  _EMBEDDED_DATABASE_MULTI_KEY_COMPRESS_H_
#include "CompressorParams.h"
#include "../../../Fields/BaseFieldEncoders.h"
namespace embDB
{
	template <class _TKey, class _TNumLenKeyCompressor>
	class TMultiKeyCompressor
	{
	public:
		typedef _TKey TKey;
		typedef _TNumLenKeyCompressor TNumLenKeyCompressor;
		typedef SignedNumLenEncoder64 TRowIDCompressor;

		typedef _TKey TKey;
		typedef IndexTuple<TKey> TIndex;
		typedef STLAllocator<TIndex> TAlloc;
		typedef std::vector<TIndex, TAlloc> TLeafMemSet;
 
		TMultiKeyCompressor(uint32 nPageSize, CommonLib::alloc_t* pAlloc, CompressorParamsBaseImp *pParams)
			: m_nCount(0), 
			m_NumLenKeyCompressor(nPageSize, pAlloc, pParams)
			,m_RowIDCompressor(nPageSize, pAlloc, pParams)
		{

		}
		~TMultiKeyCompressor()
		{

		}

		template<typename _Transactions  >
		bool  init(CompressorParamsBaseImp *pParams, _Transactions *pTran)
		{

			return true;
		}

		void AddSymbol(uint32 nSize,  int nIndex, const TIndex& nValue, const TLeafMemSet& vecValues)
		{

			if(nSize > 1)
			{

				if(nIndex == 0)
				{
					AddDiffSymbol(vecValues[nIndex + 1], nValue); 
				}
				else
				{
					const TIndex& nPrev =  vecValues[nIndex - 1];
					if(nIndex == nSize - 1)
					{
						AddDiffSymbol(nValue, nPrev); 
					}
					else
					{
						const TIndex nNext =  vecValues[nIndex + 1];
					 

						RemoveDiffSymbol( nNext, nPrev);


						AddDiffSymbol(nValue, nPrev); 
						AddDiffSymbol(nNext, nValue); 
					}
				}
			}
		}
		void RemoveSymbol(uint32 nSize,  int nIndex, const TIndex& nValue, const TLeafMemSet& vecValues)
		{

			if(vecValues.size() > 1)
			{
				if(nIndex == 0)
				{
					RemoveDiffSymbol(vecValues[nIndex + 1], nValue); 
				}
				else
				{

					if(nIndex == vecValues.size() - 1)
					{

						RemoveDiffSymbol(nValue, vecValues[nIndex - 1]); 
					}
					else
					{
						const TIndex& nPrev =  vecValues[nIndex - 1];
						const TIndex& nNext =  vecValues[nIndex + 1];
	
						AddDiffSymbol(nNext, nPrev);

						RemoveDiffSymbol(nValue, nPrev); 
						RemoveDiffSymbol(nNext, nValue); 
					}
				}
			}
		}

		void AddDiffSymbol(const TIndex& value, const TIndex& valuePrev)
		{
			TKey diffKey = value.m_key - valuePrev.m_key;
			int64 diffRowID = value.m_nRowID - valuePrev.m_nRowID;

			m_NumLenKeyCompressor.AddSymbol(diffKey);
			m_RowIDCompressor.AddSymbol(diffRowID);
			m_nCount++;

		}

		void RemoveDiffSymbol(const TIndex& value, const TIndex& valuePrev)
		{
			TKey diffKey = value.m_key - valuePrev.m_key;
			int64 diffRowID = value.m_nRowID - valuePrev.m_nRowID;

			m_NumLenKeyCompressor.RemoveSymbol(diffKey);
			m_RowIDCompressor.RemoveSymbol(diffRowID);
			m_nCount--;
		}

		uint32 GetCompressSize() const
		{
			return m_NumLenKeyCompressor.GetCompressSize() + m_RowIDCompressor.GetCompressSize() 
				+ 2 * sizeof(uint32) + sizeof(TIndex);
		}
		bool BeginEncoding(const TLeafMemSet& vecValues)
		{
			return true;
		}
		bool encode( const TLeafMemSet& vecValues, CommonLib::IWriteStream *pStream)
		{

			if(vecValues.empty())
				return true;

			assert((m_nCount + 1) == vecValues.size());


			uint32 nKeySize = m_NumLenKeyCompressor.GetCompressSize();
			uint32 nROWIDSize = m_RowIDCompressor.GetCompressSize();

			pStream->write(nKeySize);
			pStream->write(nROWIDSize);


			pStream->write(vecValues[0].m_key);
			pStream->write(vecValues[0].m_nRowID);

		

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream RowIDStreams;
			KeyStreams.attach(pStream, pStream->pos(), nKeySize);
			RowIDStreams.attach(pStream,  pStream->pos() + nKeySize, nROWIDSize);


			if (!m_NumLenKeyCompressor.BeginEncoding(&KeyStreams))
				return false;
			if (!m_RowIDCompressor.BeginEncoding(&RowIDStreams))
				return false;

			for (uint32 i = 1, sz = vecValues.size(); i < sz; ++i)
			{

				TKey diffKey = vecValues[i].m_key - vecValues[i -1].m_key;
				int64 diffRowID = vecValues[i].m_nRowID - vecValues[i -1].m_nRowID;
				if (!m_NumLenKeyCompressor.encodeSymbol(diffKey))
					return false;
				if (!m_RowIDCompressor.encodeSymbol(diffRowID))
					return false;
			}


			if (!m_NumLenKeyCompressor.FinishEncoding(&KeyStreams))
				return false;
			if (!m_RowIDCompressor.FinishEncoding(&RowIDStreams))
				return false;
			pStream->seek(pStream->pos() + nKeySize + nROWIDSize, CommonLib::soFromBegin);			
			return true;
		}
		bool decode(uint32 nSize, TLeafMemSet& vecValues, CommonLib::IReadStream *pStream)
		{
			if(nSize == 0)
				return true;

			uint32 nKeySize = pStream->readIntu32();
			uint32 nROWIDSize = pStream->readIntu32();

			TIndex value;

			pStream->read(value.m_key);
			pStream->read(value.m_nRowID);

			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream RowIDStreams;
			KeyStreams.attach(pStream, pStream->pos(), nKeySize);
			RowIDStreams.attach(pStream, pStream->pos() + nKeySize, nROWIDSize);



			m_NumLenKeyCompressor.BeginDecoding(&KeyStreams);
			m_RowIDCompressor.BeginDecoding(&RowIDStreams);
			
			vecValues.push_back(value);
		
			for (uint32 i = 1, sz = nSize; i < sz; ++i)
			{
				TKey nDiffkey;
				int64   nDiffRow;

				m_NumLenKeyCompressor.decodeSymbol(nDiffkey);
			    m_RowIDCompressor.decodeSymbol(nDiffRow);

				value.m_key += nDiffkey;
				value.m_nRowID += nDiffRow;

				vecValues.push_back(value);
			}

			m_nCount = nSize -1;
			return true;
		}
		void clear()
		{
			m_nCount = 0;
			m_NumLenKeyCompressor.clear();
			m_RowIDCompressor.clear();
		}

		uint32 count() const
		{
			return m_nCount;
		}
	protected:
		uint32 m_nCount;
		TNumLenKeyCompressor m_NumLenKeyCompressor;
		TRowIDCompressor	m_RowIDCompressor;

	};
}

#endif