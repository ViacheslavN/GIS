#ifndef _EMBEDDED_DATABASE_RB_SET_
#define _EMBEDDED_DATABASE_RB_SET_

#include "BaseRBTree.h"
namespace embDB
{

	template <
		class TypeKey,
		class TComp = comp<TypeKey>
	>
	class RBSet : public _RBTree<TypeKey, BaseRBTreeNode<TypeKey>, TComp >
	{
		public:
		
			typedef _RBTree<TypeKey, BaseRBTreeNode<TypeKey>, TComp > _Mybase;
			typedef typename  _Mybase::TTreeNode TTreeNode;
			typedef typename  _Mybase::TComporator TComporator;
	 
			RBSet(CommonLib::alloc_t *pAlloc = NULL, bool bMulti = false) : _Mybase(pAlloc, bMulti)
			{}
			RBSet(const TComporator& comp, CommonLib::alloc_t *pAlloc, bool bMulti = false) : _Mybase(comp, pAlloc, bMulti)
			{}
			~RBSet()
			{
	 
			}			
	
			void remove(const TypeKey& key){
				_Mybase::remove(key);
			}
			#include "RBSetIteratorImp.h"
			
			iterator begin()
			{
				return iterator(tree_minimum(root()), &m_NullHeadNode);
			}
			iterator last()
			{
				return iterator(tree_maximim(root()), &m_NullHeadNode);
			}
			iterator remove(iterator& it, bool bRev = false){
				TTreeNode* pFindNode = it.m_pNode;
				if(isNull(pFindNode))
					return it;
				iterator ret_it = it;
				bRev ? --ret_it : ++ret_it;
				deleteNode(pFindNode, true, true);
				//m_nSize -= 1;
				//m_pAlloc->free(pFindNode);
				return  ret_it;
			}
			iterator find(TypeKey key){
				return iterator(findNode(key), &m_NullHeadNode);
			}
	};
	//#include "RBSet.cpp"
}
#endif