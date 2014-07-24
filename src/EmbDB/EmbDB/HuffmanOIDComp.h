#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_HUFFMAN_OID_COMP_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_HUFFMAN_OID_COMP_H_

namespace embDB
{

	struct sHuffmanNode
	{

		sHuffmanNode() :
			m_pLeft(0), m_pRight(0), m_pParent(0), m_nWeight(0), m_bLeaf(false)
			{}
		~sHuffmanNode(){}

		sHuffmanNode* m_pLeft;
		sHuffmanNode* m_pRight;
		sHuffmanNode* m_pParent;
		size_t m_nWeight; 
		bool m_bLeaf;
	};
	class CHuffmanOIDComp
	{
	public:
		CHuffmanOIDComp();
		~CHuffmanOIDComp();
	private:
		size_t m_nLenCounts[64];
		sHuffmanNode m_pRootTree;

	};
}
#endif