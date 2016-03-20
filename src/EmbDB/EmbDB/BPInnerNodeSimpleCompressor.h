#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_INNER_NODE_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_INNER_NODE_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "RBSet.h"
#include "BTVector.h"
#include "Key.h"
#include "BPCompressors.h"
namespace embDB
{

	template<typename _TKey, typename _TLink, typename _TComp>
	class BPInnerNodeSimpleCompressor  : public BPInnerNodeCompressorBase<_TKey, _TLink, _TComp>
	{
	public:

		typedef  BPInnerNodeCompressorBase<_TKey, _TLink, _TComp> TBase;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TComp TComp;
		typedef typename TBase::TInnerMemSet TInnerMemSet;
		typedef typename TBase::TInnerNodeObj TInnerNodeObj;
		typedef typename TBase::TInnerVector TInnerVector;
		typedef typename TBase::TTreeNode  TTreeNode;

		BPInnerNodeSimpleCompressor() : m_nSize(0)
		{}
		virtual ~BPInnerNodeSimpleCompressor(){}
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
			
			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);
			
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

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);

			TKey key;
			TLink nlink;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(key);
				LinkStreams.read(nlink);
				vec.push_back(key, nlink);
			}
			assert(stream.pos() < stream.size());
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

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);


			TInnerMemSet::iterator it = Set.begin();
			for(; !it.isNull(); ++it)
			{
				KeyStreams.write(it.key());
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
		virtual uint32 size() const
		{
			return (sizeof(TKey) + sizeof(TLink) ) *  m_nSize + 3* sizeof(uint32) ;
		}
		virtual uint32 count() const
		{
			return m_nSize;
		}
		uint32 headSize() const
		{
			return  3 * sizeof(uint32);
		}
		uint32 rowSize()
		{
			return (sizeof(TKey) + sizeof(TLink)) *  m_nSize;
		}
		void clear()
		{
			m_nSize = 0;
		}
		uint32 tupleSize() const
		{
			return  (sizeof(TKey) + sizeof(TLink));
		}
	private:
		uint32 m_nSize;
	};
}

#endif