#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE__MAP_NODE_RO_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE__MAP_NODE_RO_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "Key.h"
#include "RBMap.h"
#include "embDBInternal.h"
#include "BTBaseNode.h"
#include "BPCompressors.h"
#include "BPInnerNodeSimpleCompressor.h"
#include "BPLeafNodeSimpleCompressor.h"
#include "simple_vector.h"
#include "BPSetNodeRO.h"
namespace embDB
{
	template<typename _TKey,  typename typename _TValue, typename typename _TLink, typename _TComp,
	class _Transaction, class _TInnerComp, class _TLeafComp>
	class BPTreeMapNodeRO : public BPTreeSetNodeRO<_TKey, _TLink, _TComp, _Transaction, _TInnerComp, _TLeafComp>
	{
	public:
		typedef _TKey TKey;
		typedef _TLink TLink;
		typedef _Transaction Transaction;
		typedef _TValue TValue;
		typedef _TComp  TComp;
		typedef  TVectorRO<TKey> TKeyMemSet;
		typedef  TVectorRO<TLink> TLinkMemSet;
		typedef  TVectorRO<TValue> TValueMemSet;

		typedef _TInnerComp TInnerCompressor;
		typedef _TLeafComp TLeafCompressor;
		typedef BPTreeSetNodeRO<_TKey, _TLink, _TComp, _Transaction, _TInnerComp, _TLeafComp> TBase;

		BPTreeMapNodeRO( CommonLib::alloc_t *pAlloc, int64 nPageAddr, bool bMulti, bool bCheckCRC32) 
			: TBase(pAlloc, nPageAddr, bMulti, bCheckCRC32), m_leafValueMemSet(pAlloc)
		{}
		~BPTreeMapNodeRO()
		{
		
		}
		bool LoadFromPage(CFilePage* pFilePage, Transaction *pTransaction)
		{
			CommonLib::FxMemoryReadStream stream;
			stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
			sFilePageHeader header;
			header.read(stream);
			if(m_bCheckCRC32 && !pFilePage->isCheck() && !header.isValid())
			{
				pTransaction->error(_T("BTREE: Page %I64d Error CRC for node page"), pFilePage->getAddr());
				return false;
			}
			m_bIsLeaf = stream.readBool();
			//m_bMulti = stream.readBool();

			if(m_bIsLeaf)
			{
				m_nNext  = stream.readInt64();
				m_nPrev  = stream.readInt64(); 
				return initLeafNode(stream);
			}
			else
			{
				m_nLess  = stream.readInt64();
				return initInnerNode(stream);
			}

		}
		bool initLeafNode(CommonLib::FxMemoryReadStream& stream)
		{
			m_pLeafCompressor = new TLeafCompressor();
			return m_pLeafCompressor->Load(m_leafKeyMemSet, m_leafValueMemSet, stream);
		}
	public:
		TValueMemSet m_leafValueMemSet;
	};

}
#endif