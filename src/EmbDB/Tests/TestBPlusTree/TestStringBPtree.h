#ifndef _TEST_STRING_BP_TREE_
#define _TEST_STRING_BP_TREE_

#include "../../EmbDB/RBSet.h"
#include "../../EmbDB/RBmap.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "../../EmbDB/BTVector.h"
#include "CommonLibrary/general.h"
#define  STRING_SIZE 3000
struct sStringStrustTest
{
	sStringStrustTest() 
	{}
	byte m_buf[STRING_SIZE]; 
	byte* getBuf(){return m_buf;}
	const byte* getBuf() const {return m_buf;}

};

class BPStringLeafNodeCompressor 
{
public:
	typedef embDB::RBMap<int64, sStringStrustTest, embDB::comp<int64> >   TLeafMemSet;
	typedef  embDB::BNodeVectorRO<int64, sStringStrustTest, embDB::comp<int64> > TLeafVectorMemSet;
	typedef TLeafMemSet::TTreeNode TTreeNode;

	BPStringLeafNodeCompressor() : m_nSize(0)
	{}
	virtual ~BPStringLeafNodeCompressor(){}
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

		int64 nkey;
		sStringStrustTest nval;
		for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
		{
			KeyStreams.read(nkey);
			ValStreams.read(nval.getBuf(), STRING_SIZE);
	 		Set.insert(nkey, nval);
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

		int64 nkey;
		sStringStrustTest nval;
		for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
		{
			KeyStreams.read(nkey);
			ValStreams.read(nval.getBuf(), STRING_SIZE);
			Set.push_back(nkey, nval);
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

		uint32 nKeySize =  nSize * sizeof(int64);
		uint32 nValSize =  nSize * STRING_SIZE;

		stream.write(nKeySize);
		stream.write(nValSize);
		KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
		ValStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

		TLeafMemSet::iterator it = Set.begin();
		for(; !it.isNull(); ++it)
		{
			KeyStreams.write(it.key());
			ValStreams.write(it.value().getBuf(), STRING_SIZE);
		}
		assert(stream.pos() <= stream.size());
		return true;
	}

	virtual bool insert(TTreeNode *pNode)
	{
		m_nSize++;
		return true;
	}
	virtual bool update(TTreeNode *pNode, const sStringStrustTest& nValue)
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
		//return (sizeof(int64) + sizeof(sFileTranPageInfo)) *  m_nSize;
		return ((STRING_SIZE + sizeof(int64)) *  m_nSize) + 3 * sizeof(uint32);
	}
	size_t headSize() const
	{
		//return (sizeof(int64) + sizeof(sFileTranPageInfo)) *  m_nSize;
		return  3 * sizeof(uint32);
	}
	size_t rowSize() const
	{
		//return (sizeof(int64) + sizeof(sFileTranPageInfo)) *  m_nSize;
		return (STRING_SIZE + sizeof(int64)) *  m_nSize;
	}
	size_t count()
	{
		return m_nSize;
	}
	size_t tupleSize() const
	{
		return  (STRING_SIZE + sizeof(int64));
	}
private:
	size_t m_nSize;
};

#endif