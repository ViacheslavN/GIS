#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_SPATIAL_RECT_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_SPATIAL_RECT_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
//#include "SpatialKey.h"
#include "CompressorParams.h"
namespace embDB
{

	template</*typename _TCoordPoint, typename _TValue, */typename _TLeafMemSet  >
	class BPSpatialRectLeafNodeMapSimpleCompressor 
	{
	public:
		typedef _TLeafMemSet TLeafMemSet;
		typedef typename TLeafMemSet::TKey TCoordPoint;
		typedef typename TLeafMemSet::TValue TValue;

		typedef typename _TLeafMemSet::TTreeNode  TTreeNode;
		typedef typename TCoordPoint::TPointType  TPointType;
		//TRect2D<TPointType>         TRect;

		BPSpatialRectLeafNodeMapSimpleCompressor(ICompressorParams *pParams) : m_nSize(0)
		{}
		virtual ~BPSpatialRectLeafNodeMapSimpleCompressor(){}
		virtual bool Load(TLeafMemSet& Set, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValStreams;
			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			Set.reserve(m_nSize);

			uint32 nKeySize = stream.readInt32();
			uint32 nValSize = stream.readInt32();

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

			TCoordPoint zPoint;
			TValue nval;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				size_t nCount = TCoordPoint::SizeInByte/8;
				for (size_t i = 0; i < nCount; ++i )
				{
					KeyStreams.read(zPoint.m_nZValue[i]);
				}
				ValStreams.read(nval);
				Set.insert(zPoint, nval);
			}
			assert(ValStreams.pos() < stream.size());
			return true;
		}

		virtual bool Write(TLeafMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
		{
			assert(m_nSize == Set.size());
			uint32 nSize = (uint32)Set.size();
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream ValStreams;

			uint32 nKeySize =  nSize * TCoordPoint::SizeInByte;
			uint32 nValSize =  nSize * sizeof(TValue);

			stream.write(nKeySize);
			stream.write(nValSize);
			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

			TLeafMemSet::iterator it = Set.begin();
			for(; !it.isNull(); ++it)
			{
				size_t nCount = TCoordPoint::SizeInByte/8;
				for (size_t i = 0; i < nCount; ++i )
				{
					KeyStreams.write(it.key().m_nZValue[i]);
				}
				ValStreams.write(it.value());
			}
			assert((stream.pos() + KeyStreams.pos() +  ValStreams.pos())< stream.size());
			return true;
		}

		virtual bool insert(TTreeNode *pObj)
		{
			m_nSize++;
			return true;
		}
		virtual bool update(TTreeNode *pObj,  const TValue& nValue)
		{
			return true;
		}
		virtual bool remove(TTreeNode *pObj)
		{
			m_nSize--;
			return true;
		}
		virtual size_t size() const
		{
			return (TCoordPoint::SizeInByte + sizeof(TValue)) *  m_nSize + 3 * sizeof(uint32);
		}
		virtual size_t count() const
		{
			return m_nSize;
		}
		size_t headSize() const
		{
			return  3 * sizeof(uint32);
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