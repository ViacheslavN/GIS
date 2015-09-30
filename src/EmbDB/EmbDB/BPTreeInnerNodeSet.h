#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_INNER_NODE_SET_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_INNER_NODE_SET_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "Key.h"
#include "RBMap.h"
#include "IDBTransactions.h"
#include "BTBaseNode.h"
#include "BPCompressors.h"
#include "BPInnerNodeSimpleCompressor.h"
#include "CompressorParams.h"
namespace embDB
{
	template<typename _TCompressor, typename _TInnerMemSet>
	class BPTreeInnerNodeSet :   public  BPBaseTreeNode
	{
	public:

	
		typedef _TInnerMemSet TInnerMemSet;
		typedef typename TInnerMemSet::TKey TKey;
		typedef typename TInnerMemSet::TValue TLink;
		typedef typename TInnerMemSet::TTreeNode TMemSetNode;
		typedef _TCompressor TCompressor;

		//typedef BPInnerNodeCompressorBase<TKey, TLink, TComp> TCompressor;


		BPTreeInnerNodeSet( CommonLib::alloc_t *pAlloc,  bool bMulti) :
		m_innerMemSet(pAlloc, bMulti), m_pCompressor(0), m_nLess(-1)

		{
			
		}

		virtual bool init(ICompressorParams *pParams = NULL)
		{
			assert(!m_pCompressor);
			/*switch(m_nCompID)
			{
			case SIMPLE_COMPRESSOR:
				m_pCompressor = new BPInnerNodeSimpleCompressor<TKey, TLink, TComp>();
				break;
			}
			return m_pCompressor != 0;*/
			m_pCompressor = new TCompressor(pParams);
			return true;
		}
		~BPTreeInnerNodeSet()
		{
			if(m_pCompressor)
				delete m_pCompressor;
		}

		virtual bool isLeaf() const {return false;}
		virtual size_t size() const
		{
			assert(m_pCompressor);
			return sizeof(TLink) +  m_pCompressor->size();
		}
		virtual size_t headSize() const
		{
			assert(m_pCompressor);
			return sizeof(TLink) +  m_pCompressor->headSize(); 
		}
		virtual size_t rowSize() const
		{
			assert(m_pCompressor);
			return m_pCompressor->rowSize();
		}
		virtual bool IsFree() const 
		{
			return !(m_nFlag & (ROOT_NODE | BUSY_NODE));
		}
		virtual  bool Save(	CommonLib::FxMemoryWriteStream& stream) 
		{
			stream.write(m_nLess);
			return m_pCompressor->Write(m_innerMemSet, stream);
		}
		virtual bool Load(CommonLib::FxMemoryReadStream& stream)
		{
			 stream.read(m_nLess); 
			return m_pCompressor->Load(m_innerMemSet, stream);
		}
		virtual bool insert(const TKey& key, TLink nLink)
		{
			assert(nLink!= 0);
			assert(m_pCompressor->count() == m_innerMemSet.size());
			TMemSetNode *pNode = m_innerMemSet.insert(key, nLink);
			bool bRet = false;
			if(!m_innerMemSet.isNull(pNode))
			{
				bRet = m_pCompressor->insert(pNode);
			}
			assert(m_pCompressor->count() == m_innerMemSet.size());
			return bRet;
		}
		virtual bool remove(const TKey& key)
		{
			assert(m_pCompressor->count() == m_innerMemSet.size());
			TMemSetNode* pNode = m_innerMemSet.findNode(key);
			if(m_innerMemSet.isNull(pNode))
				return false;
			m_pCompressor->remove(pNode);
			m_innerMemSet.deleteNode(pNode, true, true);
			assert(m_pCompressor->count() == m_innerMemSet.size());
			return true;
		}
		bool SplitIn(BPTreeInnerNodeSet *pNode, TKey* pMedianKey)
		{
			assert(m_innerMemSet.size());

			TInnerMemSet& nodeNewTree = pNode->m_innerMemSet;
			TMemSetNode* pCurr = m_innerMemSet.tree_maximim(m_innerMemSet.root());
			TCompressor* pNewNodeComp = pNode->m_pCompressor;
			size_t nSize = m_innerMemSet.size()/2;
			if(m_innerMemSet.size() < 3)
			{

				//m_innerMemSet.deleteNode(pCurr, true, false);
				//m_pCompressor->remove(pCurr);

				*pMedianKey = pCurr->m_key;  // Медианный элемент

				//nodeNewTree.insertNode(pCurr);
				//pNewNodeComp->insert(pCurr);

				//TTreeNode* pPrev = pCurr->m_pPrev;
				pNode->m_nLess =  pCurr->m_val;

				m_innerMemSet.deleteNode(pCurr, true, true);
				m_pCompressor->remove(pCurr);
				assert(pNode->m_nLess != -1);
				return true;
			}
			while(nSize)
			{
				
				m_innerMemSet.deleteNode(pCurr, true, false);
				m_pCompressor->remove(pCurr);

				TMemSetNode* pPrev = pCurr->m_pPrev;
				nodeNewTree.insertNode(pCurr);
				pNewNodeComp->insert(pCurr);
				pCurr = pPrev;
			 	--nSize;
			}

			*pMedianKey = pCurr->m_key;  // Медианный элемент
			TLink nLink = pCurr->m_val;
		
			m_pCompressor->remove(pCurr);
			m_innerMemSet.deleteNode(pCurr, true, true);

			pNode->m_nLess =  nLink;
			assert(pNode->m_nLess != -1);
			return true;
		}
		TLink findNodeForBTree(const TKey& key)
		{
			assert(m_nLess != -1);
			short nType;
			TMemSetNode *pFindNode = m_innerMemSet.findNodeForBTree(key, nType);
			if(nType == FIND_KEY)
			{
				assert(!m_innerMemSet.isNull(pFindNode));
				if(m_innerMemSet.isMulti())
					return  pFindNode->m_val;
				return -1;
			}
			if(!m_innerMemSet.isNull(pFindNode))
			{
				return  pFindNode->m_val;;
			}
			else
			{
				return m_nLess;
			}
			return -1;
		}
		TLink findNext(const TKey& key, TMemSetNode **pFindRBNode)
		{
			assert(m_nLess != -1);
			short nType = 0;
			if(pFindRBNode)
			*pFindRBNode = NULL;
			TMemSetNode *pRBNode = m_innerMemSet.findNodeForBTree(key, nType);

			if(!m_innerMemSet.isNull(pRBNode))
			{
				if(pFindRBNode)
				 *pFindRBNode = pRBNode;
				return pRBNode->m_val;
			}
			else
			{
				return m_nLess;
			}
		}
				  
		TMemSetNode *findLessOrEQNode(const TKey& key)
		{
			short nType = 0;
			return m_innerMemSet.findNodeForBTree(key, nType);
		}
		TLink findNextForDelete(const TKey& key, TMemSetNode **pFindRBNode, short& nType)
		{
			assert(m_nLess != -1);
			nType = 0;
			//if(*pFindRBNode)
				*pFindRBNode = NULL;
		
			TMemSetNode *pRBNode = m_innerMemSet.findNodeForBTree(key, nType);

			if(nType == FIND_KEY)
			{
				*pFindRBNode = pRBNode;
				assert(pRBNode->m_val != -1);
				return pRBNode->m_val;
			}
			else
			{
				if(!m_innerMemSet.isNull(pRBNode))
				{
					*pFindRBNode = pRBNode;
					assert(pRBNode->m_val != -1);
					return pRBNode->m_val;
				}
				else
				{
					return m_nLess;
				}

			}
			return -1;
		}
		int64 first()
		{
			TMemSetNode *pNode = m_innerMemSet.minimumNode();
			if(m_innerMemSet.isNull(pNode))
				return -1;
			return pNode->m_val;
		}
		int64 last()
		{
			TMemSetNode *pNode = m_innerMemSet.maximumNode();
			if(m_innerMemSet.isNull(pNode))
				return -1;
			return pNode->m_val;
		}
		bool deleteNode(TMemSetNode *pNode, bool bDecSize, bool bRemNode)
		{
			 m_pCompressor->remove(pNode);
			 m_innerMemSet.deleteNode(pNode, bDecSize, bRemNode);
			 return true;
		}
		bool AlignmentOf(BPTreeInnerNodeSet* pNode, bool bFromLeft, TKey& lastKey, TLink& nLastLink)
		{
			TInnerMemSet& nodeTree = pNode->m_innerMemSet;
			TCompressor* pNodeComp = pNode->m_pCompressor;

			int nCnt = ((m_innerMemSet.size() + nodeTree.size()))/2 - m_innerMemSet.size();
			//assert(nCnt > 0);
			if(nCnt <= 0)
				return false;
			TMemSetNode* pCurr = NULL;
			if(bFromLeft)
			{
				pCurr = nodeTree.maximumNode();
				while(nCnt)
				{
					
					pNodeComp->remove(pCurr);
					nodeTree.deleteNode(pCurr, true, false);
				

					TMemSetNode* pPrev = pCurr->m_pPrev;

					m_innerMemSet.insertNode(pCurr);
					m_pCompressor->insert(pCurr);
					pCurr = pPrev;
					--nCnt;
				}
			}
			else
			{
				pCurr = nodeTree.minimumNode();
				while(nCnt)
				{
					pNodeComp->remove(pCurr);
					nodeTree.deleteNode(pCurr, true, false);
					

					TMemSetNode* pNext = pCurr->m_pNext;

					m_innerMemSet.insertNode(pCurr);
					m_pCompressor->insert(pCurr);
					pCurr = pNext;
					--nCnt;
				}
			}
			lastKey = pCurr->m_key;
			nLastLink = pCurr->m_val;
			pNodeComp->remove(pCurr);
			nodeTree.deleteNode(pCurr, true, true);
			
		
			return true;
		}
		bool UnionWith(BPTreeInnerNodeSet* pNode)
		{
			TInnerMemSet& nodeTree = pNode->m_innerMemSet;
			size_t nSize = nodeTree.size();
			TMemSetNode* pCurr = nodeTree.minimumNode();
			while(nSize)
			{
				TMemSetNode* pNext = pCurr->m_pNext;
				m_innerMemSet.insertNode(pCurr);
				m_pCompressor->insert(pCurr);
				pCurr = pNext;
				--nSize;
			}
			nodeTree.dropTree();
			pNode->m_pCompressor->clear();
			return true;
		}
		bool InsertNode(BPTreeInnerNodeSet* pNode, TLink nPageAddr)
		{
			TInnerMemSet& nodeTree = pNode->m_innerMemSet;
			TTreeNode* pCurr = nodeTree.minimumNode();
			if(!nodeTree.isNull(pCurr))
			{
				m_innerMemSet.insert(pCurr->m_key, nPageAddr);
				m_pCompressor->insert(pCurr);
			}

			return true;
		}

		TKey& firstKey()
		{
			assert(!m_innerMemSet.isEmpty());
			return m_innerMemSet.minimumNode()->m_key;
		}
		size_t count() const 
		{
			return m_innerMemSet.size();
		}
		size_t tupleSize() const
		{
			return m_pCompressor->tupleSize();
		}
		TLink lastPage()
		{
			if(m_innerMemSet.isEmpty())
				return -1;
			return m_innerMemSet.maximumNode()->m_val;
		}
		bool isNull(TMemSetNode* pNode )
		{
			return m_innerMemSet.isNull(pNode);
		}
		TMemSetNode* lastNode()
		{
			return m_innerMemSet.maximumNode();
		}
		TMemSetNode* firstNode()
		{
			return m_innerMemSet.minimumNode();
		}
	public:
		TCompressor * m_pCompressor;
		TInnerMemSet m_innerMemSet;
		TLink m_nLess;
	};

}
#endif