#ifndef _EMBEDDED_DATABASE__SIMPLE_COMPRESSORS_H_
#define _EMBEDDED_DATABASE__SIMPLE_COMPRESSORS_H_

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "IDBStorage.h"
#include "Compressors.h"
namespace embDB
{
	template<class _TNodeElem>
	class SimpleNodeCompressor : public NodeCompressor<_TNodeElem>
	{
		public:

			typedef  NodeCompressor<_TNodeElem> TBase;
			typedef typename TBase::TNodeElem TNodeElem;
			typedef typename TBase::TMemSet TMemSet;
			typedef typename TBase::TVector TVector;
			typedef typename TNodeElem::TVal TVal;
			typedef typename TNodeElem::TKey TKey;
			SimpleNodeCompressor(CommonLib::alloc_t* pAlloc) : 
			  m_nSize(4 * sizeof(uint32))
			  ,m_pAlloc(pAlloc)
			{

			}
			~SimpleNodeCompressor()
			{

			}

			virtual bool LoadNode(TMemSet& Set, CommonLib::FxMemoryReadStream& stream)
			{
				CommonLib::FxMemoryReadStream KeyStreams;
				CommonLib::FxMemoryReadStream ValStreams;
				CommonLib::FxMemoryReadStream LessStreams;
				
				uint32 nSize = stream.readInt32();
				if(!nSize)
					return true;

				Set.reserve(nSize);

				uint32 nKeySize = stream.readInt32();
				uint32 nValSize = stream.readInt32();
				uint32 nLessSize = stream.readInt32();

				KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
				ValStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValSize);
				LessStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize + nValSize, nLessSize);
				//OIDs

				for (uint32 nIndex = 0; nIndex < nSize; ++nIndex)
				{
					TNodeElem key;
					KeyStreams.read(key.m_key);
					ValStreams.read(key.m_val);
					key.m_nLink = LessStreams.readInt64();
					Set.insert(key);
				}
				//m_nSize = nSize * (2 * sizeof(int64) + sizeof(key.val));
				return true;
			}
			virtual bool WriteNode(TMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
			{

				uint32 nSize = (uint32)Set.size();
				stream.write(nSize);
				if(!nSize)
					return true;


				CommonLib::FxMemoryWriteStream KeyStreams;
				CommonLib::FxMemoryWriteStream ValStreams;
				CommonLib::FxMemoryWriteStream LessStreams;


				uint32 nKeySize =  nSize * sizeof(TKey);
				uint32 nValSize =  nSize * sizeof(TVal);
				uint32 nLessSize =  nSize * sizeof(int64);
				stream.write(nKeySize);
				stream.write(nValSize);
				stream.write(nLessSize);
				KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
				ValStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValSize);
				LessStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize + nValSize, nLessSize);
				
			
			
				TMemSet::iterator it = Set.begin();
				for(; !it.isNull(); ++it)
				{
					TNodeElem& key = *it;
					 KeyStreams.write(key.m_key );
					 ValStreams.write(key.m_val);
					 LessStreams.write(key.m_nLink);
				}
				return true;
			}

			virtual bool LoadNode(TVector& vec, CommonLib::FxMemoryReadStream& stream)
			{

				CommonLib::FxMemoryReadStream KeyStreams;
				CommonLib::FxMemoryReadStream ValStreams;
				CommonLib::FxMemoryReadStream LessStreams;

				uint32 nSize = stream.readInt32();
				if(!nSize)
					return true;

				vec.reserve(nSize);

				uint32 nKeySize = stream.readInt32();
				uint32 nValSize = stream.readInt32();
				uint32 nLessSize = stream.readInt32();

				KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
				ValStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValSize);
				LessStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize + nValSize, nLessSize);
				//OIDs

				for (uint32 nIndex = 0; nIndex < nSize; ++nIndex)
				{
					TNodeElem key;
					KeyStreams.read(key.m_key);
					ValStreams.read(key.m_val);
					key.m_nLink = LessStreams.readInt64();
					vec.push_back(key);
				}
				//m_nSize = nSize * (2 * sizeof(int64) + sizeof(key.val));
				return true;
			}

			virtual bool insert(const TNodeElem& key)
			{
				m_nSize  +=  (2 * sizeof(int64) + sizeof(TVal));
				return true;
			}
			virtual bool remove(const TNodeElem& key)
			{
				m_nSize -= (2 * sizeof(int64) + sizeof(TVal));
				return true;
			}
			virtual uint32 size() const
			{
				return m_nSize/* + (2 * sizeof(int64) /*+ sizeof(key.val))*/; //что бы не делать преинсерт и премувж
			}
			virtual uint32 cnt_size(size_t nCnt)
			{
				return (2 * sizeof(int64) + sizeof(TVal)) * nCnt + 4 * sizeof(uint32);
			}
		private:
			CommonLib::alloc_t* m_pAlloc;
			uint32 m_nSize;
	};
}
#endif