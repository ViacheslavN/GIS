#pragma 
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../CompressorParams.h"
#include "BPTreeInnerNodeSetv3.h"
#include "BPTreeLeafNodeSetv3.h"
#include "CommonLibrary/IRefCnt.h"
namespace embDB
{



	template<class _TKey, class _Transaction,
		class _TInnerCompressor, class _TLeafCompressor,
		class _TInnerNode,
		class _TLeafNode >
		class BPTreeNodeSetv3 
	{
	public:

		typedef _TKey TKey;
		typedef int64 TLink;
		//typedef _TComp TComp;
		typedef	_Transaction  Transaction;
		typedef _TInnerCompressor TInnerCompressor;
		typedef _TLeafCompressor TLeafCompressor;
		typedef _TInnerNode TInnerNode;
		typedef _TLeafNode  TLeafNode;

		typedef typename TInnerCompressor::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TLeafCompressor::TLeafCompressorParams TLeafCompressorParams;


		BPTreeNodeSetv3(int64 nParentAddr, CommonLib::alloc_t *pAlloc, int64 nPageAddr, bool bMulti, bool  bIsLeaf, bool bCheckCRC32,
			uint32 nPageSize, TInnerCompressorParams *pInnerCompParams = NULL, TLeafCompressorParams *pLeafCompParams = NULL) :
			m_bIsLeaf(bIsLeaf)
			, m_nPageAddr(nPageAddr)
			, m_pAlloc(pAlloc)
			, m_nParent(nParentAddr)
			, m_bMulti(bMulti)
			, m_pBaseNode(0)
			, m_LeafNode(pAlloc, bMulti, (nPageSize - (sFilePageHeader::size(bCheckCRC32) + 1)))
			, m_InnerNode(pAlloc, bMulti, (nPageSize - (sFilePageHeader::size(bCheckCRC32) + 1)))
			, m_nFoundIndex(-1)
			, m_nType(0)
			, m_bValidNextKey(false)
			, m_pInnerCompParams(pInnerCompParams)
			, m_pLeafCompParams(pLeafCompParams)
			, m_bCheckCRC32(bCheckCRC32)
			, m_nPageSize(nPageSize)
		{

		}
		~BPTreeNodeSetv3()
		{

		}

		bool Load(Transaction* pTransactions)
		{
			if (m_nPageAddr == -1)
			{

				FilePagePtr pFilePage = pTransactions->getNewPage(m_nPageSize);
				if (!pFilePage.get())
					return false;

				m_nPageAddr = pFilePage->getAddr();
				if (m_bIsLeaf)
				{
					m_pBaseNode = &m_LeafNode;
					return m_LeafNode.init(m_pLeafCompParams, pTransactions);
				}
				else
				{
					m_pBaseNode = &m_InnerNode;
					return m_InnerNode.init(m_pInnerCompParams, pTransactions);
				}

			}
			FilePagePtr pFilePage = pTransactions->getFilePage(m_nPageAddr, m_nPageSize);
			assert(pFilePage.get());
			if (!pFilePage.get())
				return false;

			return LoadFromPage(pFilePage.get(), pTransactions);
		}
		void PreSave(Transaction* pTransactions)
		{
			m_pBaseNode->PreSave();
		}
		bool Save(Transaction* pTransactions)
		{

			FilePagePtr pFilePage(NULL);
			if (m_nPageAddr != -1)
				pFilePage = pTransactions->getFilePage(m_nPageAddr, m_nPageSize, false);
			else
				pFilePage = pTransactions->getNewPage(m_nPageSize);

			if (!pFilePage.get())
				return false;


			CommonLib::FxMemoryWriteStream stream;

			sFilePageHeader header(pFilePage->getPageSize());
			header.m_nObjectPageType = BTREE_PAGE;
			if (getFlags() & ROOT_NODE)
				header.m_nSubObjectPageType = BTREE_ROOT_PAGE;
			else if (m_bIsLeaf)
				header.m_nSubObjectPageType = BTREE_LEAF_PAGE;
			else
				header.m_nSubObjectPageType = BTREE_INNER_PAGE;

			header.m_bCheckCRC = m_bCheckCRC32;

			stream.attachBuffer(pFilePage->getRowData(), pFilePage->getPageSize());
			header.write(stream);
			stream.write(m_bIsLeaf);
			//stream.write(m_bMulti);
			assert(m_pBaseNode);
			m_pBaseNode->Save(stream);

			if (m_bCheckCRC32)
				header.writeCRC32(stream);
			//	pFilePage->setCheck(true);
			pTransactions->saveFilePage(pFilePage, stream.pos(), true);

			//pFilePage->setFlag(eFP_CHANGE, false);
			return true;
		}
		bool LoadFromPage(CFilePage* pFilePage, Transaction* pTransactions)
		{

			CommonLib::FxMemoryReadStream stream;
			stream.attachBuffer(pFilePage->getRowData(), pFilePage->getPageSize());
			sFilePageHeader header(stream, pFilePage->getPageSize(), m_bCheckCRC32 /*&& !pFilePage->isCheck()*/);
			if (/* m_bCheckCRC32 && !pFilePage->isCheck() &&*/ !header.isValid())
			{
				pTransactions->error(L"BTREE: Page %I64d Error CRC for node page (crc page: %ud, calc crc %ud)", pFilePage->getAddr(), header.m_nCRC32, header.m_nCalcCRC32);
				return false;
			}
			//pFilePage->setCheck(true);

			m_bIsLeaf = stream.readBool();
			//m_bMulti = stream.readBool();
			if (m_bIsLeaf)
			{
				m_pBaseNode = &m_LeafNode;
				if (!m_LeafNode.init(m_pLeafCompParams, pTransactions))
					return false;
			}
			else
			{
				m_pBaseNode = &m_InnerNode;
				if (!m_InnerNode.init(m_pInnerCompParams, pTransactions))
					return false;
			}

			bool bRet = m_pBaseNode->Load(stream);
			return bRet;


		}

		bool TransformToInner(Transaction* pTransactions)
		{
			assert(m_bIsLeaf);
			m_bIsLeaf = false;
			m_pBaseNode = &m_InnerNode;
			if (!m_InnerNode.init(m_pInnerCompParams, pTransactions))
				return false;

			return true;
		}

		bool TransformToLeaf(Transaction* pTransactions)
		{
			assert(!m_bIsLeaf);
			m_bIsLeaf = true;
			m_pBaseNode = &m_LeafNode;
			if (!m_LeafNode.init(m_pLeafCompParams, pTransactions))
				return false;

			return true;
		}
		bool IsFree()
		{
			return RefCounter::isRemovable();
		}
		uint32 size()
		{
			assert(m_pBaseNode);
			return sFilePageHeader::size(m_bCheckCRC32) + 1 + /*3 * sizeof(int16) */+m_pBaseNode->size();
		}

		bool isNeedSplit() const
		{
			assert(m_pBaseNode);
			return m_pBaseNode->isNeedSplit();
		}

		uint32 headSize()
		{
			assert(m_pBaseNode);
			return sFilePageHeader::size(m_bCheckCRC32) + 1 + /*3 * sizeof(int16)*/ +m_pBaseNode->headSize();
		}
		uint32 rowSize()
		{
			assert(m_pBaseNode);
			return  m_pBaseNode->rowSize();
		}
		bool isLeaf() const
		{
			assert(m_pBaseNode);
			return m_pBaseNode->isLeaf();
		}
		template<class TComp>
		int insertInLeaf(TComp& comp, const TKey& key, int nInsertLeafIndex = -1)
		{
			assert(m_bIsLeaf);
			return m_LeafNode.insert(comp, key, nInsertLeafIndex);
		}
		template<class TComp>
		int insertInInnerNode(TComp& comp, const TKey& key, TLink nLink)
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.insert(comp, key, nLink);
		}
		int getFlags()
		{
			assert(m_pBaseNode);
			return m_pBaseNode->getFlags();
		}
		uint32 tupleSize() const
		{
			assert(m_pBaseNode);
			return m_pBaseNode->tupleSize();
		}
		void setFlags(int nFlag, bool bSet)
		{
			m_pBaseNode->setFlags(nFlag, bSet);
		}
		template<class TComp>
		TLink findNodeInsert(const TComp& comp, const TKey& key)
		{
			assert(!m_bIsLeaf);
			int32 nIndex = -1;
			return m_InnerNode.upper_bound(comp, key, nIndex);

		}
		/*TLink findNext(const TKey& key , int32& nIndex)
		{
		assert(!m_bIsLeaf);
		return m_InnerNode.findNext(key, nIndex);
		}*/
		template<class TComp>
		int32 binary_search(const TComp& comp, const TKey& key)
		{
			assert(m_bIsLeaf);
			return m_LeafNode.binary_search(comp, key);
		}
		template<class TComp>
		TLink inner_lower_bound(const TComp& comp, const TKey& key, short& nType, int32& nIndex)
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.lower_bound(comp, key, nType, nIndex);
		}
		template<class TComp>
		TLink  inner_upper_bound(const TComp& comp, const TKey& key, int32& nIndex)
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.upper_bound(comp, key, nIndex);
		}
		template<class TComp>
		int32 leaf_lower_bound(const TComp& comp, const TKey& key, short& nType)
		{
			assert(m_bIsLeaf);
			return m_LeafNode.lower_bound(comp, key, nType);
		}
		template<class TComp>
		int32  leaf_upper_bound(const TComp& comp, const TKey& key)
		{
			assert(m_bIsLeaf);
			return m_LeafNode.upper_bound(comp, key);
		}
		int64 less()
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.m_nLess;
		}
		void setLess(TLink nLess)
		{
			assert(!m_bIsLeaf);
			//assert(nLess != -1);
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
		uint32 count() const
		{
			if (m_bIsLeaf)
				return m_LeafNode.count();
			return m_InnerNode.count();
		}
		TLink addr() const
		{
			return m_nPageAddr;
		}

		int splitIn(BPTreeNodeSetv3 *pNewNode, TKey* pSplitKey)
		{
			if (m_bIsLeaf)
				return m_LeafNode.SplitIn(&pNewNode->m_LeafNode, pSplitKey);
			return m_InnerNode.SplitIn(&pNewNode->m_InnerNode, pSplitKey);
		}

		int splitIn(BPTreeNodeSetv3 *pLeftNode, BPTreeNodeSetv3 *pRightNode, TKey* pSplitKey)
		{
			if (m_bIsLeaf)
				return m_LeafNode.SplitIn(&pLeftNode->m_LeafNode, &pRightNode->m_LeafNode, pSplitKey);
			return m_InnerNode.SplitIn(&pLeftNode->m_InnerNode, &pRightNode->m_InnerNode, pSplitKey);
		}



		int32 search(const TKey& key)
		{
			assert(m_bIsLeaf);
			if (m_bIsLeaf)
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
			if (m_bIsLeaf)
				return m_LeafNode.key(nIndex);
			return m_InnerNode.key(nIndex);
		}

		TKey& key(int32 nIndex)
		{
			if (m_bIsLeaf)
				return m_LeafNode.key(nIndex);
			return m_InnerNode.key(nIndex);
		}

		bool AlignmentOf(BPTreeNodeSetv3 *pNode, bool bFromLeft)
		{
			return m_LeafNode.AlignmentOf(&pNode->m_LeafNode, bFromLeft);
		}
		bool AlignmentInnerNodeOf(BPTreeNodeSetv3 *pNode, const TKey& lessMin, bool bLeft)
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.AlignmentOf(&pNode->m_InnerNode, lessMin, bLeft);
		}

		bool UnionWith(BPTreeNodeSetv3* pNode, bool bLeft, int *nCheckIndex = 0)
		{
			assert(m_bIsLeaf);
			return m_LeafNode.UnionWith(&pNode->m_LeafNode, bLeft, nCheckIndex);

		}
		bool UnionInnerWith(BPTreeNodeSetv3* pNode, const TKey* lessMin, bool bLeft)
		{
			assert(!m_bIsLeaf);
			return m_InnerNode.UnionWith(&pNode->m_InnerNode, lessMin, bLeft);

		}
		void removeByIndex(int32 nIndex)
		{
			if (m_bIsLeaf)
				m_LeafNode.removeByIndex(nIndex);
			else
				m_InnerNode.removeByIndex(nIndex);
		}
		template<class TComp>
		bool isKey(TComp& comp, const TKey& key, uint32 nIndex)
		{
			if (m_bIsLeaf)
				return m_LeafNode.isKey(comp, key, nIndex);
			else
				return m_InnerNode.isKey(comp, key, nIndex);
		}
	

		void setParent(std::shared_ptr<BPTreeNodeSetv3>& pNode, int32 nFoundIndex = -1)
		{

			m_pParent =  pNode;
			m_nFoundIndex = nFoundIndex;
			if (pNode.get())
				m_nParent = pNode->addr();
			else
				m_nParent = -1;

		}
 

		int64 parentAddr() const { return m_nParent; }
		int32 foundIndex() const  { return m_nFoundIndex; }
		void setFoundIndex(int32 nFoundIndex) { m_nFoundIndex = nFoundIndex; }
		std::shared_ptr<BPTreeNodeSetv3> parentNodePtr() { return m_pParent.lock(); }

		void SetMinSplit(bool bOneSplit)
		{
			m_LeafNode.SetMinSplit(bOneSplit);
			m_InnerNode.SetMinSplit(bOneSplit);
		}

		virtual void clear()
		{
			if (m_bIsLeaf)
				m_LeafNode.clear();
			else
				m_InnerNode.clear();
		}


		bool IsHaveUnion(BPTreeNodeSetv3 *pNode)
		{
			assert(pNode);
			if (m_bIsLeaf)
				return m_LeafNode.IsHaveUnion(&pNode->m_LeafNode);
			else
				return m_InnerNode.IsHaveUnion(&pNode->m_InnerNode);
		}
		bool IsHaveAlignment(BPTreeNodeSetv3 *pNode)
		{
			assert(pNode);
			if (m_bIsLeaf)
				return m_LeafNode.IsHaveAlignment(&pNode->m_LeafNode);
			else
				return m_InnerNode.IsHaveAlignment(&pNode->m_InnerNode);
		}


		bool isHalfEmpty() const
		{
			if (m_bIsLeaf)
				return m_LeafNode.isHalfEmpty();
			else
				return m_InnerNode.isHalfEmpty();
		}
	public:

		BPBaseTreeNode* m_pBaseNode;
		TLeafNode    m_LeafNode;
		TInnerNode	m_InnerNode;

		bool m_bIsLeaf;
		bool m_bMulti;
		TLink m_nPageAddr;
		CommonLib::alloc_t* m_pAlloc;

		short m_nType; // ������� �� �����?
					   //for spatial search
		TKey m_NextLeafKey;
		bool m_bValidNextKey;






		TInnerCompressorParams *m_pInnerCompParams;
		TLeafCompressorParams *m_pLeafCompParams;

		bool m_bCheckCRC32;
	protected:
		//for removing
		typedef std::weak_ptr<BPTreeNodeSetv3> TParentNodePtr;
		TParentNodePtr m_pParent;
		int32 m_nFoundIndex;
		int64 m_nParent;
		uint32 m_nPageSize;
	};
}

