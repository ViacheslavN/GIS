#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_SPATIAL_POINT_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_64_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_SPATIAL_POINT_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_64_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "SpatialPointQuery.h"
#include "CompressorParams.h"
namespace embDB
{

	template<class _TValue>
	class BPSpatialPointLeafNodeMapSimpleCompressor64 
	{
	public:
		typedef _TValue TValue;
		typedef  TBPVector<ZOrderPoint2DU64> TLeafKeyMemSet;
		typedef  TBPVector<TValue> TLeafValueMemSet;
		typedef CompressorParamsBaseImp TLeafCompressorParamsBase;
		BPSpatialPointLeafNodeMapSimpleCompressor64(CommonLib::alloc_t *pAlloc = 0, TLeafCompressorParamsBase *pParms = NULL) : m_nSize(0)
		{}


		template<typename _Transactions  >
		static TLeafCompressorParamsBase *LoadCompressorParams(int64 nPage, _Transactions *pTran)
		{
			return NULL;
		}
		virtual bool Load(TLeafKeyMemSet& vecKeys, TLeafValueMemSet& vecValues, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			vecKeys.reserve(m_nSize);
			vecValues.reserve(m_nSize);


			uint32 nKeySize =  m_nSize * 2 * sizeof(int64);
			uint32 nValSize =  m_nSize * sizeof(TValue);


			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

			ZOrderPoint2DU64 zPoint;
			TValue nValue;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(zPoint.m_nZValue[0]);
				KeyStreams.read(zPoint.m_nZValue[1]);
				ValStreams.read(nValue);

				vecKeys.push_back(zPoint);
				vecValues.push_back(nValue);
			}
 
			return true;
		}
		virtual bool Write(TLeafKeyMemSet& vecKeys, TLeafValueMemSet& vecValues, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)vecKeys.size();
			assert(m_nSize == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream valueStreams;

			uint32 nKeySize =  nSize *2 * sizeof(int64);
			uint32 nValuesSize =  nSize * sizeof(TValue);

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			valueStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValuesSize);
			stream.seek(stream.pos() + nKeySize + nValuesSize, CommonLib::soFromBegin);			 

			for(size_t i = 0, sz = vecKeys.size(); i < sz; ++i)
			{
				ZOrderPoint2DU64& zPoint = vecKeys[i];
				KeyStreams.write(zPoint.m_nZValue[0]);
				KeyStreams.write(zPoint.m_nZValue[1]);
				valueStreams.write(vecValues[i]);
			}

			return true;
		}

		virtual bool insert(const ZOrderPoint2DU64& key, int64 link )
		{
			m_nSize++;
			return true;
		}
		virtual bool add(const TLeafKeyMemSet& keySet, const TLeafValueMemSet& linkSet)
		{
			m_nSize += keySet.size();
			return true;
		}
		virtual bool recalc(const TLeafKeyMemSet& keySet, const TLeafValueMemSet& linkSet)
		{
			m_nSize = keySet.size();
			return true;
		}
		virtual bool remove(const ZOrderPoint2DU64& key)
		{
			m_nSize--;
			return true;
		}
		virtual bool update(const ZOrderPoint2DU64& key, const TValue& value)
		{
			return true;
		}
		virtual size_t size() const
		{
			return (2 *sizeof(uint64) + sizeof(TValue)) *  m_nSize + sizeof(uint32);
		}
		virtual size_t count() const
		{
			return m_nSize;
		}
		size_t headSize() const
		{
			return   sizeof(uint32);
		}
		size_t rowSize() const
		{
			return (2 *sizeof(uint64) + sizeof(TValue)) *  m_nSize;
		}
		size_t tupleSize() const
		{
			return  (2 *sizeof(uint64) + sizeof(TValue));
		}
	private:
		size_t m_nSize;

	};
}

#endif