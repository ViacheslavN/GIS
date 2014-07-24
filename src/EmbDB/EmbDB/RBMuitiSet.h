#ifndef _EMBEDDED_DATABASE_RB_MULTI_SET_
#define _EMBEDDED_DATABASE_RB_MULTI_SET_


#include "BaseRBMultiTree.h"

namespace embDB
{
	

	template <
		class TypeKey,
		class TComp = comp<TypeKey>
	>
	class RBMultiSet : public _RBMultiTree<TypeKey, RBMultiNode<TypeKey>, TComp >
	{
	public:

		typedef _RBMultiTree<TypeKey, RBMultiNode<TypeKey>, TComp > _Mybase;
		typedef typename  _Mybase::TTreeNode TTreeNode;
		typedef typename  _Mybase::TComporator TComporator;
		typedef typename TTreeNode::TListElem TListNode;

		RBMultiSet(CommonLib::alloc_t *pAlloc) : _Mybase(pAlloc)
		{}
		RBMultiSet(CommonLib::alloc_t *pAlloc, const TComporator& comp) : _Mybase(pAlloc, comp)
		{}
		~RBMultiSet(){}			

		#include "RBMuitiSetIteratorImp.h"

		iterator begin(){
			if(!m_pRoot)
				return iterator(NULL, NULL);
			TTreeNode* pMinNode = tree_minimum(m_pRoot);
			return iterator(pMinNode, pMinNode ? pMinNode->m_pFirst : NULL);
		}
		iterator last(){
			if(!m_pRoot)
				return iterator(NULL, NULL);
			TTreeNode* pMaxNode = tree_maximim(m_pRoot);
			return iterator(pMaxNode, pMaxNode ? pMaxNode->m_pLast : NULL);
		}
		iterator remove(iterator& it, bool bRev = false){
			TTreeNode* pFindNode = it.m_pNode;
			TListNode* pListEl = it.m_pListNode;
			if(!pFindNode)
				return it;
			iterator ret_it = it;
			bRev ? --ret_it : ++ret_it;
			pFindNode->remove(pListEl, m_pAlloc);
			m_nSize -= 1;
			if(!pFindNode->m_pFirst){
				deleteNode(pFindNode);
				m_pAlloc->free(pFindNode);
			}
			return  ret_it;
		}
		void remove(const TypeKey& key){
			_Mybase::remove(key);
		}
		iterator find(TypeKey key){
			TTreeNode* pFindNode = findNode(key);
			return iterator(pFindNode, pFindNode ? pFindNode->m_pFirst : NULL);
		}
	};

}

#endif