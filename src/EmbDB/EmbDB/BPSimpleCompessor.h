#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_SIMPLE_COMPRESSORS_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_SIMPLE_COMPRESSORS_H_

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "embDBInternal.h"
#include "BPCompressors.h"
namespace embDB
{
	template<typename _TKey, typename _TValue, typename _TLink, typename _TComp >
	class BPSimpleNodeCompressor : public BPNodeCompressor<_TKey, _TValue, _TLink, _TComp>
	{
	public:

		typedef  BPNodeCompressor<_TKey, _TValue, _TLink,  _TComp> TBase;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::TValue TValue;
		typedef typename TBase::TComp TComp;
		typedef typename TBase::TLink TLink;


		typedef typename TBase::TInnerMemSet TInnerMemSet;
		typedef typename TBase::TLeafMemSet TLeafMemSet;

		typedef typename TBase::TInnerNodeObj TInnerNodeObj;
		typedef typename TBase::TLeafNodeObj TLeafNodeObj;

		typedef typename TBase::TInnerVector TInnerVector;
		typedef typename TBase::TLeafVector TLeafVector;



		BPSimpleNodeCompressor() 
		{

		}
		~BPSimpleNodeCompressor()
		{
			
		}

		virtual bool LoadInnerNode(TInnerMemSet& Set, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			uint32 nSize = stream.readInt32();
			if(!nSize)
				return true;

			Set.reserve(nSize);

			uint32 nKeySize = stream.readInt32();
			uint32 nLinkSize = stream.readInt32();


			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);

		
			TKey key;
			TLink nlink;
			for (uint32 nIndex = 0; nIndex < nSize; ++nIndex)
			{
				KeyStreams.read(key);
				LinkStreams.read(nlink);
				Set.insert(key, nlink);
			}
			//m_nSize = nSize * (2 * sizeof(int64) + sizeof(key.val));
			assert(LinkStreams.pos() < stream.size());
			return true;
		}
		virtual bool LoadInnerNode(TInnerVector& vec, CommonLib::FxMemoryReadStream& stream)
		{
			return true;
		}
		virtual bool WriteInnerNode(TInnerMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)Set.size();
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
				KeyStreams.write(it.getKey());
				LinkStreams.write(it.getVal());
			}
			assert(LinkStreams.pos() < stream.size());
			return true;
		}


		virtual bool LoadLeafNode(TLeafMemSet& Set, CommonLib::FxMemoryReadStream& stream)
		{

			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValStreams;
			uint32 nSize = stream.readInt32();
			if(!nSize)
				return true;

			Set.reserve(nSize);

			uint32 nKeySize = stream.readInt32();
			uint32 nValSize = stream.readInt32();

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

			TKey nkey;
			TValue nval;
			for (uint32 nIndex = 0; nIndex < nSize; ++nIndex)
			{
				KeyStreams.read(nkey);
				ValStreams.read(nval);
				Set.insert(nkey, nval);
			}
			assert(ValStreams.pos() < stream.size());
			return true;
		}
		virtual bool LoadLeafNode(TLeafVector& vec, CommonLib::FxMemoryReadStream& stream)
		{
			return true;
		}
		virtual bool WriteLeafNode(TLeafMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)Set.size();
			stream.write(nSize);
			if(!nSize)
				return true;



			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream ValStreams;


			uint32 nKeySize =  nSize * sizeof(TKey);
			uint32 nValSize =  nSize * sizeof(TValue);

			stream.write(nKeySize);
			stream.write(nValSize);
			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);
			
			TLeafMemSet::iterator it = Set.begin();
			for(; !it.isNull(); ++it)
			{
				KeyStreams.write(it.getKey());
				ValStreams.write(it.getVal());
			}
			assert(ValStreams.pos() < stream.size());
			return true;
		}


		//for inner
		virtual bool insertInner(TKey key, int64 nLink)
		{
			return true;
		}
		//for leaf
		virtual bool insertLeaf(TKey key, TValue nValue)
		{
			return true;
		}

		virtual bool remove(TKey key)
		{
			return true;
		}


		virtual size_t cnt_size(size_t nCnt, bool bLeaf) const 
		{
			if(bLeaf)
			{
				return (sizeof(int64) + sizeof(TValue)) * nCnt;
			}

			return (2 *sizeof(TLink)) * nCnt;
		}

		virtual size_t size(bool bLeaf) const
		{
			assert(0);
			return 0;
		}
	
	private:
	
	};
}
#endif