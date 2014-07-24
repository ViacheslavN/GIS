#ifndef _EMBEDDED_DATABASE_B_TREE_NODE_H_
#define _EMBEDDED_DATABASE_B_TREE_NODE_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "FilePage.h"
#include "Compressors.h"
#include "RBSet.h"
#include "SimpleCompessor.h"
#include "IDBTransactions.h"
#include "DBConfig.h"
namespace embDB
{

	//_TAggrKey - агрегатный ключ, данные вместе с ключем храняться
	//Нода для уникальных значений

	/*enum eBTreeNodePageFlags
	{
		ROOT_NODE = 0x01,
		CHANGE_NODE = 0x02,
		BUSY_NODE = 0x04
	};*/

	template <class _TNodeElem, class _TComp >
	class BTreeNode
	{
	public:

		BTreeNode(CommonLib::alloc_t *pAlloc, int64 nPageAddr, uint16 nComp);
		~BTreeNode();
		typedef _TNodeElem       TNodeElem;
		typedef _TComp   TComp;
		typedef RBSet<TNodeElem, TComp>   TMemSet;
		typedef typename TMemSet::TTreeNode TTreeNode;
		typedef NodeCompressor<TNodeElem> TCompressor;

		bool Load(IDBTransactions* pTransactions);
		bool Save(IDBTransactions* pTransactions);


		bool LoadFromPage(CFilePage* pFilePage);
	

		size_t size();
		bool isNoFull();
		bool  insert(const TNodeElem& obj);
		TNodeElem* find(const TNodeElem& obj);
		void remove(const TNodeElem& obj);
		void clear();
		bool findNodeInsert(const TNodeElem& obj, int64& nNextNode, BTreeNode** pNode);
		void InsertInNode(BTreeNode*pNode, const TNodeElem& key);
		void findKey(const TNodeElem& obj, int64& nNextNode, TTreeNode** pRBTreeNode);
		void setFlags(int nFlag, bool bSet);
		int getFlags(){return m_nFlag;}
		bool IsFree(){return !(m_nFlag & (ROOT_NODE | CHANGE_NODE));}
		int64 m_nPageAddr;
		int64 m_nLess;
		int64 m_nParent;
        TMemSet m_memset;
		TCompressor* m_pCompressor;
		int m_nFlag;
		CommonLib::alloc_t* m_pAlloc;
		
	};

	#include "BTreeNode.cpp"
}
#endif