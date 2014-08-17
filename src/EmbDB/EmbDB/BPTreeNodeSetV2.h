#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_NODE_SET_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_NODE_SET_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "CompressorParams.h"
#include "BPTreeInnerNodeSetv2.h"
#include "BPTreeLeafNodeSetv2.h"
namespace embDB
{



	template<class _TKey, class _TLink, class _TComp, class _Transaction, 
	class _TInnerCompressor, class _TLeafCompressor>
	class BPTreeNodeSetv2
	{
	public:

		typedef _TKey TKey;
		typedef _TLink TLink;
		typedef _TComp TComp;
		typedef	_Transaction  Transaction;
		typedef _TInnerCompressor TInnerCompressor;
		typedef _TLeafCompressor TLeafCompressor;

		typedef BPTreeInnerNodeSetv2<TKey, TLink, TComp, Transaction, TInnerCompressor> TInnerNode;
		typedef BPTreeLeafNodeSetv2<TKey, TLink, TComp, Transaction,TLeafCompressor> TLeafNode;


		BPTreeNodeSetv2(int64 nParentAddr, CommonLib::alloc_t *pAlloc, int64 nPageAddr, bool bMulti, bool  bIsLeaf, bool bCheckCRC32,
			ICompressorParams *pInnerCompParams = NULL, ICompressorParams *pLeafCompParams = NULL) :
		m_bIsLeaf(bIsLeaf)
			,m_nPageAddr(nPageAddr)
			,m_pAlloc(pAlloc)
			,m_nParent(nParentAddr)
			,m_bMulti(bMulti)
			,m_pBaseNode(0)
			,m_LeafNode(pAlloc,  bMulti)
			,m_InnerNode(pAlloc, bMulti)
			,m_nFoundIndex(-1)
			,m_nType(0)
			,m_bValidNextKey(false)
			,m_pInnerCompParams(pInnerCompParams)
			,m_pLeafCompParams(pLeafCompParams)
			,m_bCheckCRC32(bCheckCRC32)

		{

		}
		~BPTreeNodeSetv2()
		{

		}

		bool Load(Transaction* pTransactions)
		{
			if(m_nPageAddr == -1)
			{

				CFilePage* pFilePage = pTransactions->getNewPage();
				if(!pFilePage)
					return false;

				m_nPageAddr = pFilePage->getAddr();
				if(m_bIsLeaf)
					m_pBaseNode = &m_LeafNode;
				else
					m_pBaseNode = &m_InnerNode;
				return m_pBaseNode->init(m_bIsLeaf ? m_pLeafCompParams : m_pInnerCompParams);
			}
			CFilePage* pFilePage =  pTransactions->getFilePage(m_nPageAddr);
			assert(pFilePage);
			if(!pFilePage)
				return false; 
			return LoadFromPage(pFilePage, pTransactions);
		}
		bool Save(Transaction* pTransactions)
		{
			CFilePage *pFilePage = NULL;
			if(m_nPageAddr != -1)
				pFilePage = pTransactions->getFilePage(m_nPageAddr, false);
			else
				pFilePage = pTransactions->getNewPage();

			if(!pFilePage)
				return false;


			pFilePage->setFlag(eFP_CHANGE, true);
			CommonLib::FxMemoryWriteStream stream;

			sFilePageHeader header;
			header.m_nObjectPageType = BTREE_PAGE;
			if(getFlags() & ROOT_NODE)
				header.m_nSubObjectPageType = BTREE_ROOT_PAGE;
			else if(m_bIsLeaf)
				header.m_nSubObjectPageType = BTREE_LEAF_PAGE;
			else
				header.m_nSubObjectPageType = BTREE_INNER_PAGE;
			stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
			header.write(stream);
			stream.write(m_bIsLeaf);
			//stream.write(m_bMulti);
			assert(m_pBaseNode);
			m_pBaseNode->Save(stream);
			if(m_bCheckCRC32)
				header.writeCRC32(stream);
			pFilePage->setCheck(true);
			pTransactions->saveFilePage(pFilePage);
			return true;
		}
		bool LoadFromPage(CFilePage* pFilePage, Transaction* pTransactions)
		{
			CommonLib::FxMemoryReadStream stream;
			stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
			sFilePageHeader header(stream, m_bCheckCRC32 && !pFilePage->isCheck());
			if( m_bCheckCRC32 && !pFilePage->isCheck() && !header.isValid())
			{
				CommonLib::str_t sMsg;
				sMsg.format(_T("BTREE: Page %I64d Error CRC for node page"), pFilePage->getAddr());
				pTransactions->error(sMsg);
				return false;
			}
			pFilePage->setCheck(true);

			m_bIsLeaf = stream.readBool();
			//m_bMulti = stream.readBool();
			if(m_bIsLeaf)
			{
				m_pBaseNode = &m_LeafNode;
			}
			else
			{
				m_pBaseNode = &m_InnerNode;
			}
			if(!m_pBaseNode->init(m_bIsLeaf ? m_pLeafCompParams : m_pInnerCompParams))
				return false;
			return m_pBaseNode->Load(stream);


		}
		bool IsFree()
		{
			return m_pBaseNode->IsFree();
		}
		size_t size()
		{
			assert(m_pBaseNode);
			return sFilePageHeader::size() + 1 + 3 * sizeof(int16) + m_pBaseNode->size();
		}
		size_t headSize()
		{
			assert(m_pBaseNode);
			return sFilePageHeader::size() + 1 + 3 * sizeof(int16) + m_pBaseNode->headSize();
		}
		size_t rowSize()
		{
			assert(m_pBaseNode);
			return  m_pBaseNode->rowSize();
		}
		bool isLeaf() const
		{
			assert(m_pBaseNode);
			return m_pBaseNode->isLeaf();
		}

		bool insertInLeaf( const TKey& key)
		{
			assert(m_bIsLeaf);
			return m_LeafNode.insert(key);
		}

		bool insertInInnerNode(const TKey& key, TLink nLink)
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.insert(key, nLink);
		}
		int getFlags()
		{
			assert(m_pBaseNode);
			return m_pBaseNode->getFlags();
		}
		size_t tupleSize() const
		{
			assert(m_pBaseNode);
			return m_pBaseNode->tupleSize();
		}
		void setFlags(int nFlag, bool bSet)
		{
			m_pBaseNode->setFlags(nFlag,bSet);
		}
		TLink findNodeInsert(const TKey& key)
		{
			assert(!m_bIsLeaf);
			int32 nIndex = -1;
			return m_InnerNode.upper_bound(key, nIndex);
		}
		TLink findNext(const TKey& key , int32& nIndex)
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.findNext(key, nIndex);
		}
		TLink inner_lower_bound(const TKey& key, short& nType, int32& nIndex )
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.lower_bound(key, nType, nIndex);
		}
		TLink  inner_upper_bound(const TKey& key, int32& nIndex )
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.upper_bound(key, nIndex);
		}
		int32 leaf_lower_bound(const TKey& key, short& nType)
		{
			assert(m_bIsLeaf);
			return m_LeafNode.lower_bound(key, nType);
		}
		int32  leaf_upper_bound(const TKey& key)
		{
			assert(m_bIsLeaf);
			return m_LeafNode.upper_bound(key);
		}
		int64 less()
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.m_nLess;
		}
		void setLess(TLink nLess)
		{
			assert(!m_bIsLeaf);
			m_InnerNode.m_nLess = nLess;
		}
		TLink next()
		{
			assert(m_bIsLeaf);
			return m_LeafNode.m_nNext;
		}
		TLink prev()
		{
			assert(m_bIsLeaf);
			return m_LeafNode.m_nPrev;
		}
		void setNext(TLink nNextAddr)
		{
			assert(m_bIsLeaf);
			m_LeafNode.m_nNext = nNextAddr;
		}
		void setPrev(TLink nPrevAddr)
		{
			assert(m_bIsLeaf);
			m_LeafNode.m_nPrev = nPrevAddr;
		}
		size_t count() const
		{
			if(m_bIsLeaf)
				return m_LeafNode.count();
			return m_InnerNode.count();
		}
		TLink addr() const
		{
			return m_nPageAddr;
		}

		bool splitIn(BPTreeNodeSetv2 *pNewNode, TKey* pSplitKey)
		{
			if(m_bIsLeaf)
				return m_LeafNode.SplitIn(&pNewNode->m_LeafNode, pSplitKey);
			return m_InnerNode.SplitIn(&pNewNode->m_InnerNode, pSplitKey);
		}
		int32 search(const TKey& key)
		{
			assert(m_bIsLeaf);
			if(m_bIsLeaf)
			{
				return m_LeafNode.binary_search(key);
			}
			return -1;
		}

		TLink backLink()
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.m_innerLinkMemSet.back();
		}

		TLink frontLink()
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.m_innerLinkMemSet.front();
		}
		TLink link(int32 nIndex)
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.link(nIndex);
		}
		void updateLink(int32 nIndex, TLink nLink)
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.updateLink(nIndex, nLink);
		}
		void updateKey(int32 nIndex, const TKey& Key)
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.updateKey(nIndex, Key);
		}
		const TKey& key(int32 nIndex) const
		{
			if(m_bIsLeaf)
				return m_LeafNode.key(nIndex);
			return m_InnerNode.key(nIndex);
		}

		TKey& key(int32 nIndex)
		{
			if(m_bIsLeaf)
				return m_LeafNode.key(nIndex);
			return m_InnerNode.key(nIndex);
		}

		bool AlignmentOf(BPTreeNodeSetv2 *pNode, bool bFromLeft)
		{
			return m_LeafNode.AlignmentOf(&pNode->m_LeafNode, bFromLeft);
		}
		bool AlignmentInnerNodeOf(BPTreeNodeSetv2 *pNode,  const TKey& lessMin, bool bLeft)
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.AlignmentOf(&pNode->m_InnerNode, lessMin, bLeft);
		}

		bool UnionWith(BPTreeNodeSetv2* pNode, bool bLeft)
		{
			assert(m_bIsLeaf);
			return m_LeafNode.UnionWith(&pNode->m_LeafNode, bLeft);
		 
		}
		bool UnioInnerWith(BPTreeNodeSetv2* pNode, const TKey& lessMin, bool bLeft)
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.UnionWith(&pNode->m_InnerNode, lessMin, bLeft);

		}
		void removeByIndex(int32 nIndex)
		{
			if(m_bIsLeaf)
				 m_LeafNode.removeByIndex(nIndex);
			else
				m_InnerNode.removeByIndex(nIndex);
		}

		bool isKey(const TKey& key, uint32 nIndex)
		{
			if(m_bIsLeaf)
				return m_LeafNode.isKey(key, nIndex);
			else
				return m_InnerNode.isKey(key, nIndex);
		}
	public:
		BPBaseTreeNode* m_pBaseNode;
		TLeafNode    m_LeafNode;
		TInnerNode	m_InnerNode;

		bool m_bIsLeaf;
		bool m_bMulti;
		TLink m_nPageAddr;
		CommonLib::alloc_t* m_pAlloc;
		//for removing
		TLink m_nParent;
		int32 m_nFoundIndex;
		short m_nType; // найдена по ключу?
		//for spatial search
		TKey m_NextLeafKey;
		bool m_bValidNextKey;

		ICompressorParams *m_pInnerCompParams;
		ICompressorParams *m_pLeafCompParams;

		bool m_bCheckCRC32;
	};
}
#endif