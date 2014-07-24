#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_LEAF_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_LEAF_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "RBMap.h"
#include "BPTreeLeafNodeSet.h"
#include "BPLeafNodeMapSimpleCompressor.h"

namespace embDB
{
	template<typename _TLink,  typename _TCompressor, class _TLeafMemSet>
	class BPTreeLeafNodeMap  : public  BPTreeLeafNodeSet<_TLink, _TCompressor, _TLeafMemSet>
	{
	public:

		typedef  BPTreeLeafNodeSet<_TLink, _TCompressor, _TLeafMemSet> TBase;
		typedef typename TBase::TLeafMemSet TLeafMemSet;
		typedef _TCompressor TCompressor;
		typedef typename TBase::TKey TKey;
		typedef typename TLeafMemSet::TValue TValue;
		typedef typename TBase::TLink TLink;
		typedef typename TLeafMemSet::TTreeNode TTreeNode;


		BPTreeLeafNodeMap( CommonLib::alloc_t *pAlloc, bool bMulti) : TBase(pAlloc, bMulti)
		{
		
		}
		~BPTreeLeafNodeMap()
		{
			
		}
		
		bool insert( const TKey& key, const TValue& val)
		{
			TTreeNode *pNode = m_leafMemSet.insert(key, val);
			bool bRet = false;
			if(!m_leafMemSet.isNull(pNode))
			{
				bRet = m_pCompressor->insert(pNode);
			}
			return bRet;
		}
		virtual bool update(const TKey& key, const TValue& newValue)
		{
			return true;
		}
	public:
		//TCompressor * m_pCompressor;
		//TLeafMemSet m_leafMemSet;
	};
	
}
#endif