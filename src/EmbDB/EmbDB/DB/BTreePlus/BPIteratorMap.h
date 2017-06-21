#pragma once
#include "BPIteratorSet.h"

namespace embDB
{


	template <class _TKey,  class _TValue, class _TLeafNode, class _TBTreeNode, class _TBTree>
	class TBPMapIterator  : public TBPSetIterator<_TKey, _TLeafNode, _TBTreeNode, _TBTree>
	{
	public:
		
		typedef TBPSetIterator<_TKey, _TLeafNode, _TBTreeNode, _TBTree> TBase;
		
		typedef _TValue		TValue;
	 

		TBPMapIterator(TBTree *pTree, TBTreeNodePtr& pCurNode, int32 nIndex) :
			TBase(pTree, pCurNode, nIndex)
		{
		}

		TBPMapIterator()  
		{

		}	 

		const TValue& value() const
		{
			return this->m_pCurLeafNode->value((uint32)m_nIndex);
		}
		TValue& value()
		{
			return this->m_pCurLeafNode->value((uint32)m_nIndex);
		}
 

 
	};
}
