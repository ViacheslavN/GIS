#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_NODE_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_NODE_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "Key.h"
#include "RBMap.h"
#include "BPCompressors.h"
//#include "IDBTransactions.h"
#include "BPSimpleCompessor.h"
#include "BaseRBTree.h"
#include "BPTreeLeafNode.h"
#include "BPTreeInnerNode.h"
namespace embDB
{


		template<typename _TKey, typename _TValue, typename _TLink, typename _TComp,
		class _Transaction, class _TInnerComp, class _TLeafComp, class _TInnerMemSet = RBMap<_TKey, _TLink, _TComp>
		, class _TLeafMemSet = RBMap<_TKey, _TValue, _TComp> >
		class BPTreeNode
		{
		public:
			typedef _TKey TKey;
			typedef _TValue TValue;
			typedef  _TComp TComp;
			typedef _TLink TLink;
			typedef	_Transaction  Transaction;
			typedef _TInnerComp TInnerCompressor;
			typedef _TLeafComp TLeafCompressor;
			typedef _TInnerMemSet  TInnerMemSet;
			typedef _TLeafMemSet   TLeafMemSet;
			typedef BPNodeCompressor<TKey, _TValue, _TLink, _TComp> TCompressor;
			typedef BPTreeLeafNode<TKey, _TValue,  _TLink, _TComp, _TLeafComp> TLeafNode;
			typedef BPTreeInnerNode<TKey, _TLink, _TComp, _TInnerComp > TInnerNode;
			typedef typename TInnerMemSet::TTreeNode TInnerMemSetNode;
			typedef typename TLeafMemSet::TTreeNode TLeafMemSetNode;
		
			BPTreeNode(int64 nParentAddr, CommonLib::alloc_t *pAlloc, int64 nPageAddr, bool bMulti, bool  bIsLeaf) :
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

			{

			}
			~BPTreeNode()
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
					return m_pBaseNode->init();
				}
				CFilePage* pFilePage =  pTransactions->getFilePage(m_nPageAddr);
				assert(pFilePage);
				if(!pFilePage)
					return false; 
				return LoadFromPage(pFilePage);
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
				stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
				stream.write(m_bIsLeaf);
				//stream.write(m_bMulti);
				assert(m_pBaseNode);
				m_pBaseNode->Save(stream);
				pTransactions->saveFilePage(pFilePage);
				return true;
			}
			bool LoadFromPage(CFilePage* pFilePage)
			{
				CommonLib::FxMemoryReadStream stream;
				stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
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
				if(!m_pBaseNode->init())
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
				return 1 + 3 * sizeof(int16) + m_pBaseNode->size();
			}
			size_t headSize()
			{
				assert(m_pBaseNode);
				return 1 + 3 * sizeof(int16) + m_pBaseNode->headSize();
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

			bool insertInLeaf( const TKey& key, const TValue& val)
			{
				assert(m_bIsLeaf);
				return m_LeafNode.insert(key, val);
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


			TLink findNext(const TKey& key)
			{
				assert(!m_bIsLeaf);
				return m_InnerNode.findNext(key);
			}
 


			TLink findNextForDelete(const TKey& key, TInnerMemSetNode** pNode, short& nType)
			{
				assert(!m_bIsLeaf);
				return m_InnerNode.findNextForDelete(key, pNode, nType);
			}

			bool splitIn(BPTreeNode *pNewNode, TKey* pSplitKey)
			{
				if(m_bIsLeaf)
					return m_LeafNode.SplitIn(&pNewNode->m_LeafNode, pSplitKey);
				return m_InnerNode.SplitIn(&pNewNode->m_InnerNode, pSplitKey);
			}

			TLeafMemSetNode* findNode(const TKey& key)
			{
				assert(m_bIsLeaf);
				TLeafMemSetNode* pNode = m_LeafNode.m_leafMemSet.findNode(key);
				if(m_LeafNode.m_leafMemSet.isNull(pNode))
					return NULL;
				return pNode;
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
			bool AlignmentOf(BPTreeNode *pNode, bool bFromLeft)
			{
				//if(m_bIsLeaf)
					return m_LeafNode.AlignmentOf(&pNode->m_LeafNode, bFromLeft);
				//return m_InnerNode.AlignmentOf(&pNode->m_InnerNode, bFromLeft);
			}
			bool AlignmentInnerNodeOf(BPTreeNode *pNode, bool bFromLeft, TKey& lastKey, TLink& nLastLink)
			{
				assert(!m_InnerNode.isLeaf());
				return m_InnerNode.AlignmentOf(&pNode->m_InnerNode, bFromLeft, lastKey, nLastLink);
			}
			


			bool UnionWith(BPTreeNode* pNode)
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
		};
}
#endif