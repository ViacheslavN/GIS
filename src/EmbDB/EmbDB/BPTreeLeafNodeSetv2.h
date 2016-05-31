#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_LEAF_NODE_SET_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_LEAF_NODE_SET_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "Key.h"
//#include "RBMap.h"
#include "embDBInternal.h"
#include "BTBaseNode.h"
#include "CompressorParams.h"
namespace embDB
{
	template<typename _TKey,
	class _Transaction, class _TCompressor,
	class _TLeafMemSet = TBPVector<_TKey> >
	class BPTreeLeafNodeSetv2Base : public  BPBaseTreeNode
	{
	public:

		typedef int64 TLink;
		typedef _TKey TKey;
		typedef _Transaction Transaction;
		typedef _TCompressor TCompressor;
		typedef _TLeafMemSet TLeafMemSet;

		typedef typename _TCompressor::TLeafCompressorParams TLeafCompressorParams;

		BPTreeLeafNodeSetv2Base(CommonLib::alloc_t *pAlloc, bool bMulti, uint32 nPageSize) :
		m_leafKeyMemSet(pAlloc),  m_pCompressor(0),	m_nNext(-1), m_nPrev(-1), m_bMulti(bMulti),
			m_pAlloc(pAlloc), m_bMinSplit(false), m_nPageSize(nPageSize)

		{
		
		}
		~BPTreeLeafNodeSetv2Base()
		{
			if(m_pCompressor)
				delete m_pCompressor;
		}

	

		virtual bool isLeaf() const {return true;}
		virtual uint32 size() const
		{
			assert(m_pCompressor);
			return  2 * sizeof(TLink) +  m_pCompressor->size();
		}
		virtual bool isNeedSplit() const
		{
			assert(m_pCompressor);
			return m_pCompressor->isNeedSplit(/*nPageSize - 2 *sizeof(TLink)*/);
		}
		virtual uint32 headSize() const
		{
			assert(m_pCompressor);
			return 2 * sizeof(TLink) +  m_pCompressor->headSize(); 
		}
		virtual uint32 rowSize() const
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
			 if(!m_pCompressor->insert(nIndex, key))
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
				if(nIndex && comp.EQ(key, m_leafKeyMemSet[nIndex - 1]))
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
			m_pCompressor->remove(nIndex, m_leafKeyMemSet[nIndex]);
			bool bRet = m_leafKeyMemSet.remove(nIndex);
			if(!bRet)
			{
				//TO DO Logs
				return false;
			}
			return true;
		}
		template<class TVector, class TVecVal>
		int SplitInVec(TVector& src, TVector& dst, TVecVal* pSplitVal, int32 _nBegin = -1, int32 _nEnd = -1)
		{
			uint32 nBegin = _nBegin == -1 ? src.size()/2 : _nBegin;
			uint32 nEnd = _nEnd == -1 ? src.size() : _nEnd;
			dst.copy(src, 0, nBegin, nEnd);
			src.resize(src.size() - (nEnd - nBegin));
			if(pSplitVal)
				*pSplitVal = dst[0];
			return (int)nBegin;

		}


		template<class TVector>
		void SplitInVec(TVector& src, TVector& dst, uint32 nBegin, uint32 nCount)
		{		 
			dst.copy(src, 0, nBegin, nCount);
		}

		template<class TVector, class TVecVal>
		int SplitOne(TVector& src, TVector& dst, TVecVal* pSplitVal)
		{
			uint32 nSize = src.size() - 1;
			dst.push_back(src[nSize]);
			src.resize(nSize);
			if(pSplitVal)
				*pSplitVal = dst[0];
			return (int)nSize;

		}


		

		int  SplitIn(BPTreeLeafNodeSetv2Base *pNode, TKey* pSplitKey)
		{

			TLeafMemSet& newNodeMemSet = pNode->m_leafKeyMemSet;
			TCompressor* pNewNodeComp = pNode->m_pCompressor;

	
			if(m_bMinSplit)
			{
				m_pCompressor->remove(m_leafKeyMemSet.size() -1, m_leafKeyMemSet.back());
				int nSplitIndex = SplitOne(m_leafKeyMemSet, newNodeMemSet, pSplitKey);
				
				pNewNodeComp->insert(0, newNodeMemSet[0]);
				return nSplitIndex;
			}
			else
			{
				int nSplitIndex = SplitInVec(m_leafKeyMemSet, newNodeMemSet, pSplitKey);
				m_pCompressor->SplitIn(0, nSplitIndex, pNewNodeComp);
				return nSplitIndex;
			}


		}


		int  SplitIn(BPTreeLeafNodeSetv2Base *pLeftNode, BPTreeLeafNodeSetv2Base *pRightNode, TKey* pSplitKey)
		{

			TLeafMemSet& leftNodeMemSet = pLeftNode->m_leafKeyMemSet;
			TCompressor* pleftNodeComp = pLeftNode->m_pCompressor;

			TLeafMemSet& rightNodeMemSet = pRightNode->m_leafKeyMemSet;
			TCompressor* pRightNodeComp = pRightNode->m_pCompressor;


			uint32 nSize = m_leafKeyMemSet.size()/2;
			
			if(pSplitKey)
				*pSplitKey = m_leafKeyMemSet[nSize];

			SplitInVec(m_leafKeyMemSet, leftNodeMemSet, 0, nSize);
			SplitInVec(m_leafKeyMemSet, rightNodeMemSet, nSize, m_leafKeyMemSet.size());
		
			pleftNodeComp->recalc(leftNodeMemSet);
			pRightNodeComp->recalc(rightNodeMemSet);
			return nSize;
			


		}

		uint32 count() const 
		{
			return m_leafKeyMemSet.size();
		}
		uint32 tupleSize() const
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

				uint32 newSize = srcVec.size() - nCnt;
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
		
		void SetMinSplit(bool bMinSplit )
		{
			m_bMinSplit = bMinSplit;
		}
		virtual void clear()
		{
			m_leafKeyMemSet.clear();
			delete m_pCompressor;
			m_pCompressor = NULL;

		}
		bool IsHaveUnion(BPTreeLeafNodeSetv2Base *pNode)
		{

			return this->m_pCompressor->IsHaveUnion(pNode->m_pCompressor);
		}
		bool IsHaveAlignment(BPTreeLeafNodeSetv2Base *pNode)
		{
			return this->m_pCompressor->IsHaveAlignment(pNode->m_pCompressor);
		}
		bool isHalfEmpty() const
		{
			return this->m_pCompressor->isHalfEmpty();
		}
		virtual  void PreSave()
		{

		}

	public:
		TCompressor * m_pCompressor;
		TLeafMemSet m_leafKeyMemSet;
		TLink m_nNext;
		TLink m_nPrev;
		bool m_bMulti;
		CommonLib::alloc_t *m_pAlloc;
		bool m_bMinSplit;
		uint32 m_nPageSize;
		//TComporator m_comp;
	};

	template<typename _TKey, 
	class _Transaction, class _TCompressor>
	class BPTreeLeafNodeSetv2: public  BPTreeLeafNodeSetv2Base <_TKey,  _Transaction, _TCompressor>
	{
	public:
		
		typedef BPTreeLeafNodeSetv2Base <_TKey, _Transaction, _TCompressor> TBase;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::Transaction Transaction;
		

		typedef typename TBase::TCompressor TCompressor;
		typedef typename TBase::TLeafMemSet TLeafMemSet;
		typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;

		BPTreeLeafNodeSetv2( CommonLib::alloc_t *pAlloc, bool bMulti, uint32 nPageSize) : TBase(pAlloc, bMulti, nPageSize)
		{}


		bool init(TLeafCompressorParams *pParams = NULL, Transaction* pTransaction = NULL )
		{
			assert(!this->m_pCompressor);
			this->m_pCompressor = new TCompressor(this->m_nPageSize - 2* sizeof(TLink), pTransaction, this->m_pAlloc, pParams, &this->m_leafKeyMemSet);
			return true;
		}
	

		virtual  bool Save(	CommonLib::FxMemoryWriteStream& stream) 
		{
			stream.write(this->m_nNext);
			stream.write(this->m_nPrev);
			return this->m_pCompressor->Write(this->m_leafKeyMemSet, stream);
		}
		virtual bool Load(CommonLib::FxMemoryReadStream& stream)
		{
			stream.read(this->m_nNext);
			stream.read(this->m_nPrev); 
			return this->m_pCompressor->Load(this->m_leafKeyMemSet, stream);
		}


		bool IsHaveUnion(BPTreeLeafNodeSetv2 *pNode)
		{
			 
			return this->m_pCompressor->IsHaveUnion(pNode->m_pCompressor);
		}
		bool IsHaveAlignment(BPTreeLeafNodeSetv2 *pNode)
		{
			return this->m_pCompressor->IsHaveAlignment(pNode->m_pCompressor);
		}
	};
	
}
#endif