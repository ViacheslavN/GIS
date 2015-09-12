#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_LEAF_FIXED_STRING_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_LEAF_FIXED_STRING_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/String.h"
#include "RBSet.h"
#include "BTVector.h"
#include "Key.h"
#include "BPCompressors.h"
namespace embDB
{

	template<typename _TKey,  typename _TComp>
	class BPLeafNodeFixedStringCompressor  : public BPLeafNodeCompressorBase<_TKey, CommonLib::CString, _TComp>
	{
	public:

		typedef  BPLeafNodeCompressorBase<_TKey, CommonLib::CString, _TComp> TBase;
		typedef typename TBase::TKey TKey;
		//typedef typename TBase::TValue TValue;
		typedef typename TBase::TComp TComp;
		typedef typename TBase::TLeafMemSet TLeafMemSet;
		typedef typename TBase::TLeafNodeObj TLeafNodeObj;
		typedef typename TBase::TLeafVector TLeafVector;
		typedef typename TBase::TTreeNode  TTreeNode;

		BPLeafNodeFixedStringCompressor(short nStringSize) : m_nCount(0), m_nStringSize(nStringSize)
		{}
		virtual ~BPLeafNodeFixedStringCompressor(){}
		virtual bool Load(TLeafMemSet& Set, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValStreams;
			m_nCount = stream.readInt32();
			if(!m_nCount)
				return true;

			Set.reserve(m_nSize);

			uint32 nKeySize = stream.readInt32();
			uint32 nValSize = stream.readInt32();

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

			TKey nkey;
			TValue nval;
			for (uint32 nIndex = 0; nIndex < m_nCount; ++nIndex)
			{
				KeyStreams.read(nkey);
				ValStreams.read(nval);
				Set.insert(nkey, nval);
			}
			assert(ValStreams.pos() < stream.size());
			return true;
		}
		virtual bool Load(TLeafVector& vec, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValStreams;
			m_nCount = stream.readInt32();
			if(!m_nCount)
				return true;

			vec.reserve(m_nCount);

			uint32 nKeySize = stream.readInt32();
			uint32 nValSize = stream.readInt32();

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

			TKey nkey;
			TValue nval;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(nkey);
				ValStreams.read(nval);
				vec.push_back(nkey, nval);
			}
			assert(ValStreams.pos() < stream.size());
			return true;
		}
		virtual bool Write(TLeafMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
		{
			assert(m_nCount == Set.size());
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
				KeyStreams.write(it.key());
				ValStreams.write(it.value());
			}
			assert((stream.pos() + KeyStreams.pos() +  ValStreams.pos())< stream.size());
			return true;
		}

		virtual bool insert(TTreeNode *pObj)
		{
			m_nCount++;
			return true;
		}
		virtual bool update(TTreeNode *pObj,  const TValue& nValue)
		{
			return true;
		}
		virtual bool remove(TTreeNode *pObj)
		{
			m_nCount--;
			return true;
		}
		virtual size_t size() const
		{
			return (sizeof(TKey) + sizeof(TValue)) *  m_nCount + 3 * sizeof(uint32);
		}
		virtual size_t count() const
		{
			return m_nCount;
		}
		size_t headSize() const
		{
			return  3 * sizeof(uint32);
		}
		size_t rowSize() const
		{
			return (sizeof(TKey) + sizeof(TValue)) *  m_nCount;
		}
	private:
		size_t m_nCount;
		size_t m_nStringSize;
	};
}

#endif