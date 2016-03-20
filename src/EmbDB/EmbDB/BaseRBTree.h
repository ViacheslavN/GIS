#ifndef _EMBEDDED_DATABASE_BASERB_TREE_
#define _EMBEDDED_DATABASE_BASERB_TREE_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"

//#include "RBBaseIterator.h"
namespace embDB
{

	enum BTreeInsertRet
	{
		NOT_FOUND_KEY,
		FIND_KEY,
		LE_KEY,
		LQ_KEY
	};


template<class _Ty>
	struct comp
	{	
		bool LE(const _Ty& _Left, const _Ty& _Right) const
		{
			return (_Left < _Right);
		}
		bool EQ(const _Ty& _Left, const _Ty& _Right) const
		{
			return (_Left == _Right);
		}
		/*bool MR(const _Ty& _Left, const _Ty& _Right){
			return (_Left > _Right);
		}*/
	};
#ifndef ANDROID

#define RED  true
	#define BLACK false
	typedef  bool COLOR;

	template <class TypeKey>
	class BaseRBTreeNode
	{
	public:
	BaseRBTreeNode(): 
		m_pLeft(0)
		,m_pRight(0)
		,m_pParent(0)
		,m_pPrev(0)
		,m_pNext(0)
		,color_(RED)

	{
 
	}

	 BaseRBTreeNode(const TypeKey& key): 
		m_pLeft(0)
		,m_pRight(0)
		,m_pParent(0)
		,m_pPrev(0)
		,m_pNext(0)
		,color_(RED)
		,m_key(key)
	{}


		BaseRBTreeNode* m_pLeft;
		BaseRBTreeNode* m_pRight;
		BaseRBTreeNode* m_pParent;
		BaseRBTreeNode* m_pPrev;
		BaseRBTreeNode* m_pNext;
		COLOR  color_;
		TypeKey m_key;
	};

	


template <
	class TypeKey,
	class _TTreeNode = BaseRBTreeNode<TypeKey>,
	class TComp = comp<TypeKey>
>
class _RBTree
{


public:
	typedef TypeKey  TKey;
	typedef TypeKey  TKeyVal;
	typedef _TTreeNode TTreeNode;
	//typedef TIter iterator;
	typedef TComp TComporator;
	_RBTree(CommonLib::alloc_t *pAlloc = NULL, bool bMulti = false);
	_RBTree(const TComporator& comp, CommonLib::alloc_t *pAlloc = NULL, bool bMulti = false);
	~_RBTree();

	void clear();
	TTreeNode* insert(const TypeKey& key);
	TTreeNode* insertNode(TTreeNode* pNode); //for B Tree
	void insert_aw(const TypeKey& key);
	void remove(const TypeKey& key);
	void remove_erase_val(const TypeKey& key);
	void reserve(uint32 nSize){}
	TTreeNode* findNode(const TypeKey& key, TTreeNode* pFromNode = NULL);
	TTreeNode* findNodeForBTree(const TypeKey& key, short& nTypeRet);
	TTreeNode* findLessOrEQNode(const TypeKey& key, short& nTypeRet, TTreeNode* pFromNode);
	TTreeNode* findNodeForBIter(const TypeKey& key); //»щем первый  элемент который минимально больше ключа

	uint32 size() const {return m_nSize;}
	bool isEmpty() const {return m_nSize == 0;}
	bool isNull(TTreeNode *pNode) const {return pNode == &m_NullHeadNode;}
	
//protected:
  void deleteNode(TTreeNode *pNode, bool bDecSize = false, bool bRemoveNode = true);	
  void rotateLeft(TTreeNode *pNodeLeft);
  void rotateRight(TTreeNode *pNodeRight);
  TTreeNode* insertFixup(TTreeNode *pNode);
  void deleteFixup(TTreeNode *x, TTreeNode *x_par);
  void destroyTree();
  bool getNodeForInsert(TTreeNode **pNode, const TypeKey& key);
  bool getNodeForInsertAw(TTreeNode **pNode, const TypeKey& key);
  TTreeNode* addNewNode(bool bLeft, TTreeNode *pNode, TTreeNode* pNodeNew);

  TTreeNode* tree_successor(TTreeNode *pNode);
  TTreeNode* tree_minimum(TTreeNode *pNode);
  TTreeNode* tree_maximim(TTreeNode *pNode);

  TTreeNode* minimumNode();
   TTreeNode* maximumNode();

  void setMulti(bool bMulti)
  {
	  m_bMulti = bMulti;
  }
  bool isMulti() const 
  {
	  return m_bMulti;
  }
 TTreeNode* rmost() const
  {	// return rightmost node in nonmutable tree
	  return m_NullHeadNode.m_pRight;
  }

   TTreeNode* root() const
  {	// return root of nonmutable tree
	  return m_NullHeadNode.m_pParent;
  }
     TTreeNode* lmost() const
   {	// return leftmost node in nonmutable tree
	   return m_NullHeadNode.m_pLeft;
   }


	 TTreeNode*  nextDel( TTreeNode* pNode)
	 {	// preincrement
		 TTreeNode* pWhere = pNode;
		 if (pWhere == &m_NullHeadNode)
			 ;	// end() shouldn't be incremented, don't move
		 else if (pWhere->m_pRight != &m_NullHeadNode)
			 pWhere = tree_minimum(pWhere->m_pRight);	// ==> smallest of right subtree
		 else
		 {	// climb looking for right subtree
			  TTreeNode*  pPnode = pWhere->m_pParent ;
			 while (pPnode != &m_NullHeadNode && pWhere == pPnode->m_pRight)
				 pWhere = pPnode;	// ==> parent while right subtree
			 pWhere = pPnode;	// ==> parent (head if end())
		 }
		 return pWhere;
	 }
	bool dropTree()
	{
		m_NullHeadNode.color_ = BLACK;
		m_NullHeadNode.m_pLeft = &m_NullHeadNode;
		m_NullHeadNode.m_pRight = &m_NullHeadNode;
		m_NullHeadNode.m_pParent = &m_NullHeadNode;
		m_NullHeadNode.m_pNext = &m_NullHeadNode;
		m_NullHeadNode.m_pPrev = &m_NullHeadNode;
		m_nSize = 0;
		return true;
	}
//protected:
	//TTreeNode *m_pRoot;
	CommonLib::simple_alloc_t m_simple_alloc;
	CommonLib::alloc_t *m_pAlloc;
	uint32 m_nSize;
	TComporator m_Cmp;
	bool m_bMulti;
	TTreeNode m_NullHeadNode;
 
};


#include "BaseRBTree.cpp"

#endif
}
#endif