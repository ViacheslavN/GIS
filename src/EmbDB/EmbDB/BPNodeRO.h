#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_NODE_RO_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_NODE_RO_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "Key.h"
#include "RBMap.h"
#include "IDBTransactions.h"
#include "BTBaseNode.h"
#include "BPCompressors.h"
#include "BPInnerNodeSimpleCompressor.h"
#include "BPLeafNodeSimpleCompressor.h"
#include "BTVector.h"
namespace embDB
{
	template<typename _TKey,  typename typename _TValue, typename typename _TLink, typename _TComp,
	class _Transaction, class _TInnerComp, class _TLeafComp>
	class BPTreeNodeRO 
	{
	public:
		typedef _TKey TKey;
		typedef _TLink TLink;
		typedef _TValue TValue;
		typedef _TComp  TComp;


		typedef  BNodeVectorRO<TKey, TLink, TComp> TInnerMemSet;
		typedef  BNodeVectorRO<TKey, TValue, TComp> TLeafMemSet;

		typedef typename TInnerMemSet::TVecObj TInnerObj;
		typedef typename TLeafMemSet::TVecObj TLeafObj;

		
		typedef _TInnerComp TInnerCompressor;
		typedef _TLeafComp TLeafCompressor;
		typedef	_Transaction  Transaction;


		BPTreeNodeRO( CommonLib::alloc_t *pAlloc, int64 nPageAddr, bool bMulti) 
			: m_pAlloc(pAlloc), m_nPageAddr(nPageAddr), m_nParent(-1)
			, m_bIsLeaf(false), m_bMulti(bMulti), m_pInnerCompressor(0), m_pLeafCompressor(0),
			m_innerMemset(pAlloc), m_leafMemset(pAlloc), m_nFlag(0)
		{}
		~BPTreeNodeRO()
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
			return LoadFromPage(pFilePage);
		}
		bool LoadFromPage(CFilePage* pFilePage)
		{
			CommonLib::FxMemoryReadStream stream;
			stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
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
			int32 nSearhIndex = m_innerMemset.search_or_less_index(key, nType);
			int32 nIndex = -1;
			if(nSearhIndex != -1)
			{
				TInnerObj& elem = m_innerMemset[nSearhIndex];
				nNext = elem.m_val;
			}
			else
				nNext = m_nLess;
				return nNext;
		}

		TLeafObj* findNode(const TKey& key)
		{
			assert(m_bIsLeaf);
			int32 nIndex = -1;
			nIndex = m_leafMemset.search_index(key);
			if(nIndex == -1)
				return NULL;
			return &m_leafMemset[nIndex];
		}

		int32 findIndex(const TKey& key)
		{
			assert(m_bIsLeaf);
			return m_leafMemset.search_index(key);
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
			/*switch(m_nCompID)
			{
			case SIMPLE_COMPRESSOR:
				m_pInnerCompressor = new BPInnerNodeSimpleCompressor<TKey, TLink, TComp>();
				break;
			}

			if(!m_pInnerCompressor)
				return false;*/

			//m_innerMemset.setMulti(m_bMulti);
			m_pInnerCompressor = new TInnerCompressor();
			return m_pInnerCompressor->Load(m_innerMemset, stream);
		}
		bool initLeafNode(CommonLib::FxMemoryReadStream& stream)
		{
			/*switch(m_nCompID)
			{
			case SIMPLE_COMPRESSOR:
				m_pLeafCompressor = new BPLeafNodeSimpleCompressor<TKey, TLink, TComp>();
				break;
			}

			if(!m_pLeafCompressor)
				return false;

			//m_leafMemset.setMulti(m_bMulti);*/
			m_pLeafCompressor = new TLeafCompressor();
			return m_pLeafCompressor->Load(m_leafMemset, stream);
		}
		const TLeafObj* getLeafNode(size_t nIndex)
		{
			assert(m_bIsLeaf);
			if(nIndex< m_leafMemset.size())
				return m_leafMemset[nIndex];
			return NULL;
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
		// short m_nCompID;
		 //inner node
		 TLink m_nLess;
		 TInnerMemSet m_innerMemset;
		 TInnerCompressor *m_pInnerCompressor;
		 //leaf node
		 TLink m_nPrev;
		 TLink m_nNext;
		 TLeafMemSet m_leafMemset;
		 TLeafCompressor*m_pLeafCompressor;
		 int m_nFlag;
	};

}
#endif