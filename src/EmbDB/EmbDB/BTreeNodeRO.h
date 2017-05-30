#ifndef _EMBEDDED_DATABASE_B_TREE_NODE_READ_ONLY_H_
#define _EMBEDDED_DATABASE_B_TREE_NODE_READ_ONLY_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "storage/FilePage.h"
#include "Compressors.h"
#include "BTVector.h"
#include "SimpleCompessor.h"
#include "embDBInternal.h"
#include "DBConfig.h"
namespace embDB
{

	//_TAggrKey - агрегатный ключ, данные вместе с ключем храняться
	//Нода для уникальных значений


	template <class _TNodeElem, class _TComp >
	class BTreeNodeRO
	{
	public:

		BTreeNodeRO(CommonLib::alloc_t *pAlloc, int64 nPageAddr, uint16 nCompID): 
		  m_memset(pAlloc)
			  ,m_nPageAddr(nPageAddr)
			  ,m_nLess(-1)
			  ,m_nParent(-1)
			  ,m_pCompressor(0)
			  ,m_pAlloc(pAlloc)
			  ,m_nFlag(0)

		  {
			  switch(nCompID)
			  {
			  case SIMPLE_COMPRESSOR:
				  m_pCompressor = new SimpleNodeCompressor<TNodeElem>(pAlloc);
				  break;
			  }
		  }
		~BTreeNodeRO()
		{
			if(m_pCompressor)
				delete m_pCompressor;
		}
		typedef _TNodeElem       TNodeElem;
		typedef _TComp   TComp;
		typedef BNodeVectorRO<TNodeElem, TComp>   TMemSet;
		typedef NodeCompressor<TNodeElem> TCompressor;

		bool Load(IDBTransaction* pTransactions)
		{
			assert(m_nPageAddr != -1);
			CFilePage* pFilePage =  pTransactions->getFilePage(m_nPageAddr);
			assert(pFilePage);
			if(!pFilePage)
				return false; 
			return LoadFromPage(pFilePage);
		}
		bool LoadFromPage(CFilePage* pFilePage)
		{
			CommonLib::FxMemoryReadStream stream;
			stream.attachBuffer(pFilePage->getRowData(), pFilePage->getPageSize());
			m_nLess  = stream.readInt64();
			m_nParent  = stream.readInt64();

			return m_pCompressor->LoadNode(m_memset, stream);
		}
		TNodeElem* find(const TNodeElem& obj)
		{
			return m_memset.search(obj);
		}
		void findKey(const TNodeElem& obj, int64& nNextNode, int32& nIndex)
		{
			short nType = 0;
			int32 nSearhIndex = m_memset.search_or_less_index(obj, nType);
			nIndex = -1;
			if(nType == FIND_KEY)
			{
				nIndex = nSearhIndex;
			}
			else
			{
				if(nSearhIndex != -1)
				{
					TNodeElem& elem = m_memset[nSearhIndex];
					nNextNode = elem.m_nLink;
				}
				else
					nNextNode = m_nLess;

			}
		}
		void setFlags(int nFlag, bool bSet)
		{
			if(bSet)
				m_nFlag |= nFlag;
			else
				m_nFlag &= ~nFlag;
		}
		TNodeElem* getElement(uint32 nIndex)
		{
			assert(m_memset.size() > nIndex);
			return &m_memset[nIndex];
		}
		bool IsFree(){return !(m_nFlag & (ROOT_NODE));}
		int getFlags(){return m_nFlag;}
		int64 m_nPageAddr;
		int64 m_nLess;
		int64 m_nParent;
		TMemSet m_memset;
		TCompressor* m_pCompressor;
		int m_nFlag;
		CommonLib::alloc_t* m_pAlloc;

	};

}
#endif