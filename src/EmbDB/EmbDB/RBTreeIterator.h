#ifndef _EMBEDDED_DATABASE_RB_TREE_ITERATOR_
#define _EMBEDDED_DATABASE_RB_TREE_ITERATOR_

 
namespace embDB
{
#define RED  true
#define BLACK false
typedef  bool COLOR;


template <class TypeKey, class TypeVal>
class TRBTree;


template <class TypeKey, class TypeVal>
struct RedBlackTreeNode
	{
	  RedBlackTreeNode(TypeKey key, TypeVal value): 
		key_(key)
		,value_(value)
		,left_(0)
		,right_(0)
		,parent_(0)
		,prev_(0)
		,next_(0)
		,color_(RED)
	  {}
	  RedBlackTreeNode* left_;
	  RedBlackTreeNode* right_;
	  RedBlackTreeNode* parent_;
	  RedBlackTreeNode* prev_;
	  RedBlackTreeNode* next_;
	  COLOR  color_;
	  TypeKey key_;
	  TypeVal value_;
	};


template <class TypeKey, class TypeVal>
class RBTreeIterator
{
  public:
	typedef RedBlackTreeNode<TypeKey, TypeVal> RBTreeNode;
	RBTreeIterator(TRBTree<TypeKey, TypeVal>* tree, RBTreeNode* node, CommonLib::alloc_t *pAlloc, bool bBegin = true);

	RBTreeIterator(const RBTreeIterator& it);
	RBTreeIterator& operator = (const RBTreeIterator& it);

	  ~RBTreeIterator();
    bool Reset(bool bBegin = true);
    RBTreeNode* GetCurNode();
    TypeVal& GetValue();
    TypeKey& GetKey();

    bool next();
	bool back();
    bool IsEnd();
	RBTreeIterator& operator ++ ();
	RBTreeIterator& operator -- ();
	bool operator == (const RBTreeIterator& it);
  private:

    RBTreeNode* rbNode_;
    RBTreeNode* rbCurrNode_;
   	CommonLib::alloc_t *pAlloc_;
    TRBTree<TypeKey, TypeVal>* tree_;
	friend 	class TRBTree<TypeKey, TypeVal>;


};

#include "RBTreeIterator.cpp"

}

#endif