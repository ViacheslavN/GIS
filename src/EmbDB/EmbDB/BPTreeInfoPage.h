#ifndef _EMBEDDED_DATABASE_BP_TREE_INFO_PAGE_
#define _EMBEDDED_DATABASE_BP_TREE_INFO_PAGE_

#include "TranStorage.h"
#include "RBSet.h"
#include "FilePage.h"
#include "BTreeNode.h"
#include "BPTreeNode.h"
#include "BaseBPMap.h"
#include "BPInnerNodeSimpleCompressor.h"
#include "BPLeafNodeSimpleCompressor.h"

namespace embDB
{
	struct sFileTranPageInfo
	{
		sFileTranPageInfo(int64  nAddr = -1, int nFlags = -1) : m_nFileAddr(nAddr), m_nFlags(nFlags)
		{}
		int64  m_nFileAddr; // адрес страницы в файле транзакций
		uint32 m_nFlags; //флаги выгружаемой страницы
	};

	class BPNewPageLeafNodeCompressor 
	{
	public:
		typedef RBMap<int64, sFileTranPageInfo, embDB::comp<int64> >   TLeafMemSet;
		typedef TLeafMemSet::TTreeNode TTreeNode;

		BPNewPageLeafNodeCompressor(ICompressorParams *pParams = NULL) : m_nSize(0)
		{}
		virtual ~BPNewPageLeafNodeCompressor(){}
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

			int64 nkey;
			sFileTranPageInfo nval;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(nkey);
				ValStreams.read(nval.m_nFileAddr);
				ValStreams.read(nval.m_nFlags);
				Set.insert(nkey, nval);
			}
			assert(stream.pos() <= stream.size());
			return true;
		}
		virtual bool Write(TLeafMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
		{
			assert(m_nSize == (uint32)Set.size());
			uint32 nSize = (uint32)Set.size();
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream ValStreams;

			uint32 nKeySize =  nSize * sizeof(int64);
			uint32 nValSize =  nSize * (sizeof(int64) + sizeof(int32));

			stream.write(nKeySize);
			stream.write(nValSize);
			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

			TLeafMemSet::iterator it = Set.begin();
			for(; !it.isNull(); ++it)
			{
				KeyStreams.write(it.key());
				ValStreams.write(it.value().m_nFileAddr);
				ValStreams.write(it.value().m_nFlags);
			}
			assert(stream.pos() <= stream.size());
			return true;
		}

		virtual bool insert(TTreeNode *pNode)
		{
			m_nSize++;
			return true;
		}
		virtual bool update(TTreeNode *pNode, const sFileTranPageInfo& nValue)
		{
			return true;
		}
		virtual bool remove(TTreeNode *pNode)
		{
			m_nSize--;
			return true;
		}
		virtual size_t size() const
		{
			//return (sizeof(int64) + sizeof(sFileTranPageInfo)) *  m_nSize;
			return ( (2 * sizeof(int64) + sizeof(int32)) *  m_nSize) + 3 * sizeof(uint32);
		}
		size_t headSize() const
		{
			//return (sizeof(int64) + sizeof(sFileTranPageInfo)) *  m_nSize;
			return  3 * sizeof(uint32);
		}
		 size_t rowSize() const
		{
			//return (sizeof(int64) + sizeof(sFileTranPageInfo)) *  m_nSize;
			return (2 * sizeof(int64) + sizeof(int32)) *  m_nSize;
		}
		size_t count()
		{
			return m_nSize;
		}
		size_t tupleSize() const
		{
			return  (sizeof(int64) + sizeof(int32));
		}
	private:
		size_t m_nSize;
	};

	class  BPNewPageStorage
	{
	public:	
		BPNewPageStorage(CTranStorage *pTranStorage, CommonLib::alloc_t *pAlloc);
		~BPNewPageStorage();
		int64 saveFilePage(CFilePage* pPage);
		CFilePage* getFilePage(int64 nAddr, bool bRead = true);
		CFilePage* getNewPage();
		void error(const CommonLib::str_t& sError){}
		size_t getPageSize(){return m_pTranStorage->getPageSize();}
		bool dropFilePage(CFilePage* pPage){return true;}
		bool dropFilePage(int64 pAddrPage){return true;}
	private:
		CTranStorage *m_pTranStorage;
		size_t m_nMaxPageBuf;
		typedef TSimpleCache<int64, CFilePage> TNodesCache;
		TNodesCache m_Chache;

	};


}
#endif