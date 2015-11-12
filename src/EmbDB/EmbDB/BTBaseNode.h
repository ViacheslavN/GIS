#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_BASE_NODE_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_BASE_NODE_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "Key.h"
#include "RBMap.h"
#include "BPCompressors.h"
#include "embDBInternal.h"
#include "BPSimpleCompessor.h"
#include "BTVector.h"
#include "CompressorParams.h"
namespace embDB
{
	enum eBPTreeNodePageFlags
	{
		ROOT_NODE = 1,
		CHANGE_NODE = 2,
		BUSY_NODE = 4,
		REMOVE_NODE = 8,
		CHECK_REM_NODE = 16
	};

 
	class BPBaseTreeNode
	{
	public:
		BPBaseTreeNode() : m_nFlag(0){}
		//virtual ~BPBaseTreeNode(){}
		virtual bool isLeaf() const = 0;
		virtual  bool Load(CommonLib::FxMemoryReadStream& stream) = 0;
		virtual  bool Save(CommonLib::FxMemoryWriteStream& stream) = 0;
		virtual size_t size() const = 0;
		virtual size_t headSize() const = 0;
		virtual size_t rowSize() const = 0;
		virtual	size_t tupleSize() const= 0;
		virtual bool IsFree() const = 0;
		//virtual bool init(ICompressorParams *pParams = NULL) = 0;
		virtual int getFlags() const
		{
			return m_nFlag;
		}
		virtual void setFlags(int nFlag, bool bSet) 
		{
			if(bSet)
				m_nFlag |= nFlag;
			else
				m_nFlag &= ~nFlag;
		}
		virtual void clear() = 0;
	public:
		int m_nFlag;
	};



	template<typename _TKey, typename _TLink, typename _TComp>
	class BPSetTreeLeafNodeBase : public  BPBaseTreeNode
	{
	public:
		typedef _TKey TKey;
		typedef _TComp TComp;
		typedef _TLink TLink;
		typedef RBMap<TKey,  TComp>   TLeafMemSet;

		BPSetTreeLeafNodeBase() : m_nNext(-1), m_nPrev(-1) {}
		virtual ~BPSetTreeLeafNodeBase() {}

		virtual bool insert(const TKey& key) = 0;
		virtual bool remove(const TKey& key) = 0;
	public:

		TLink m_nNext;
		TLink m_nPrev;
	};


	template<typename _TKey, typename _TValue, typename _TLink, typename _TComp,
	typename _TLeafMemset =  RBMap<_TKey, _TValue, _TComp> >
	class BPTreeLeafNodeBase : public  BPBaseTreeNode
	{
	public:
		typedef _TKey TKey;
		typedef _TValue TValue;
		typedef _TComp TComp;
		typedef _TLink TLink;
		typedef _TLeafMemset   TLeafMemSet;

		BPTreeLeafNodeBase() : m_nNext(-1), m_nPrev(-1) {}
		virtual ~BPTreeLeafNodeBase() {}

		virtual bool insert(const TKey& key, const TValue& value) = 0;
		virtual bool remove(const TKey& key) = 0;
		virtual bool update(const TKey& key, const TValue& newValue) = 0;
	public:

		TLink m_nNext;
		TLink m_nPrev;
	};


	template<typename _TKey, typename _TLink, typename _TComp>
	class BPTreeInnerNodeBase : public  BPBaseTreeNode
	{
	public:
		typedef _TKey TKey;
		typedef _TLink TLink;
		typedef _TComp TComp;
		typedef RBMap<TKey, _TLink, TComp>   TInnerMemSet;
	
		BPTreeInnerNodeBase() : m_nLess(-1) {}
		virtual ~BPTreeInnerNodeBase() {}


		virtual bool insert(const TKey& key, TLink nLink) = 0;
		virtual bool remove(const TKey& key) = 0;
	public:
		TLink m_nLess;
	};

}
#endif