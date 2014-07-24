#ifndef _TEST_BIG_INNER_STRING_BP_TREE_
#define _TEST_BIG_INNER_STRING_BP_TREE_

#include "../../EmbDB/RBSet.h"
#include "../../EmbDB/RBmap.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "../../EmbDB/BTVector.h"
#include "CommonLibrary/general.h"
#define  INNER_STRING_SIZE 500
struct sStringInnerStrustTest
{
	sStringInnerStrustTest(	int64 nID) : m_nID(nID)
	{}
	sStringInnerStrustTest() : m_nID(-1)
	{}
	int64 m_nID;
	byte m_buf[INNER_STRING_SIZE]; 
	byte* getBuf(){return m_buf;}
	const byte* getBuf() const {return m_buf;}

	int64 operator - (sStringInnerStrustTest& obj)
	{
		return m_nID - obj.m_nID;
	}
};


struct InnerComp
	{	
		bool LE(const sStringInnerStrustTest& _Left, const sStringInnerStrustTest& _Right)
		{
			return (_Left.m_nID < _Right.m_nID);
		}
		bool EQ(const sStringInnerStrustTest& _Left, const sStringInnerStrustTest& _Right)
		{
			return (_Left.m_nID == _Right.m_nID);
		}
};

class BPStringInnerNodeCompressor 
{
public:
	typedef embDB::RBMap<sStringInnerStrustTest, int64, InnerComp >   TLeafMemSet;
	typedef  embDB::BNodeVectorRO<sStringInnerStrustTest, int64, InnerComp > TLeafVectorMemSet;
	typedef TLeafMemSet::TTreeNode TTreeNode;

	BPStringInnerNodeCompressor() : m_nSize(0)
	{}
	virtual ~BPStringInnerNodeCompressor(){}
	virtual bool Load(TLeafMemSet& Set, CommonLib::FxMemoryReadStream& stream)
	{
		CommonLib::FxMemoryReadStream KeyStreams;
		CommonLib::FxMemoryReadStream ValStreams;
		m_nSize = stream.readInt32();
		if(!m_nSize)
			return true;

		Set.reserve(m_nSize);

		uint32 nKeySize = stream.readInt32();
		uint32 nValSize = stream.readInt32();

		KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
		ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

		sStringInnerStrustTest nKey;
		int64 nPageAddr;
		for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
		{
			KeyStreams.read(nKey.m_nID);
			KeyStreams.read(nKey.getBuf(), INNER_STRING_SIZE);
			ValStreams.read(nPageAddr);
			Set.insert(nKey, nPageAddr);
		}
		assert(stream.pos() <= stream.size());
		return true;
	}
	virtual bool Load(TLeafVectorMemSet& Set, CommonLib::FxMemoryReadStream& stream)
	{
		CommonLib::FxMemoryReadStream KeyStreams;
		CommonLib::FxMemoryReadStream ValStreams;
		m_nSize = stream.readInt32();
		if(!m_nSize)
			return true;

		Set.reserve(m_nSize);

		uint32 nKeySize = stream.readInt32();
		uint32 nValSize = stream.readInt32();

		KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
		ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

		int64 nPageAddr;
		sStringInnerStrustTest nKey;
		for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
		{
			KeyStreams.read(nKey.m_nID);
			KeyStreams.read(nKey.getBuf(), INNER_STRING_SIZE);
			ValStreams.read(nPageAddr);
			Set.push_back(nKey, nPageAddr);
		}
		assert(stream.pos() <= stream.size());
		return true;
	}
	virtual bool Write(TLeafMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
	{
		uint32 nSize = (uint32)Set.size();
		assert(m_nSize == nSize);
		stream.write(nSize);
		if(!nSize)
			return true;

		CommonLib::FxMemoryWriteStream KeyStreams;
		CommonLib::FxMemoryWriteStream ValStreams;

		uint32 nKeySize =  nSize *(INNER_STRING_SIZE + sizeof(int64)) ;
		uint32 nValSize =  nSize * sizeof(int64);

		stream.write(nKeySize);
		stream.write(nValSize);
		KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
		ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

		TLeafMemSet::iterator it = Set.begin();
		for(; !it.isNull(); ++it)
		{
			KeyStreams.write(it.key().m_nID);
			KeyStreams.write(it.key().getBuf(), INNER_STRING_SIZE);
			ValStreams.write(it.value());
		}
		assert(stream.pos() <= stream.size());
		return true;
	}

	virtual bool insert(TTreeNode *pNode)
	{
		m_nSize++;
		return true;
	}
	virtual bool update(TTreeNode *pNode, const sStringInnerStrustTest& nValue)
	{
		return true;
	}
	virtual bool remove(TTreeNode *pNode)
	{
		m_nSize--;
		return true;
	}
	virtual size_t size() const
	{
		return ((INNER_STRING_SIZE  + 2* sizeof(int64))*  m_nSize) + 3 * sizeof(uint32);
	}
	size_t headSize() const
	{
		return  3 * sizeof(uint32);
	}
	size_t rowSize() const
	{
		return (INNER_STRING_SIZE  + 2* sizeof(int64)) *  m_nSize;
	}
	size_t count()
	{
		return m_nSize;
	}
	void clear()
	{
		m_nSize = 0;
	}
	size_t tupleSize() const
	{
		return INNER_STRING_SIZE  + 2* sizeof(int64);
	}
private:
	size_t m_nSize;
};



class BPKeyStringLeafNodeCompressor 
{
public:
	typedef embDB::RBMap<sStringInnerStrustTest, double, InnerComp >   TLeafMemSet;
	typedef  embDB::BNodeVectorRO<sStringInnerStrustTest, double, InnerComp > TLeafVectorMemSet;
	typedef TLeafMemSet::TTreeNode TTreeNode;

	BPKeyStringLeafNodeCompressor() : m_nSize(0)
	{}
	virtual ~BPKeyStringLeafNodeCompressor(){}
	virtual bool Load(TLeafMemSet& Set, CommonLib::FxMemoryReadStream& stream)
	{
		CommonLib::FxMemoryReadStream KeyStreams;
		CommonLib::FxMemoryReadStream ValStreams;
		m_nSize = stream.readInt32();
		if(!m_nSize)
			return true;

		Set.reserve(m_nSize);

		uint32 nKeySize = stream.readInt32();
		uint32 nValSize = stream.readInt32();

		KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
		ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

		sStringInnerStrustTest nKey;
		double nVal;
		for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
		{
			KeyStreams.read(nKey.m_nID);
			KeyStreams.read(nKey.getBuf(), INNER_STRING_SIZE);
			ValStreams.read(nVal);
			Set.insert(nKey, nVal);
		}
		assert(stream.pos() <= stream.size());
		return true;
	}
	virtual bool Load(TLeafVectorMemSet& Set, CommonLib::FxMemoryReadStream& stream)
	{
		CommonLib::FxMemoryReadStream KeyStreams;
		CommonLib::FxMemoryReadStream ValStreams;
		m_nSize = stream.readInt32();
		if(!m_nSize)
			return true;

		Set.reserve(m_nSize);

		uint32 nKeySize = stream.readInt32();
		uint32 nValSize = stream.readInt32();

		KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
		ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

		double nVal;
		sStringInnerStrustTest nKey;
		for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
		{
			KeyStreams.read(nKey.m_nID);
			KeyStreams.read(nKey.getBuf(), INNER_STRING_SIZE);
			ValStreams.read(nVal);
			Set.push_back(nKey, nVal);
		}
		assert(stream.pos() <= stream.size());
		return true;
	}
	virtual bool Write(TLeafMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
	{
		uint32 nSize = (uint32)Set.size();
		assert(m_nSize == nSize);
		stream.write(nSize);
		if(!nSize)
			return true;

		CommonLib::FxMemoryWriteStream KeyStreams;
		CommonLib::FxMemoryWriteStream ValStreams;

		uint32 nKeySize =  nSize *(INNER_STRING_SIZE + sizeof(int64)) ;
		uint32 nValSize =  nSize * sizeof(int64);

		stream.write(nKeySize);
		stream.write(nValSize);
		KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
		ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

		TLeafMemSet::iterator it = Set.begin();
		for(; !it.isNull(); ++it)
		{
			KeyStreams.write(it.key().m_nID);
			KeyStreams.write(it.key().getBuf(), INNER_STRING_SIZE);
			ValStreams.write(it.value());
		}
		assert(stream.pos() <= stream.size());
		return true;
	}

	virtual bool insert(TTreeNode *pNode)
	{
		m_nSize++;
		return true;
	}
	virtual bool update(TTreeNode *pNode, const sStringInnerStrustTest& nValue)
	{
		return true;
	}
	virtual bool remove(TTreeNode *pNode)
	{
		m_nSize--;
		return true;
	}
	virtual size_t size() const
	{
		return ((INNER_STRING_SIZE  + 2* sizeof(int64))*  m_nSize) + 3 * sizeof(uint32);
	}
	size_t headSize() const
	{
		return  3 * sizeof(uint32);
	}
	size_t rowSize() const
	{
		return (INNER_STRING_SIZE  + 2* sizeof(int64)) *  m_nSize;
	}
	size_t count()
	{
		return m_nSize;
	}
	size_t tupleSize() const
	{
		return INNER_STRING_SIZE  + 2* sizeof(int64);
	}
private:
	size_t m_nSize;
};
#endif