#ifndef _EMBEDDED_DATABASE_SPATIAL_POINT_BTREE_PLUS_INNER_NODE_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_SPATIAL_POINT_BTREE_PLUS_INNER_NODE_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CompressorParams.h"
namespace embDB
{

	template<typename _CoordPoint>
	class BPSpatialPointInnerNodeSimpleCompressor  
	{
	public:

		typedef _CoordPoint CoordPoint;
		typedef typename CoordPoint::ZValueType ZValueType;
		typedef int64 TLink;
		typedef  TBPVector<TKey> TKeyMemSet;
		typedef  TBPVector<TLink> TLinkMemSet;
	
		BPSpatialPointInnerNodeSimpleCompressor(ICompressorParams *pParms = NULL) : m_nSize(0)
		{}
		virtual ~BPSpatialPointInnerNodeSimpleCompressor(){}
		/*virtual bool Load(TInnerMemSet& Set, CommonLib::FxMemoryReadStream& stream)
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

			CoordPoint zPoint;
			TLink nlink;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(zPoint.m_nZValue);
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

			uint32 nKeySize =  nSize * sizeof(ZValueType);
			uint32 nLinkSize =  nSize * sizeof(TLink);

			stream.write(nKeySize);
			stream.write(nLinkSize);

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);


			TInnerMemSet::iterator it = Set.begin();
			for(; !it.isNull(); ++it)
			{
				KeyStreams.write(it.key().m_nZValue);
				LinkStreams.write(it.value());
			}
			assert((stream.pos() + KeyStreams.pos() +  LinkStreams.pos())< stream.size());
			return true;
		}*/

		virtual bool Load(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			keySet.reserve(m_nSize);
			linkSet.reserve(m_nSize);

			uint32 nKeySize =  m_nSize * sizeof(TKey);
			uint32 nLinkSize =  m_nSize * sizeof(int64);

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);

			CoordPoint zPoint;
			TLink nlink;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(zPoint.m_nZValue);
				LinkStreams.read(nlink);

				keySet.push_back(zPoint);
				linkSet.push_back(nlink);
			}
			stream.seek(LinkStreams.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);		
			return true;
		}
		virtual bool Write(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)keySet.size();
			assert(m_nSize == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream LinkStreams;

			uint32 nKeySize =  nSize * sizeof(TKey);
			uint32 nLinkSize =  nSize * sizeof(int64);

			/*stream.write(nKeySize);
			stream.write(nLinkSize);*/

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);			 
			for(size_t i = 0, sz = keySet.size(); i < sz; ++i)
			{
				KeyStreams.write(keySet[i].m_nZValue);
				LinkStreams.write(linkSet[i]);
			}
			
			return true;
		}

		virtual bool insert(const TKey& key, TLink link )
		{
			m_nSize++;
			return true;
		}
		virtual bool add(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nSize += keySet.size();
			return true;
		}
		virtual bool recalc(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nSize = keySet.size();
			return true;
		}
		virtual bool remove(const TKey& key, TLink link)
		{
			m_nSize--;
			return true;
		}
		virtual bool update(const TKey& key, TLink link)
		{
			return true;
		}
		virtual size_t size() const
		{
			return (sizeof(ZValueType) + sizeof(TLink) ) *  m_nSize +  sizeof(uint32) ;
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
			return (sizeof(ZValueType) + sizeof(TLink)) *  m_nSize;
		}
		void clear()
		{
			m_nSize = 0;
		}
		size_t tupleSize() const
		{
			return  (sizeof(ZValueType) + sizeof(TLink));
		}
	private:
		size_t m_nSize;
	};
}

#endif