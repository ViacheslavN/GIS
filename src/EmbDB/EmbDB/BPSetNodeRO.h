#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE__SET_NODE_RO_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE__SET_NODE_RO_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "Key.h"
#include "RBMap.h"
#include "IDBTransactions.h"
#include "BTBaseNode.h"
#include "BPCompressors.h"
#include "BPInnerNodeSimpleCompressor.h"
#include "BPLeafNodeSimpleCompressor.h"
#include "simple_vector.h"
namespace embDB
{
	template<typename _TKey, /* typename typename _TValue,*/ typename typename _TLink, typename _TComp,
	class _Transaction, class _TInnerComp, class _TLeafComp>
	class BPTreeSetNodeRO 
	{
	public:
		typedef _TKey TKey;
		typedef _TLink TLink;
		typedef _Transaction Transaction;
		//typedef _TValue TValue;
		typedef _TComp  TComp;
		typedef  TVectorRO<TKey> TKeyMemSet;
		typedef  TVectorRO<TLink> TLinkMemSet;
		//typedef  TVectorRO<TValue> TValueMemSet;

		typedef _TInnerComp TInnerCompressor;
		typedef _TLeafComp TLeafCompressor;

		BPTreeSetNodeRO( CommonLib::alloc_t *pAlloc, int64 nPageAddr, bool bMulti, bool bCheckCRC32) 
			: m_pAlloc(pAlloc), m_nPageAddr(nPageAddr), m_nParent(-1)
			, m_bIsLeaf(false), m_pInnerCompressor(0), m_pLeafCompressor(0),
			m_innerKeyMemSet(pAlloc), m_innerLinkMemSet(pAlloc), m_leafKeyMemSet(pAlloc), m_nFlag(0),
			m_bMulti(bMulti), m_bCheckCRC32(bCheckCRC32)
		{}
		~BPTreeSetNodeRO()
		{
			if(m_pLeafCompressor)
				delete m_pLeafCompressor;
			if(m_pInnerCompressor)
				delete m_pInnerCompressor;
		}


		virtual bool isLeaf() const {return m_bIsLeaf;}
		virtual bool IsFree() const 
		{
			return !(m_nFlag & (ROOT_NODE | BUSY_NODE));
		}

		bool Load(Transaction* pTransactions)
		{
			assert(m_nPageAddr != -1);

			if(m_nPageAddr == -1)
				return false;
			CFilePage* pFilePage =  pTransactions->getFilePage(m_nPageAddr);
			if(!pFilePage)
				return false; 
			return LoadFromPage(pFilePage, pTransactions);
		}
		bool LoadFromPage(CFilePage* pFilePage, Transaction* pTransactions)
		{
			CommonLib::FxMemoryReadStream stream;
			stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
			sFilePageHeader header;
			header.read(stream, m_bCheckCRC32 && !pFilePage->isCheck());
			if(m_bCheckCRC32 && !pFilePage->isCheck() && !header.isValid())
			{
				CommonLib::CString sMsg;
				sMsg.format(_T("BTREE: Page %I64d Error CRC for node page"), pFilePage->getAddr());
				pTransactions->error(sMsg);
				return false;
			}
			pFilePage->setCheck(true);
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
		TLink findNext(const TKey& key)
		{
			assert(!m_bIsLeaf);
			short nType = 0;
			TLink nNext = -1;
			int32 nSearhIndex = -1;
			if(m_bMulti)
				nSearhIndex = m_innerKeyMemSet.binary_search_or_less_index_multi(key, nType, m_comp);
			else
				nSearhIndex = m_innerKeyMemSet.binary_search_or_less_index(key, nType, m_comp);
			int32 nIndex = -1;
			if(nSearhIndex != -1)
			{
				nNext = m_innerLinkMemSet[nSearhIndex];
			}
			else
				nNext = m_nLess;
				return nNext;
		}

		int32 findIndex(const TKey& key)
		{
			assert(m_bIsLeaf);
			if(m_bMulti)
				return m_leafKeyMemSet.binary_search_multi(key, m_comp);
			else
				return m_leafKeyMemSet.binary_search(key, m_comp);
		}

		int getFlags() const
		{
			return m_nFlag;
		}
		void setFlags(int nFlag, bool bSet) 
		{
			if(bSet)
				m_nFlag |= nFlag;
			else
				m_nFlag &= ~nFlag;
		}
		bool initInnerNode(CommonLib::FxMemoryReadStream& stream)
		{

			m_pInnerCompressor = new TInnerCompressor();
			return m_pInnerCompressor->Load(m_innerKeyMemSet, m_innerLinkMemSet, stream);
		}
		bool initLeafNode(CommonLib::FxMemoryReadStream& stream)
		{
			m_pLeafCompressor = new TLeafCompressor();
			return m_pLeafCompressor->Load(m_leafKeyMemSet, stream);
		}
		

		TLink less()
		{
			assert(!m_bIsLeaf);
			return m_nLess;
		}

		TLink prev()
		{
			assert(m_bIsLeaf);
			return m_nPrev;
		}
		TLink next()
		{
			assert(m_bIsLeaf);
			return m_nNext;
		}
		
		
	public:
		 TLink m_nPageAddr;
		 TLink m_nParent;
		 bool m_bIsLeaf;
		 bool m_bMulti;
		 CommonLib::alloc_t *m_pAlloc;
		 TComp m_comp;
		// short m_nCompID;
		 //inner node
		 TLink m_nLess;
		 TKeyMemSet m_innerKeyMemSet;
		 TLinkMemSet m_innerLinkMemSet;
		 TInnerCompressor *m_pInnerCompressor;

		 //leaf node
		 TLink m_nPrev;
		 TLink m_nNext;
	     TKeyMemSet m_leafKeyMemSet;
		// TValueMemSet m_leafValueMemSet;
		 TLeafCompressor*m_pLeafCompressor;
		 int m_nFlag;
		 bool m_bCheckCRC32;
	};

}
#endif