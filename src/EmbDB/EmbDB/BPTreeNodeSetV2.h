#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_NODE_SET_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_NODE_SET_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "CompressorParams.h"
#include "BPTreeInnerNodeSetv2.h"
#include "BPTreeLeafNodeSetv2.h"
namespace embDB
{



	template<class _Transaction, class _TKey, class _TLink, class _TComp,
	class _TInnerCompressor, class _TLeafCompressor>
	class BPTreeNodeSetv2
	{
	public:
		typedef _TInnerNode  TInnerNode;
		typedef _TLeafNode   TLeafNode;
		typedef typename TInnerNode::TKey TKey;
		typedef typename TInnerNode::TLink TLink;
		typedef	_Transaction  Transaction;
		typedef typename TInnerNode::TInnerMemSet  TInnerMemSet;
		typedef typename TLeafNode::TLeafMemSet   TLeafMemSet;

		typedef typename TInnerMemSet::TTreeNode TInnerMemSetNode;
		typedef typename TLeafMemSet::TTreeNode TLeafMemSetNode;

		BPTreeNodeSet(int64 nParentAddr, CommonLib::alloc_t *pAlloc, int64 nPageAddr, bool bMulti, bool  bIsLeaf, bool bCheckCRC32,
			ICompressorParams *pInnerCompParams = NULL, ICompressorParams *pLeafCompParams = NULL) :
		m_bIsLeaf(bIsLeaf)
			,m_nPageAddr(nPageAddr)
			,m_pAlloc(pAlloc)
			,m_nParent(nParentAddr)
			,m_bMulti(bMulti)
			,m_pBaseNode(0)
			,m_LeafNode(pAlloc,  bMulti)
			,m_InnerNode(pAlloc, bMulti)
			,m_pParrentMemsetNode(0)
			,m_nType(0)
			,m_bValidNextKey(false)
			,m_pInnerCompParams(pInnerCompParams)
			,m_pLeafCompParams(pLeafCompParams)
			,m_bCheckCRC32(bCheckCRC32)

		{

		}
		~BPTreeNodeSet()
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
			return m_InnerNode.findNodeForBTree(key);
		}
		TLink findNext(const TKey& key, TInnerMemSetNode** pMemSetNode )
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.findNext(key, pMemSetNode);
		}

		TLink findNextForDelete(const TKey& key, TInnerMemSetNode** pNode, short& nType)
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.findNextForDelete(key, pNode, nType);
		}

		bool splitIn(BPTreeNodeSet *pNewNode, TKey* pSplitKey)
		{
			if(m_bIsLeaf)
				return m_LeafNode.SplitIn(&pNewNode->m_LeafNode, pSplitKey);
			return m_InnerNode.SplitIn(&pNewNode->m_InnerNode, pSplitKey);
		}

		TLeafMemSetNode* findNode(const TKey& key, TLeafMemSetNode* pFromNode = NULL)
		{
			assert(m_bIsLeaf);
			TLeafMemSetNode* pNode = m_LeafNode.m_leafMemSet.findNode(key, pFromNode);
			if(m_LeafNode.m_leafMemSet.isNull(pNode))
				return NULL;
			return pNode;
		}
		TLeafMemSetNode *findLessOrEQNode(const TKey& key, TLeafMemSetNode* pFromNode = NULL)
		{
			assert(m_bIsLeaf);
			short nTypeRet = 0;
			TLeafMemSetNode* pNode = m_LeafNode.m_leafMemSet.findLessOrEQNode(key, nTypeRet, pFromNode);
			if(m_LeafNode.m_leafMemSet.isNull(pNode))
				return NULL;
			if(nTypeRet == LQ_KEY)
				return NULL;
			return  pNode;
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

		bool removeRBLeafNode(TLeafMemSetNode* pNode)
		{
			assert(m_bIsLeaf);
			m_LeafNode.deleteNode(pNode, true, true);
			return true;
		}

		TInnerMemSetNode *findLessOrEQInnerNode(const TKey& key)
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.findLessOrEQNode(key);
		}
		TLink first()
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.first();
		}
		TLink last()
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.last();
		}
		TKey& firstKey()
		{
			if(m_bIsLeaf)
				return m_LeafNode.firstKey();
			else 
				return m_InnerNode.firstKey();
		}

		bool remove(const TKey& key)
		{
			if(m_bIsLeaf)
				return m_LeafNode.remove(key);
			return m_InnerNode.remove(key);
		}
		bool removeRBInnerNode(TInnerMemSetNode* pRBNode)
		{
			assert(!m_bIsLeaf);
			m_InnerNode.deleteNode(pRBNode, true, true);
			return true;
		}
		bool AlignmentOf(BPTreeNodeSet *pNode, bool bFromLeft)
		{
			return m_LeafNode.AlignmentOf(&pNode->m_LeafNode, bFromLeft);
		}
		bool AlignmentInnerNodeOf(BPTreeNodeSet *pNode, bool bFromLeft, TKey& lastKey, TLink& nLastLink)
		{
			assert(!m_InnerNode.isLeaf());
			return m_InnerNode.AlignmentOf(&pNode->m_InnerNode, bFromLeft, lastKey, nLastLink);
		}

		bool UnionWith(BPTreeNodeSet* pNode)
		{
			if(m_bIsLeaf)
				return m_LeafNode.UnionWith(&pNode->m_LeafNode);
			return m_InnerNode.UnionWith(&pNode->m_InnerNode);
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
		TLeafMemSetNode* firstLeftMemSetNode()
		{
			assert(m_bIsLeaf);
			return m_LeafNode.firstNode();
		}
		TLeafMemSetNode* lastLeftMemSetNode()
		{
			assert(m_bIsLeaf);
			return m_LeafNode.lastNode();
		}
		TInnerMemSetNode* lastInnerMemSetNode()
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.lastNode();
		}
		TInnerMemSetNode* firstInnerMemSetNode()
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.firstNode();
		}

		TLink lastPage()
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.lastPage();
		}

		bool isLeamMemsetNodeNull(TLeafMemSetNode *pLeafNode)
		{
			assert(m_bIsLeaf);
			return m_LeafNode.isNull(pLeafNode);
		}
		bool isInnerMemsetNodeNull(TInnerMemSetNode *pInnerNode)
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.isNull(pInnerNode);
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
		TInnerMemSetNode* m_pParrentMemsetNode;//Родительская  нода указывающая на данную
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