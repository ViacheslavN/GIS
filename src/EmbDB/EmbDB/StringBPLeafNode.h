#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_STRING_LEAF_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_STRING_LEAF_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "BPTreeLeafNodeMapv2.h"
#include "StringVal.h"
#include "LeafStringCompress.h"
#include "PageAlloc.h"
namespace embDB
{
	template<typename _TKey, typename _Transaction>
	class TStringLeafNode : public  BPTreeLeafNodeMapv2<_TKey, sStringVal, _Transaction, BPStringLeafNodeCompressor<_TKey, _Transaction> >
	{
	public:
		typedef   BPTreeLeafNodeMapv2<_TKey, sStringVal, _Transaction, BPStringLeafNodeCompressor<_TKey,  _Transaction> > TBase;
		typedef sStringVal TValue;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::Transaction Transaction;
		typedef typename TBase::TCompressor TCompressor;
		typedef typename TBase::TLeafMemSet TLeafMemSet;
		typedef TBPVector<sStringVal>		TValueMemSet;
		typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;
		TStringLeafNode( CommonLib::alloc_t *pAlloc, bool bMulti) :
		TBase(pAlloc, bMulti), m_pPageAlloc(NULL)
		{

		}
		~TStringLeafNode()
		{
			if(m_pCompressor)
				m_pCompressor->Clear();
		}


		virtual bool init(TLeafCompressorParams *pParams , Transaction* pTransaction)
		{
			assert(!m_pCompressor);
			m_pCompressor = new TCompressor(pTransaction, (CommonLib::alloc_t*)m_pPageAlloc, pParams, &m_leafKeyMemSet, &m_leafValueMemSet);
			return true;
		}

		bool insertString(const TKey& key, const CommonLib::CString& sString)
		{

		}
		void SetPageAlloc(CPageAlloc *pPageAlloc)
		{
			assert(pPageAlloc != NULL);
			assert(m_pCompressor == NULL);
			m_pPageAlloc = pPageAlloc;
		}
	public:
		CPageAlloc *m_pPageAlloc;


	};

}
#endif