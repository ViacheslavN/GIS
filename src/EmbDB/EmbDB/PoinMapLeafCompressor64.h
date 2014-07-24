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
		typedef RBMap<ZOrderPoint2DU64, TValue, ZPointComp64>   TLeafMemSet;

		typedef typename TLeafMemSet::TTreeNode  TTreeNode;

		BPSpatialPointLeafNodeMapSimpleCompressor64(ICompressorParams *pParms = NULL) : m_nSize(0)
		{}
		virtual ~BPSpatialPointLeafNodeMapSimpleCompressor64(){}
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

			ZOrderPoint2DU64 zPoint;
			TValue nval;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(zPoint.m_nZValue[0]);
				KeyStreams.read(zPoint.m_nZValue[1]);
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

			uint32 nKeySize =  nSize * 2 *sizeof(uint64);
			uint32 nValSize =  nSize * sizeof(TValue);

			stream.write(nKeySize);
			stream.write(nValSize);
			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

			TLeafMemSet::iterator it = Set.begin();
			for(; !it.isNull(); ++it)
			{
				ZOrderPoint2DU64& zPoint = it.key();
				KeyStreams.write(zPoint.m_nZValue[0]);
				KeyStreams.write(zPoint.m_nZValue[1]);
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
			return (2 *sizeof(uint64) + sizeof(TValue)) *  m_nSize + 3 * sizeof(uint32);
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