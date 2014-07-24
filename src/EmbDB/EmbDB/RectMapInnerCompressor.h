#ifndef _EMBEDDED_DATABASE_SPATIAL_RECT_BTREE_PLUS_INNER_NODE_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_SPATIAL_RECT_BTREE_PLUS_INNER_NODE_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CompressorParams.h"
namespace embDB
{

	template<typename _TInnerMemSet >
	class BPSpatialRectInnerNodeSimpleCompressor  
	{
	public:

		typedef _TInnerMemSet TInnerMemSet;
		typedef typename TInnerMemSet::TKey TCoordPoint;
		typedef typename TInnerMemSet::TValue TLink;
		typedef typename TInnerMemSet::TTreeNode  TTreeNode;

		BPSpatialRectInnerNodeSimpleCompressor(ICompressorParams *pParams) : m_nSize(0)
		{}
		virtual ~BPSpatialRectInnerNodeSimpleCompressor(){}
		virtual bool Load(TInnerMemSet& Set, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			Set.reserve(m_nSize);
			uint32 nKeySize = stream.readInt32();
			uint32 nLinkSize = stream.readInt32();

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);

			TCoordPoint zPoint;
			TLink nlink;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				size_t nCount = TCoordPoint::SizeInByte/8;
				for (size_t i = 0; i < nCount; ++i )
				{
					KeyStreams.read(zPoint.m_nZValue[i]);
				}
				LinkStreams.read(nlink);
				Set.insert(zPoint, nlink);
			}
			assert(LinkStreams.pos() < stream.size());
			return true;
		}
		virtual bool Write(TInnerMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)Set.size();
			assert(m_nSize == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream LinkStreams;

			uint32 nKeySize =  nSize *  TCoordPoint::SizeInByte;
			uint32 nLinkSize =  nSize * sizeof(TLink);

			stream.write(nKeySize);
			stream.write(nLinkSize);

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);


			TInnerMemSet::iterator it = Set.begin();
			for(; !it.isNull(); ++it)
			{
				size_t nCount =  TCoordPoint::SizeInByte/8;
				for (size_t i = 0; i < nCount; ++i )
				{
					KeyStreams.write(it.key().m_nZValue[i]);
				}
				LinkStreams.write(it.value());
			}
			assert((stream.pos() + KeyStreams.pos() +  LinkStreams.pos())< stream.size());
			return true;
		}

		virtual bool insert(TTreeNode *pObj)
		{
			m_nSize++;
			return true;
		}
		virtual bool remove(TTreeNode *pObj)
		{
			m_nSize--;
			return true;
		}
		virtual size_t size() const
		{
			return (TCoordPoint::SizeInByte + sizeof(TLink) ) *  m_nSize + 3* sizeof(uint32) ;
		}
		virtual size_t count() const
		{
			return m_nSize;
		}
		size_t headSize() const
		{
			return  3 * sizeof(uint32);
		}
		size_t rowSize()
		{
			return (TCoordPoint::SizeInByte+ sizeof(TLink)) *  m_nSize;
		}
		void clear()
		{
			m_nSize = 0;
		}
		size_t tupleSize() const
		{
			return  (TCoordPoint::SizeInByte + sizeof(TLink));
		}
	private:
		size_t m_nSize;
	};
}

#endif