#ifndef _EMBEDDED_DATABASE_RB_TREE_
#define _EMBEDDED_DATABASE_RB_TREE_
#include "CommonLibrary/general.h"
#include "CommonLibrary/alloc_t.h"
#include "RBTreeIterator.h"
namespace embDB
{

template <class TypeKey, class TypeVal>
class TRBTree
{
 public:
   typedef RedBlackTreeNode<TypeKey, TypeVal> RBTreeNode;
   typedef RBTreeIterator<TypeKey, TypeVal> iterator;

  TRBTree(CommonLib::alloc_t *alloc, bool bMuitiset = true);
  ~TRBTree();//{}
  RBTreeNode* GetRoot();
  void insert(const TypeKey& key, const TypeVal& val);
  void remove(const TypeKey& key);
  iterator remove(const iterator& it);

  iterator find(TypeKey key);
  iterator begin();
  iterator last();
  RBTreeNode* findNode(TypeKey key);
  size_t size() const;
protected:
  void DeleteNode(RBTreeNode *pNodeLeft);	
  void rotateLeft(RBTreeNode *pNodeLeft);
  void rotateRight(RBTreeNode *pNodeRight);
  void insertFixup(RBTreeNode *x);


  RBTreeNode* tree_successor(RBTreeNode *x);
  RBTreeNode* tree_minimum(RBTreeNode *x);
  RBTreeNode* tree_maximim(RBTreeNode *x);
  void deleteFixup(RBTreeNode *x, RBTreeNode *x_par);
protected:
  void DestroyTree();
  friend class iterator;
	CommonLib::alloc_t *pAlloc_;
	RBTreeNode *pRoot_;
	size_t tree_size_;
	//bool bMuitiset_;
};


 #include "RBTree.cpp"


}

#endif //_EMBEDDED_DATABASE_RB_TREE_