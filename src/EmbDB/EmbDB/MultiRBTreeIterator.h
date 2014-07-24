#ifndef _EMBEDDED_DATABASE_MULTI_RB_TREE_ITERATOR_
#define _EMBEDDED_DATABASE_MULTI_RB_TREE_ITERATOR_

 
namespace embDB
{
#define RED  true
#define BLACK false
typedef  bool COLOR;



template <class TypeKey>
struct RedBlackTreeNode;


template <class TypeKey>
struct SimpleNode
{
	TypeKey key_;
}

template <class TypeKey>
struct RedBlackTreeNode
	{
	  RedBlackTreeNode(TypeKey key): 
		key_(key)
		,left_(0)
		,right_(0)
		,parent_(0)
		,prev_(0)
		,next_(0)
		,color_(RED)
		,listNext_(0)
		,listPrev_(0)
		,size_(1)
	  {}
	  RedBlackTreeNode* left_;
	  RedBlackTreeNode* right_;
	  RedBlackTreeNode* parent_;
	  RedBlackTreeNode* prev_;
	  RedBlackTreeNode* next_;
	  COLOR  color_;
	  TypeKey key_;

	  RedBlackTreeNode* listNext_;
	  RedBlackTreeNode* listPrev_;
	  size_t size_;

 
	};


}
#endif //_EMBEDDED_DATABASE_MULTI_RB_TREE_ITERATOR_