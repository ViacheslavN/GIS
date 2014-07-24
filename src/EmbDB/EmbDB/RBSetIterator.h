#ifndef _EMBEDDED_DATABASE_RB_SET_ITERATOR_H_
#define _EMBEDDED_DATABASE_RB_SET_ITERATOR_H_
#include "BaseRBTree.h"

namespace embDB
{

	template <class TypeKey>
	struct RBSetNode
	{
		RBSetNode(const TypeKey& key): 
			left_(0)
			,right_(0)
			,parent_(0)
			,prev_(0)
			,next_(0)
			,color_(RED)
			,key_(key)
	{}
			RBSetNode* left_;
			RBSetNode* right_;
			RBSetNode* parent_;
			RBSetNode* prev_;
			RBSetNode* next_;
			COLOR  color_;
			TypeKey key_;
	};
 
	template <class TypeKey>
	class RBSetIterator : public RBTreeBaseIterator< RBSetNode <TypeKey> >
	{
	 public:
		 typedef RBTreeBaseIterator<RBSetNode<TypeKey>> _Mybase;
		 typedef typename  _Mybase::TTreeNode TTreeNode;

		 RBSetIterator(int64 nParentTreeID, TTreeNode* pNode, bool bBegin) : _Mybase(nParentTreeID, pNode, bBegin)
		 { }
		 RBSetIterator(const RBSetIterator& it) : _Mybase(it)
		 { }
		 RBSetIterator(int64 nParentTreeID, TTreeNode* pNode): _Mybase(nParentTreeID, pNode)
		 { }
		 const TypeKey& getKey() const{
			 return m_pCurrNode->key_;
		 }

	};
}
#endif