#ifndef _EMBEDDED_DATABASE_RB_MULTI_MAP_H_
#define _EMBEDDED_DATABASE_RB_MULTI_MAP_H_
#include "BaseRBMultiTree.h"

namespace embDB
{
	template<class TKey, class TVal>
	class TMapListElem
	{
	public:
		TMapListElem(const TKey& _key, const TVal& _val) : key_(_key), val_(_val), m_pNext(0), m_pPrev(0)
		{
		}
		TMapListElem *m_pNext;
		TMapListElem *m_pPrev;
		TKey key_;
		TVal val_;
	};

	template <class TKey, class TVal>
	struct RBMultiMapNode
	{
		RBMultiMapNode(): 
		m_pLeft(0)
		,m_pRight(0)
		,m_pParent(0)
		,m_pPrev(0)
		,m_pNext(0)
		,m_pFirst(0)
		,m_pLast(0)
		,color_(RED)

	{}

	typedef TMapListElem<TKey, TVal> TListElem;

	RBMultiMapNode* m_pLeft;
	RBMultiMapNode* m_pRight;
	RBMultiMapNode* m_pParent;
	RBMultiMapNode* m_pPrev;
	RBMultiMapNode* m_pNext;
	COLOR  color_;
	//list
	TListElem *m_pFirst;
	TListElem *m_pLast;

	void push_pop(const TKey& key, const TVal& val, CommonLib::alloc_t *pAlloc){
		TListElem* pElem =  new (pAlloc->alloc(sizeof(TListElem))) TListElem(key, val);
		if(!m_pFirst){
			m_pFirst = pElem;
			m_pLast= pElem;
		}
		else{
			pElem->pNext_ = m_pFirst;
			m_pFirst->pPrev_ = pElem;
			m_pFirst = pElem;
		}
	}
	void push_back(const TKey& key, const TVal& val, CommonLib::alloc_t *pAlloc){
		TListElem* pElem =  new (pAlloc->alloc(sizeof(TListElem))) TListElem(key, val);
		if(!m_pLast){
			m_pLast = pElem;
			m_pFirst= pElem;
		}
		else{
			pElem->m_pPrev = m_pLast;
			m_pLast->m_pNext = pElem;
			m_pLast = pElem;
		}
	}
	void remove(TListElem* pElement, CommonLib::alloc_t *pAlloc){
		if(pElement->m_pNext)
			pElement->m_pNext->m_pPrev = pElement->m_pPrev;
		else{
			//удаляем последний элемент
			m_pLast = pElement->m_pPrev;
		} 


		if(pElement->m_pPrev)
			pElement->m_pPrev->m_pNext = pElement->m_pNext;
		else{
			m_pFirst = pElement->m_pNext;
		}
		pAlloc->free(pElement);
	}

	};

	template <
		class TypeKey,
		class TypeVal,
		class TComp = comp<TypeKey>
	>
	class RBMultiMap : public _RBMultiTree<TypeKey, RBMultiMapNode<TypeKey, TypeVal>, TComp >
	{
	public:

		typedef _RBMultiTree<TypeKey, RBMultiMapNode<TypeKey, TypeVal>, TComp > _Mybase;
		typedef typename  _Mybase::TTreeNode TTreeNode;
		typedef typename  _Mybase::TComporator TComporator;
		typedef typename TTreeNode::TListElem TListNode;

		RBMultiMap(CommonLib::alloc_t *pAlloc) : _Mybase(pAlloc)
		{}
		RBMultiMap(CommonLib::alloc_t *pAlloc, const TComporator& comp) : _Mybase(pAlloc, comp)
		{}
		~RBMultiMap(){}		

		void insert(const TypeKey& key, const TypeVal& val){
			TTreeNode *pNode = NULL; 
			TTreeNode *pFindNode = NULL; 
			getNodeForInsert(&pNode, &pFindNode, key);
			if(pFindNode){
				pFindNode->push_back(key, val, m_pAlloc);
				m_nSize++;
				return;
			}
			TTreeNode* pNewNode =  new (m_pAlloc->alloc(sizeof(TTreeNode))) TTreeNode();
			pNewNode->push_back(key, val,  m_pAlloc);
			addNewNode(pNode, pNewNode);
		}

		void remove(const TypeKey& key){
			_Mybase::remove(key);
		}

#include "RBMuitiMapIteratorImp.h"

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
		iterator find(TypeKey key){
			TTreeNode* pFindNode = findNode(key);
			return iterator(pFindNode, pFindNode ? pFindNode->m_pFirst : NULL);
		}
	};
}
#endif