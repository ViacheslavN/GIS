#pragma once
#include "BPIteratorSetV3.h"

namespace embDB
{


	template <class _TKey,  class _TValue, class _TLeafNode, class _TBTreeNode, class _TBTree>
	class TBPMapIteratorV3  : public TBPSetIteratorV3<_TValue, _TLeafNode, _TBTreeNode, _TBTree>
	{
	public:
		
		typedef TBPSetIteratorV3<_TValue, _TLeafNode, _TBTreeNode, _TBTree> TBase;
		
		typedef _TValue		TValue;
	 

		TBPMapIteratorV3(TBTree *pTree, TBTreeNodePtr& pCurNode, int32 nIndex) :
			TBase(pTree, pCurNode, nIndex)
		{
		}

		TBPMapIteratorV3()  
		{

		}	 

		const TValue& value() const
		{
			return this->m_pCurLeafNode->value((uint32)m_nIndex);
		}
		TKey& value()
		{
			return this->m_pCurLeafNode->value((uint32)m_nIndex);
		}
 

 
	};
}
