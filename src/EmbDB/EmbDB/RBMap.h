#ifndef _EMBEDDED_DATABASE_RB_MAP_H_
#define _EMBEDDED_DATABASE_RB_MAP_H_
#ifndef ANDROID

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "BaseRBTree.h"
//#include "RBBaseIterator.h"
namespace embDB
{

	template <class TypeKey, class TypeValue>
	struct RBMapNode
	{
	 RBMapNode(const TypeKey& key, const TypeValue& val): 
		m_pLeft(0)
		,m_pRight(0)
		,m_pParent(0)
		,m_pPrev(0)
		,m_pNext(0)
		,color_(RED)
		,m_key(key)
		,m_val(val)
	{}

		RBMapNode(): 
		m_pLeft(0)
			,m_pRight(0)
			,m_pParent(0)
			,m_pPrev(0)
			,m_pNext(0)
			,color_(RED)
		{}

		RBMapNode* m_pLeft;
		RBMapNode* m_pRight;
		RBMapNode* m_pParent;
		RBMapNode* m_pPrev;
		RBMapNode* m_pNext;
		COLOR  color_;
		TypeKey m_key;
		TypeValue m_val;
	};


	

	
	template <
		class _TypeKey,
		class TypeVal,
		class TComp = comp<_TypeKey>
	>
	class RBMap : public _RBTree<_TypeKey, RBMapNode<_TypeKey, TypeVal>, TComp>
	{
		public:
			typedef _TypeKey     TKey;
			typedef _TypeKey     TypeKey;
			typedef TypeVal     TValue;
			typedef _RBTree<TypeKey, RBMapNode<TypeKey, TypeVal>, TComp> _Mybase;
			typedef typename  _Mybase::TTreeNode TTreeNode;
			typedef typename  _Mybase::TComporator TComporator;

			RBMap(CommonLib::alloc_t *pAlloc = NULL, bool bMulti = false) : _Mybase(pAlloc, bMulti)
			{}
			RBMap(const TComporator& comp, CommonLib::alloc_t *pAlloc = NULL, bool bMulti = false) : _Mybase(comp, pAlloc, bMulti)
			{}

			~RBMap()
			{
			
			}

			TTreeNode*  insert(const TypeKey& key, const TypeVal& val)
			{
					TTreeNode*  pTryNode = root();
					TTreeNode*  pWhereNode = &m_NullHeadNode;
					bool bAddleft = true;	// add to left of head if tree empty
					while (pTryNode != &m_NullHeadNode)
					{	// look for leaf to insert before (_Addleft) or after
							pWhereNode = pTryNode;

							bAddleft = m_Cmp.LE(key, pTryNode->m_key);
							//this->_Kfn(_Val),
							//this->_Key(_Trynode));	// favor right end
							pTryNode = bAddleft ? pTryNode->m_pLeft : pTryNode->m_pRight;
					}
					if (m_bMulti)
					{
						TTreeNode* pNewNode =  new (m_pAlloc->alloc(sizeof(TTreeNode))) TTreeNode(key, val);
						pNewNode->m_pLeft = &m_NullHeadNode;
						pNewNode->m_pRight = &m_NullHeadNode;
						pNewNode->m_pParent = &m_NullHeadNode;
						pNewNode->m_pNext = &m_NullHeadNode;
						pNewNode->m_pPrev = &m_NullHeadNode;
						return addNewNode(bAddleft, pWhereNode, pNewNode);
					}
						//return (_Pairib(_Insert(_Addleft, _Wherenode, _Node), true));


					/*
					{	// insert only if unique
					iterator _Where = iterator(_Wherenode, this);
					if (!_Addleft)
					;	// need to test if insert after is okay
					else if (_Where == begin())
					return (_Pairib(_Insert(true, _Wherenode, _Node), true));
					else
					--_Where;	// need to test if insert before is okay

					if (_DEBUG_LT_PRED(this->comp,
					this->_Key(_Where._Mynode()),
					this->_Kfn(_Val)))
					return (_Pairib(_Insert(_Addleft, _Wherenode, _Node), true));
					else
					{	// duplicate, don't insert
					_Dest_val(this->_Alval,
					_STD addressof(this->_Myval(_Node)));

					this->_Alnod.deallocate(_Node, 1);

					return (_Pairib(_Where, false));
					}
					}
					
					*/
					else
					{	// insert only if unique
						//iterator _Where = iterator(_Wherenode, this);
						TTreeNode*  pWhereNodePrevNode = pWhereNode;
						if (!bAddleft)
							;	// need to test if insert after is okay
						else if (pWhereNode == tree_minimum(root()))
							//return (_Pairib(_Insert(true, _Wherenode, _Node), true));
						{
							TTreeNode* pNewNode =  new (m_pAlloc->alloc(sizeof(TTreeNode))) TTreeNode(key, val);
							pNewNode->m_pLeft = &m_NullHeadNode;
							pNewNode->m_pRight = &m_NullHeadNode;
							pNewNode->m_pParent = &m_NullHeadNode;
							pNewNode->m_pNext = &m_NullHeadNode;
							pNewNode->m_pPrev = &m_NullHeadNode;
							return addNewNode(true, pWhereNode, pNewNode);
			
						}
						else
							pWhereNodePrevNode = pWhereNode->m_pPrev;	// need to test if insert before is okay

						if( m_Cmp.LE(pWhereNodePrevNode->m_key, key))
						{
							TTreeNode* pNewNode =  new (m_pAlloc->alloc(sizeof(TTreeNode))) TTreeNode(key, val);
							pNewNode->m_pLeft = &m_NullHeadNode;
							pNewNode->m_pRight = &m_NullHeadNode;
							pNewNode->m_pParent = &m_NullHeadNode;
							pNewNode->m_pNext = &m_NullHeadNode;
							pNewNode->m_pPrev = &m_NullHeadNode;
							return addNewNode(bAddleft, pWhereNode, pNewNode);
						}
						else
						{	
							return &m_NullHeadNode;
						}
					}
					return &m_NullHeadNode;
			}

			void remove(const TypeKey& key){
				_Mybase::remove(key);
			}

			void remove_delete_val(const TypeKey& key){
				TTreeNode* pDelNode = findNode(key);
				if(!pDelNode)
					return;
				deleteNode(pDelNode, false, false);
				m_nSize -= 1;
				delete pDelNode->value_;
				m_pAlloc->free(pDelNode);
			}

			void remove_free_val(const TypeKey& key, CommonLib::alloc_t *pAlloc){
				TTreeNode* pDelNode = findNode(key);
				if(!pDelNode)
					return;
				deleteNode(pDelNode, false, false);
				m_nSize -= 1;
				pAlloc->free(pDelNode->value_);
				m_pAlloc->free(pDelNode);
			}

			bool remove_ret_val(const TypeKey& key, TypeVal& retVal)
			{
				TTreeNode* pDelNode = findNode(key);
				if(!pDelNode)
					return false;
				retVal = pDelNode->value_;
				deleteNode(pDelNode, false, false);
				m_nSize -= 1;
				m_pAlloc->free(pDelNode);
				return true;

			}
			
			#include "MapIteratorImp.h"
			iterator begin(){
				return iterator(tree_minimum(root()), &m_NullHeadNode);
			}
			iterator last(){
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
}

#endif

#endif