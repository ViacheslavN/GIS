#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_SPATIAL_RECT_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_SPATIAL_RECT_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
//#include "SpatialKey.h"
#include "CompressorParams.h"
namespace embDB
{

	template<class _TCoordPoint, class _TValue>
	class BPSpatialRectLeafNodeMapSimpleCompressor 
	{
	public:
		typedef _TCoordPoint TCoordPoint;
		typedef  _TValue TValue;
		typedef  TBPVector<TCoordPoint> TLeafKeyMemSet;
		typedef  TBPVector<TValue> TLeafValueMemSet;

		BPSpatialRectLeafNodeMapSimpleCompressor(ICompressorParams *pParams) : m_nSize(0)
		{}
		virtual ~BPSpatialRectLeafNodeMapSimpleCompressor(){}
		virtual bool Load(TLeafKeyMemSet& vecKeys, TLeafValueMemSet& vecValues, CommonLib::FxMemoryReadStream& stream)
		{

			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValueStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			vecKeys.reserve(m_nSize);
			vecValues.reserve(m_nSize);

			uint32 nKeySize =  m_nSize *  TCoordPoint::SizeInByte;
			uint32 nValueSize =  m_nSize * sizeof(TValue);

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			ValueStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValueSize);

			TCoordPoint zPoint;
			TValue nval;
			size_t nCount = TCoordPoint::SizeInByte/8;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				for (size_t i = 0; i < nCount; ++i )
				{
					KeyStreams.read(zPoint.m_nZValue[i]);
				}
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

			uint32 nKeySize =  nSize * TCoordPoint::SizeInByte;
			uint32 nValuesSize =  nSize * sizeof(TValue);

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			valueStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValuesSize);

			stream.seek(stream.pos() + nKeySize + nValuesSize, CommonLib::soFromBegin);			
			size_t nCount = TCoordPoint::SizeInByte/8;

			for(size_t i = 0, sz = vecKeys.size(); i < sz; ++i)
			{
			
				TCoordPoint& coord = vecKeys[i];
				for (size_t j = 0; j < nCount; ++j )
				{
					KeyStreams.write(coord.m_nZValue[j]);
				}
				valueStreams.write(vecValues[i]);
			}
			return true;
		}

		virtual bool insert(const TCoordPoint& key, const TValue& value)
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
		virtual bool update(const TCoordPoint& key, const TValue& value)
		{
			return true;
		}
		virtual bool remove(const TCoordPoint& key)
		{
			m_nSize--;
			return true;
		}
		virtual size_t size() const
		{
			return (TCoordPoint::SizeInByte + sizeof(TValue)) *  m_nSize +  sizeof(uint32);
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
			return (TCoordPoint::SizeInByte  + sizeof(TValue)) *  m_nSize;
		}
		size_t tupleSize() const
		{
			return  (TCoordPoint::SizeInByte + sizeof(TValue));
		}
	private:
		size_t m_nSize;

	};
}

#endif