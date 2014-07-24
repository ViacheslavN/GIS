#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_INNEROID_NODE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_INNEROID_NODE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "RBSet.h"
#include "BTVector.h"
#include "Key.h"
#include "BPCompressors.h"
namespace embDB
{

	template<typename _TKey, typename _TLink, typename _TComp>
	class BPInnerOIDNodeCompressor  : public BPInnerNodeCompressorBase<_TKey, _TLink, _TComp>
	{
	public:

		typedef  BPInnerNodeCompressorBase<_TKey, _TLink, _TComp> TBase;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TComp TComp;
		typedef typename TBase::TInnerMemSet TInnerMemSet;
		typedef typename TBase::TInnerNodeObj TInnerNodeObj;
		typedef typename TBase::TInnerVector TInnerVector;


		BPInnerOIDNodeCompressor() : m_nSize(0)
		{}
		virtual ~BPInnerOIDNodeCompressor(){}
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

			TKey key;
			TLink nlink;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(key);
				LinkStreams.read(nlink);
				Set.insert(key, nlink);
			}
			assert(LinkStreams.pos() < stream.size());
			return true;
		}
		virtual bool Load(TInnerVector& vec, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			vec.reserve(m_nSize);
			uint32 nKeySize = stream.readInt32();
			uint32 nLinkSize = stream.readInt32();

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);

			TKey key;
			TLink nlink;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(key);
				LinkStreams.read(nlink);
				vec.push_back(key, nlink);
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

			uint32 nKeySize =  nSize * sizeof(TKey);
			uint32 nLinkSize =  nSize * sizeof(int64);

			stream.write(nKeySize);
			stream.write(nLinkSize);

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);


			TInnerMemSet::iterator it = Set.begin();
			for(; !it.isNull(); ++it)
			{
				KeyStreams.write(it.key());
				LinkStreams.write(it.value());
			}
			assert((nKeySize + nLinkSize) < (stream.size() - stream.pos()));
			return true;
		}

		virtual bool insert(const TKey& key, TLink nLink)
		{
			m_nCnt++;
			m_nRowOIDSize += sizeof(TKey);
			m_nRowLinkSize += sizeof(nLink);
			if((m_nRowOIDSize + m_nRowLinkSize ) > m_nPageSize)
			{

			}
			return true;
		}
		virtual bool remove(const TKey& key, TLink nLink)
		{
			m_nCnt--;
			m_nRowOIDSize -= sizeof(TKey);
			m_nRowLinkSize -= sizeof(nLink);
			if((m_nRowOIDSize + m_nRowLinkSize ) > m_nPageSize)
			{

			}
			return true;
		}
		virtual size_t size() const
		{
			return (sizeof(TKey) + sizeof(TLink) ) *  m_nSize + 3* sizeof(uint32) ;
		}
		virtual size_t count() const
		{
			return m_nCnt;
		}
	private:
		size_t m_nPageSize;
		size_t m_nCnt;
		size_t m_nRowOIDSize; //размер OID-дов без сжатия
		size_t m_nRowLinkSize; //размер линков без сжатия
	};
}

#endif