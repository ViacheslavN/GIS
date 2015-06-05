#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_LEAF_NODE_SET_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_LEAF_NODE_SET_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "Key.h"
#include "RBMap.h"
#include "IDBTransactions.h"
#include "BTBaseNode.h"
#include "CompressorParams.h"
namespace embDB
{
	template<typename _TKey,  /*typename _TComp,*/
	class _Transaction, class _TCompressor>
	class BPTreeLeafNodeSetv2Base : public  BPBaseTreeNode
	{
	public:

		typedef int64 TLink;
		typedef _TKey TKey;
		typedef _Transaction Transaction;
		//typedef _TComp		 TComporator;
		typedef _TCompressor TCompressor;
		typedef  TBPVector<TKey> TLeafMemSet;

		typedef typename _TCompressor::TLeafCompressorParamsBase TLeafCompressorParamsBase;

		BPTreeLeafNodeSetv2Base( CommonLib::alloc_t *pAlloc, bool bMulti) :
		m_leafKeyMemSet(pAlloc),  m_pCompressor(0),	m_nNext(-1), m_nPrev(-1), m_bMulti(bMulti),
			m_pAlloc(pAlloc)

		{
		
		}
		~BPTreeLeafNodeSetv2Base()
		{
			if(m_pCompressor)
				delete m_pCompressor;
		}

		virtual bool init(TLeafCompressorParamsBase *pParams = NULL)
		{
			assert(!m_pCompressor);
			m_pCompressor = new TCompressor(m_pAlloc, pParams);
			return true;
		}

		virtual bool isLeaf() const {return true;}
		virtual size_t size() const
		{
			assert(m_pCompressor);
			return  2 * sizeof(TLink) +  m_pCompressor->size();
		}

		virtual size_t headSize() const
		{
			assert(m_pCompressor);
			return 2 * sizeof(TLink) +  m_pCompressor->headSize(); 
		}
		virtual size_t rowSize() const
		{
			assert(m_pCompressor);
			return m_pCompressor->rowSize();
		}
		virtual bool IsFree() const 
		{
			return !(m_nFlag & (ROOT_NODE | BUSY_NODE));
		}
	
		template<class TComp>
		bool insertImp(TComp& comp, const TKey& key, int32& nIndex, int nInsertLeafIndex = -1)
		{
			nIndex = -1;
			short nType = 0;

			if(m_leafKeyMemSet.empty())
			{
				m_leafKeyMemSet.push_back(key);
				nIndex = 0;
			}
			else
			{
				if(nInsertLeafIndex != -1)
					nIndex = nInsertLeafIndex;
				else
				{
					if(m_bMulti)
						nIndex = m_leafKeyMemSet.upper_bound(key, comp);
					else
					{
						nIndex = m_leafKeyMemSet.lower_bound(key, nType, comp);
						if(nType == FIND_KEY)
						{
							//TO DO logs
							return false;
						}
					}
				}
			

				m_leafKeyMemSet.insert(key, nIndex);
			}
			return true;
		}
		template<class TComp>
		int insert(TComp& comp, const TKey& key, int nInsertLeafIndex = -1)
		{
			int32 nIndex = 0;
			if(!insertImp(comp, key, nIndex, nInsertLeafIndex))
				return -1;
			 if(!m_pCompressor->insert(key))
				 return -1;
			 return nIndex;
		}

	
		template<class TComp>
		uint32 upper_bound(TComp& comp, const TKey& key)
		{

			int32 nIndex =  m_leafKeyMemSet.upper_bound(key, comp);
			if(nIndex != 0)
			{
				nIndex--;
			}

			return nIndex;
		}
		template<class TComp>
		uint32 lower_bound(TComp& comp, const TKey& key, short& nType)
		{
			int32 nIndex =   m_leafKeyMemSet.lower_bound(key, nType, comp);
			if(nIndex == m_leafKeyMemSet.size())
			{
				return -1;
			}

			return nIndex;
		}
		template<class TComp>
		uint32 binary_search(TComp& comp, const TKey& key)
		{
			return m_leafKeyMemSet.binary_search(key, comp);
		}

		bool update(const TKey& key)
		{
			return true;
		}
		template<class TComp>
		bool remove(TComp& comp, const TKey& key)
		{
			uint32 nIndex = -1;
			short nType = 0;
			if(m_bMulti)
			{
				nIndex = m_leafKeyMemSet.upper_bound(key,  comp);
				if(nIndex && m_comp.EQ(key, m_leafKeyMemSet[nIndex - 1]))
				{
					nType = FIND_KEY;
					nIndex -= 1;
				}
			}
			else
				nIndex = m_leafKeyMemSet.lower_bound(key, nType, comp);
					
			if(nType != FIND_KEY)
			{
				return false;
			}

			removeByIndex(nIndex);
			return true;
		}
		bool removeByIndex(int32 nIndex)
		{
			m_pCompressor->remove(m_leafKeyMemSet[nIndex]);
			bool bRet = m_leafKeyMemSet.remove(nIndex);
			if(!bRet)
			{
				//TO DO Logs
				return false;
			}
			return true;
		}
		template<class TVector, class TVecVal>
		int SplitInVec(TVector& src, TVector& dst, TVecVal* pSplitVal)
		{
			size_t nSize = src.size()/2;
			dst.copy(src, 0, nSize, src.size());
			src.resize(nSize);
			if(pSplitVal)
				*pSplitVal = dst[0];
			return (int)nSize;

		}
		int  SplitIn(BPTreeLeafNodeSetv2Base *pNode, TKey* pSplitKey)
		{
	
			TLeafMemSet& newNodeMemSet = pNode->m_leafKeyMemSet;
			TCompressor* pNewNodeComp = pNode->m_pCompressor;

			int nSplitIndex = SplitInVec(m_leafKeyMemSet, newNodeMemSet, pSplitKey);
			/*size_t nSize = m_leafKeyMemSet.size()/2;

			newNodeMemSet.copy(m_leafKeyMemSet, 0, nSize, m_leafKeyMemSet.size());
			m_leafKeyMemSet.resize(nSize);
			*pSplitKey = newNodeMemSet[0];*/

			m_pCompressor->recalc(m_leafKeyMemSet);
			pNewNodeComp->recalc(newNodeMemSet);
			return nSplitIndex;
		}
		size_t count() const 
		{
			return m_leafKeyMemSet.size();
		}
		size_t tupleSize() const
		{
			return m_pCompressor->tupleSize();
		}
		const TKey& key(uint32 nIndex) const
		{
			return m_leafKeyMemSet[nIndex];
		}
		TKey& key(uint32 nIndex)
		{
			return m_leafKeyMemSet[nIndex];
		}

		template<class TVector>
		bool UnionVec(TVector& dstVec, TVector& srcVec, bool bLeft, int *nCheckIndex = 0 )
		{
			if(bLeft)
			{
				if(nCheckIndex)
					*nCheckIndex += srcVec.size();
				srcVec.push_back(dstVec);
				srcVec.swap(dstVec);
			}
			else
			{
				dstVec.push_back(srcVec);
			}
			return true;
		}


		bool UnionWith(BPTreeLeafNodeSetv2Base* pNode, bool bLeft, 
			int *nCheckIndex = 0)
		{
			m_pCompressor->add(pNode->m_leafKeyMemSet);
			UnionVec(m_leafKeyMemSet, pNode->m_leafKeyMemSet, bLeft, nCheckIndex);
		/*	m_pCompressor->add(pNode->m_leafKeyMemSet);
			if(bLeft)
			{
				pNode->m_leafKeyMemSet.push_back(m_leafKeyMemSet);
				pNode->m_leafKeyMemSet.swap(m_leafKeyMemSet);
			}
			else
			{
				m_leafKeyMemSet.push_back(pNode->m_leafKeyMemSet);
			}*/
			return true;
		}


		template<class TVector>
		bool AlignmentOfVec(TVector& dstVec, TVector& srcVec, bool bFromLeft, int *nCheckIndex = 0  )
		{

			int nCnt = ((dstVec.size() + srcVec.size()))/2 - dstVec.size();
			if(nCnt <= 0)
				return false; //оставим все при своих
			if(bFromLeft)
			{
				if(nCheckIndex)
					*nCheckIndex += nCnt;

				size_t newSize = srcVec.size() - nCnt;
				dstVec.insert(srcVec, 0, newSize, srcVec.size());
				srcVec.resize(newSize);
			}
			else
			{
				dstVec.copy(srcVec, dstVec.size(), 0, nCnt);
				srcVec.movel(nCnt, nCnt);
			}
			return true;
		}

		bool AlignmentOf(BPTreeLeafNodeSetv2Base* pNode, bool bFromLeft)
		{
			TLeafMemSet& nodeMemset = pNode->m_leafKeyMemSet;
			TCompressor* pNodeComp = pNode->m_pCompressor;
			if(!AlignmentOfVec(m_leafKeyMemSet, nodeMemset, bFromLeft))
				return false;
			pNodeComp->recalc(nodeMemset);
			m_pCompressor->recalc(m_leafKeyMemSet);

			return true;
		}
		template<class TComp>
		bool isKey(TComp& comp, const TKey& key, uint32 nIndex)
		{
			return comp.EQ(key, m_leafKeyMemSet[nIndex]);
		}
		
	public:
		TCompressor * m_pCompressor;
		TLeafMemSet m_leafKeyMemSet;
		TLink m_nNext;
		TLink m_nPrev;
		bool m_bMulti;
		CommonLib::alloc_t *m_pAlloc;
		//TComporator m_comp;
	};

	template<typename _TKey, /* typename _TComp,*/
	class _Transaction, class _TCompressor>
	class BPTreeLeafNodeSetv2: public  BPTreeLeafNodeSetv2Base <_TKey, /*_TComp,*/ _Transaction, _TCompressor>
	{
	public:
		
		typedef BPTreeLeafNodeSetv2Base <_TKey, /*_TComp,*/ _Transaction, _TCompressor> TBase;
		typedef int64 TLink;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::Transaction Transaction;
		//typedef typename TBase::TComporator	 TComporator;
		typedef typename TBase::TCompressor TCompressor;
		typedef typename TBase::TLeafMemSet TLeafMemSet;


		BPTreeLeafNodeSetv2( CommonLib::alloc_t *pAlloc, bool bMulti) : TBase(pAlloc, bMulti)
		{}
	

		virtual  bool Save(	CommonLib::FxMemoryWriteStream& stream) 
		{
			stream.write(m_nNext);
			stream.write(m_nPrev);
			return m_pCompressor->Write(m_leafKeyMemSet, stream);
		}
		virtual bool Load(CommonLib::FxMemoryReadStream& stream)
		{
			stream.read(m_nNext);
			stream.read(m_nPrev); 
			return m_pCompressor->Load(m_leafKeyMemSet, stream);
		}

	};
	
}
#endif