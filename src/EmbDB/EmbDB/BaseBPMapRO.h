#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_MAP_READ_ONLY_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_MAP_READ_ONLY_H_

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "embDBInternal.h"
#include "simple_stack.h"
#include "CacheNodes.h"
#include "DBMagicSymbol.h"
#include "BPTreeStatistics.h"
#include "BPMapNodeRO.h"
#include "BPMapIteratorRO.h"
#include "BPInnerNodeSetROSimpleCompressor.h"
#include "BPLeafNodeMapROSimpleCompressor.h"
#include "BaseBPTreeSetRO.h"
namespace embDB
{



	template <class _TKey, class _TValue, class _TLink, class _TComp, class _Transaction,
	class _TInnerComp  = BPInnerNodeSetROSimpleCompressor<_TKey, _TLink>,
	class _TLeafComp = BPLeafNodeMapROSimpleCompressor<_TKey, _TValue>,
	class _TBreeNode = BPTreeMapNodeRO<_TKey, _TValue, _TLink, _TComp, _Transaction, _TInnerComp, _TLeafComp> >
	class TBaseBPlusTreeMapRO : public TBaseBPlusTreeSetRO<_TKey, _TLink, _TComp, _Transaction, _TInnerComp,_TLeafComp, _TBreeNode>
	{
	public:


		typedef _TKey      TKey;
		typedef _TValue    TValue;
		typedef _TComp	   TComp;
		typedef _TLink    TLink;
		typedef _TBreeNode  TBTreeNode;
		typedef	_Transaction  Transaction;
		typedef _TInnerComp TInnerCompressor;
		typedef _TLeafComp TLeafCompressor;

		

		TBaseBPlusTreeMapRO(int64 nPageBTreeInfo, IDBTransaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, bool bMulti = false, bool bCheckCRC = true) :
			TBase(nPageBTreeInfo, pTransaction, pAlloc,nChacheSize, bMulti, bCheckCRC)
		{}
		~TBaseBPlusTreeMapRO()
		{}



		typedef TBaseBPlusTreeSetRO<TKey, TLink, TComp, Transaction, TInnerCompressor, TLeafCompressor, TBTreeNode> TBase;
		typedef BPTreeStatistics<TLink, Transaction, TKey> BPTreeStatisticsInfo;
		typedef TBPMapIteratorRO<TKey,  TValue, TLink, TComp, Transaction, TInnerCompressor, TLeafCompressor, TBTreeNode> iterator;


		iterator find(const TKey& key, bool bFirst = false)
		{

			TBTreeNode* pFindBTNode = 0;
			TBTreeNode* pParentNode = 0;
			int32 nIndex = 0;
			 
			if(m_nRootAddr == -1)
				loadBTreeInfo();
			if(m_nRootAddr == -1)
				return iterator(this, 0, 0);

			if(!m_pRoot)
			{
				CFilePage* pFilePage = m_pTransaction->getFilePage(m_nRootAddr);
				if(!pFilePage)
					return iterator(this, 0, 0);
				m_pRoot = new TBTreeNode(m_pAlloc, m_nRootAddr, m_bMulti, m_bCheckCRC32);
				if(!m_pRoot->LoadFromPage(pFilePage, m_pTransaction))
				{
					delete m_pRoot;
					m_pRoot = NULL;
					return iterator(this, 0, 0);
				}
			}
			if(!m_pRoot)
				return iterator(this, 0, 0);

			if(m_pRoot-> isLeaf())
			{
				nIndex = m_pRoot->findIndex(key);//m_leafMemSet.findNode(key);
				if(nIndex != -1)
					pFindBTNode = m_pRoot;

			}
			else
			{
				pParentNode = m_pRoot;
				int64 nNextAddr = m_pRoot->findNext(key);
				for (;;)
				{
					if( nNextAddr == -1)
						break;
					TBTreeNode* pNode = getNode(nNextAddr);
					if(pNode->isLeaf())
					{
						nIndex  = pNode->findIndex(key);
						if(nIndex != -1)
						{
							pFindBTNode = pNode;
							if(bFirst && m_bMulti)
							{
								if(nIndex == 0 && pNode->prev() != -1)
								{
									pNode = getNode(pNode->prev());
									int32 nSubIndex  = pNode->findIndex(key);
									if(nSubIndex != -1)
									{
										pFindBTNode = pNode;
										nIndex = nSubIndex;
									}
								}

							}
						}
						break;
					}
					pParentNode = pNode;
					nNextAddr = pNode->findNext(key);
				}
			}


			ClearChache();
			return  iterator(this, pFindBTNode, nIndex);

		}
		iterator begin()
		{
			if(m_nRootAddr == -1)
				loadBTreeInfo();
			if(m_nRootAddr == -1)
				return iterator(this, 0, 0);

			if(!m_pRoot)
			{
				CFilePage* pFilePage = m_pTransaction->getFilePage(m_nRootAddr);
				if(!pFilePage)
					return  iterator(this, 0, 0);
				m_pRoot = new TBTreeNode(m_pAlloc, m_nRootAddr, m_bMulti, m_bCheckCRC32);
				if(!m_pRoot->LoadFromPage(pFilePage, m_pTransaction))
				{
					delete m_pRoot;
					m_pRoot = NULL;
					return  iterator(this, 0, 0);;
				}
			}
			if(!m_pRoot)
				 return iterator(this, 0, 0);
			if(m_pRoot->isLeaf())
			{
				 return iterator(this, m_pRoot, 0);
			}


			int64 nNextAddr = m_pRoot->m_nLess;
			for (;;)
			{
				if( nNextAddr == -1)
					break;
				TBTreeNode* pNode = getNode(nNextAddr);
				if(pNode->isLeaf())
				{
					return iterator(this, pNode, 0);
				}
				nNextAddr = pNode->m_nLess;
			}
			 return iterator(this, 0, 0);
		}
	
	private:
	
		
	};
}
#endif