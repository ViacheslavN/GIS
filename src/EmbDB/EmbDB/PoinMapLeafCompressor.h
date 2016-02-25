#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_SPATIAL_POINT_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_SPATIAL_POINT_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CompressorParams.h"
#include "BPVector.h"
#include "SpatialPointCompressor.h"
namespace embDB
{

	template<class _TCoordPoint, class _TValue, class _Transaction = IDBTransaction>
	class BPSpatialPointLeafNodeMapSimpleCompressor 
	{
	public:
		typedef _TCoordPoint TCoordPoint;
		typedef typename TCoordPoint::ZValueType ZValueType;
		typedef int64 TLink;

		typedef  _TValue TValue;
		typedef  TBPVector<TCoordPoint> TLeafKeyMemSet;
		typedef  TBPVector<TValue> TLeafValueMemSet;
		//TRect2D<TPointType>         TRect;
		typedef CompressorParamsBaseImp TLeafCompressorParams;
		BPSpatialPointLeafNodeMapSimpleCompressor(uint32 nPageSize, _Transaction *pTran = 0, CommonLib::alloc_t *pAlloc = 0, TLeafCompressorParams *pParms = NULL,
			TLeafKeyMemSet *pKeyMemset= NULL, TLeafValueMemSet *pValueMemSet = NULL) : m_nSize(0), m_nPageSize(nPageSize)
		{}


		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return NULL;
		}
		virtual ~BPSpatialPointLeafNodeMapSimpleCompressor(){}
		virtual bool Load(TLeafKeyMemSet& vecKeys, TLeafValueMemSet& vecValues, CommonLib::FxMemoryReadStream& stream)
		{

			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValueStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			vecKeys.reserve(m_nSize);
			vecValues.reserve(m_nSize);

			uint32 nKeySize =  m_nSize * sizeof(ZValueType);
			uint32 nValueSize =  m_nSize * sizeof(TValue);

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			ValueStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValueSize);
					
			TCoordPoint zPoint;
			TValue nval;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(zPoint.m_nZValue);
				ValueStreams.read(nval);

				vecKeys.push_back(zPoint);
				vecValues.push_back(nval);
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

			uint32 nKeySize =  nSize * sizeof(ZValueType);
			uint32 nValuesSize =  nSize * sizeof(TValue);

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			valueStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValuesSize);
			stream.seek(stream.pos() + nKeySize + nValuesSize, CommonLib::soFromBegin);			 
			for(size_t i = 0, sz = vecKeys.size(); i < sz; ++i)
			{
				KeyStreams.write(vecKeys[i].m_nZValue);
				valueStreams.write(vecValues[i]);
			}
			return true;
		}

		virtual bool insert(uint32 nIndex, const TCoordPoint& key, const TValue& value)
		{
			m_nSize++;
			return true;
		}
		virtual bool add(const TLeafKeyMemSet& vecKeys, const TLeafValueMemSet& vecValues)
		{
		m_nSize += vecKeys.size();
			return true;
		}
		virtual bool recalc(const TLeafKeyMemSet& vecKeys, const TLeafValueMemSet& vecValues)
		{
			m_nSize = vecKeys.size();
			return true;
		}
		virtual bool update(uint32 nIndex, const TCoordPoint& key, const TValue& value)
		{
			return true;
		}
		virtual bool remove(uint32 nIndex, const TCoordPoint& key, const TValue& value)
		{
			m_nSize--;
			return true;
		}
		virtual size_t size() const
		{
			return (sizeof(ZValueType) + sizeof(TValue)) *  m_nSize +  sizeof(uint32);
		}
		virtual bool isNeedSplit() const
		{
			return m_nPageSize < size();
		}
		virtual size_t count() const
		{
			return m_nSize;
		}
		size_t headSize() const
		{
			return  sizeof(uint32);
		}
		size_t rowSize() const
		{
			return (sizeof(ZValueType) + sizeof(TValue)) *  m_nSize;
		}
		size_t tupleSize() const
		{
			return  (sizeof(ZValueType) + sizeof(TValue));
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPSpatialPointLeafNodeMapSimpleCompressor *pCompressor)
		{
			uint32 nSize = nEnd- nBegin;

			m_nSize -= nSize;
			pCompressor->m_nSize += nSize;
		}
	private:
		size_t m_nSize;
		uint32 m_nPageSize;
	};
}

#endif