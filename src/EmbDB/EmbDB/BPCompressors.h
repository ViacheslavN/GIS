#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "RBSet.h"
#include "BTVector.h"
#include "Key.h"
namespace embDB
{
	enum eBPCompressorID
	{
		BP_SIMPLE_COMPRESSOR = 1
	};
	template <typename _TKey, typename _TLink>
	class TInnerNodeObj
	{
	public:
		typedef _TKey TKey;
		typedef _TLink TLink;
		TKey m_key;
		TLink m_nLink;
	};

	template <typename _TKey, typename _TVal>
	class TLeafNodeObj
	{
	public:
		typedef _TKey TKey;
		typedef _TVal TVal;
		TKey m_key;
		TVal m_val;
	};

	template<typename _TKey, typename _TValue, typename _TLink, typename _TComp>
	class BPNodeCompressor
	{
	public:
		typedef _TKey TKey;
		typedef _TValue TValue;
		typedef  _TComp TComp;
		typedef  _TLink TLink;

		typedef RBMap<TKey, TLink, TComp>   TInnerMemSet;
		typedef RBMap<TKey, TValue, TComp>   TLeafMemSet;

		typedef TInnerNodeObj<TKey, TLink> TInnerNodeObj;
		typedef TLeafNodeObj<TKey, TValue> TLeafNodeObj;

		typedef BNodeVectorRO<TKey, TValue, TComp> TInnerVector;
		typedef BNodeVectorRO<TKey, TLink, TComp>   TLeafVector;


		BPNodeCompressor(){}
		virtual ~BPNodeCompressor(){}

		virtual bool LoadInnerNode(TInnerMemSet& Set, CommonLib::FxMemoryReadStream& stream) = 0;
		virtual bool LoadInnerNode(TInnerVector& vec, CommonLib::FxMemoryReadStream& stream) = 0;
		virtual bool WriteInnerNode(TInnerMemSet& Set, CommonLib::FxMemoryWriteStream& stream) = 0;


		virtual bool LoadLeafNode(TLeafMemSet& Set, CommonLib::FxMemoryReadStream& stream) = 0;
		virtual bool LoadLeafNode(TLeafVector& vec, CommonLib::FxMemoryReadStream& stream) = 0;
		virtual bool WriteLeafNode(TLeafMemSet& Set, CommonLib::FxMemoryWriteStream& stream) = 0;

		//for inner
		virtual bool insertInner(TKey key, int64 nLink) = 0;
			//for leaf
		virtual bool insertLeaf(TKey key, TValue nValue) = 0;

		virtual bool remove(TKey key) = 0;

		virtual size_t cnt_size( size_t nCnt, bool bLeaf) const = 0;
		virtual size_t size(bool bLeaf) const = 0;
	};

	dsdvvds




	template<typename _TKey, typename _TValue, typename _TComp>
	class BPLeafNodeCompressorBase
	{
	public:
		typedef _TKey TKey;
		typedef _TValue TValue;
		typedef  _TComp TComp;

		typedef RBMap<TKey, TValue, TComp>   TLeafMemSet;
		typedef TLeafNodeObj<TKey, TValue> TLeafNodeObj;
		typedef BNodeVectorRO<TKey, TValue, TComp>   TLeafVector;
		typedef typename TLeafMemSet::TTreeNode TTreeNode;

		BPLeafNodeCompressorBase(){}
		virtual ~BPLeafNodeCompressorBase(){}
		virtual bool Load(TLeafMemSet& Set, CommonLib::FxMemoryReadStream& stream) = 0;
		virtual bool Load(TLeafVector& vec, CommonLib::FxMemoryReadStream& stream) = 0;
		virtual bool Write(TLeafMemSet& Set, CommonLib::FxMemoryWriteStream& stream) = 0;

		virtual bool insert(TTreeNode *pObj) = 0;
		virtual bool update(TTreeNode *pObj, const TValue& nValue) = 0;
		virtual bool remove(TTreeNode *pObj) = 0;
		virtual size_t size() const = 0;
		virtual size_t count() const = 0;
	};



	template<typename _TKey, typename _TLink, typename _TComp>
	class BPInnerNodeCompressorBase
	{
	public:
		typedef _TKey TKey;
		typedef _TLink TLink;
		typedef  _TComp TComp;

		typedef RBMap<TKey, TLink, TComp>   TInnerMemSet;
		typedef TInnerNodeObj<TKey, TLink> TInnerNodeObj;
		typedef BNodeVectorRO<TKey, TLink, TComp> TInnerVector;
		typedef typename TInnerMemSet::TTreeNode TTreeNode;


		BPInnerNodeCompressorBase(){}
		virtual ~BPInnerNodeCompressorBase(){}

		virtual bool Load(TInnerMemSet& Set, CommonLib::FxMemoryReadStream& stream) = 0;
		virtual bool Load(TInnerVector& vec, CommonLib::FxMemoryReadStream& stream) = 0;
		virtual bool Write(TInnerMemSet& Set, CommonLib::FxMemoryWriteStream& stream) = 0;

		virtual bool insert(TTreeNode *pObj) = 0;
		virtual bool remove(TTreeNode *pObj) = 0;
		virtual size_t size() const = 0;
		virtual size_t count() const = 0;
	};

}

#endif