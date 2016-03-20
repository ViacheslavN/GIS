#ifndef _EMBEDDED_DATABASE_BASE_MULTI_RB_TREE_
#define _EMBEDDED_DATABASE_BASE_MULTI_RB_TREE_

#include "BaseRBTree.h"

namespace embDB
{
	template<class TKey>
	class TListElem
	{
	public:
		TListElem(const TKey& _key) : key_(_key), m_pNext(0), m_pPrev(0)
		{
		}
		TListElem *m_pNext;
		TListElem *m_pPrev;
		TKey key_;
	};


	template <class TKey>
	struct RBMultiNode
	{
		RBMultiNode(): 
			m_pLeft(0)
			,m_pRight(0)
			,m_pParent(0)
			,m_pPrev(0)
			,m_pNext(0)
			,m_pFirst(0)
			,m_pLast(0)
			,color_(RED)

	{}

		typedef TListElem<TKey> TListElem;

		RBMultiNode* m_pLeft;
		RBMultiNode* m_pRight;
		RBMultiNode* m_pParent;
		RBMultiNode* m_pPrev;
		RBMultiNode* m_pNext;
		COLOR  color_;
		//list
		TListElem *m_pFirst;
		TListElem *m_pLast;

		void push_top(const TKey& val, CommonLib::alloc_t *pAlloc){
			TListElem* pElem =  new (pAlloc->alloc(sizeof(TListElem))) TListElem(val);
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
		void push_back(const TKey& val, CommonLib::alloc_t *pAlloc){
			TListElem* pElem =  new (pAlloc->alloc(sizeof(TListElem))) TListElem(val);
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
		class TTreeNode = RBMultiNode<TypeKey>,
		class TComp = comp<TypeKey>
	>
	class _RBMultiTree
	{


	public:
		typedef TypeKey  TKeyVal;
		typedef TTreeNode TTreeNode;
		typedef TComp TComporator;
		typedef typename TTreeNode::TListElem TListElem;

		_RBMultiTree(CommonLib::alloc_t *pAlloc);
		_RBMultiTree(CommonLib::alloc_t *pAlloc, const TComporator& comp);
		~_RBMultiTree();

		bool insert(const TypeKey& key);
		void remove(const TypeKey& key);
		TTreeNode* findNode(const TypeKey& key);
		TTreeNode* findNodeForBTreeInsert(const TypeKey& key);
		uint32 size() const {return m_nSize;}
		bool isEmpty() const {return m_nSize == 0}

	protected:
		void deleteNode(TTreeNode *pNode);	
		void rotateLeft(TTreeNode *pNodeLeft);
		void rotateRight(TTreeNode *pNodeRight);
		void insertFixup(TTreeNode *pNode);
		void deleteFixup(TTreeNode *x, TTreeNode *x_par);
		void destroyTree();
		void destroyList(TListElem* pEl);
		void getNodeForInsert(TTreeNode **pNode, TTreeNode **pFindNode,  const TypeKey& key);
		void addNewNode(TTreeNode *pNode, TTreeNode* pNodeNew);

		TTreeNode* tree_successor(TTreeNode *pNode);
		TTreeNode* tree_minimum(TTreeNode *pNode);
		TTreeNode* tree_maximim(TTreeNode *pNode);
	protected:
		TTreeNode *m_pRoot;
		CommonLib::alloc_t *m_pAlloc;
		uint32 m_nSize;
		TComporator m_Cmp;
	};
	#include "BaseRBMultiTree.cpp"

}

#endif