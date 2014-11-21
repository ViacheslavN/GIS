#ifndef _EMBEDDED_DATABASE_BP_TREE_INFO_PAGE_
#define _EMBEDDED_DATABASE_BP_TREE_INFO_PAGE_

#include "TranStorage.h"
#include "FilePage.h"
#include "BaseBPMapv2.h"
 

namespace embDB
{
	struct sFileTranPageInfo
	{
		sFileTranPageInfo(int64  nAddr = -1, int nFlags = -1) : m_nFileAddr(nAddr), m_nFlags(nFlags), m_bOrignSave(false),
			m_bRedoSave(false)
		{}
		int64  m_nFileAddr; // адрес страницы в файле транзакций
		uint32 m_nFlags; //флаги выгружаемой страницы
		bool m_bOrignSave; // for undo
		bool m_bRedoSave; // for redo
	};

	class BPNewPageLeafNodeCompressor 
	{
	public:
 

		typedef int64 TKey;
		typedef sFileTranPageInfo TValue;
		typedef  TBPVector<TKey> TLeafKeyMemSet;
		typedef  TBPVector<TValue> TLeafValueMemSet;

		BPNewPageLeafNodeCompressor(ICompressorParams *pParams = NULL) : m_nSize(0)
		{}
		virtual ~BPNewPageLeafNodeCompressor(){}
		virtual bool Load(TLeafKeyMemSet& vecKeys, TLeafValueMemSet& vecValues, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValueStreams;
			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			vecKeys.reserve(m_nSize);
			vecValues.reserve(m_nSize);

	 
			uint32 nKeySize =  m_nSize * sizeof(TKey);
			uint32 nValueSize =  m_nSize *  (sizeof(int64) + sizeof(int32));

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			ValueStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValueSize);

			int64 nkey;
			sFileTranPageInfo nval;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(nkey);
				ValueStreams.read(nval.m_nFileAddr);
				ValueStreams.read(nval.m_nFlags);
				vecKeys.push_back(nkey);
				vecValues.push_back(nval);
			}
 
			return true;
		}
		virtual bool Write(TLeafKeyMemSet& vecKeys, TLeafValueMemSet& vecValues, CommonLib::FxMemoryWriteStream& stream)
		{
			assert(m_nSize == vecKeys.size());
			stream.write(m_nSize);
			if(!m_nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream valueStreams;


			uint32 nKeySize =  m_nSize * sizeof(int64);
			uint32 nValSize =  m_nSize * (sizeof(int64) + sizeof(int32));

	 
			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			valueStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);
			stream.seek(stream.pos() + nKeySize + nValSize, CommonLib::soFromBegin);			 

			for(size_t i = 0, sz = vecKeys.size(); i < sz; ++i)
			{
				KeyStreams.write(vecKeys[i]);
				valueStreams.write(vecValues[i].m_nFileAddr);
				valueStreams.write(vecValues[i].m_nFlags);
			}
		 	return true;
		}

		virtual bool insert(const TKey& key, const TValue& value)
		{
			m_nSize++;
			return true;
		}
		virtual bool add(const TLeafKeyMemSet& vecKeys, const TLeafValueMemSet& vecValues)
		{
			m_nSize += vecKeys.size();
			return true;
		}
		virtual bool recalc(const TLeafKeyMemSet& vecKeys, const TLeafValueMemSet& vecValues)
		{
			m_nSize = vecKeys.size();
			return true;
		}
		virtual bool update(const TKey& key, const TValue& value)
		{
			return true;
		}
		virtual bool remove(const TKey& key)
		{
			m_nSize--;
			return true;
		}
		virtual size_t size() const
		{
			return ( (2 * sizeof(int64) + sizeof(int32)) *  m_nSize) +sizeof(uint32);
		}
		size_t headSize() const
		{
			return   sizeof(uint32);
		}
		 size_t rowSize() const
		{
			return (2 * sizeof(int64) + sizeof(int32)) *  m_nSize;
		}
		size_t count()
		{
			return m_nSize;
		}
		size_t tupleSize() const
		{
			return  (sizeof(int64) + sizeof(int32));
		}
	private:
		size_t m_nSize;
	};

	/*class  BPNewPageStorage
	{
	public:	
		BPNewPageStorage(CTranStorage *pTranStorage, CommonLib::alloc_t *pAlloc);
		~BPNewPageStorage();
		int64 saveFilePage(FilePagePtr pPage, size_t pos = 0);
		FilePagePtr getFilePage(int64 nAddr, bool bRead = true);
		FilePagePtr getNewPage();
		void error(const CommonLib::str_t& sError){}
		size_t getPageSize(){return m_pTranStorage->getPageSize();}
		bool dropFilePage(FilePagePtr pPage){return true;}
		bool dropFilePage(int64 pAddrPage){return true;}
	private:
		CTranStorage *m_pTranStorage;
		size_t m_nMaxPageBuf;
		typedef TSimpleCache<int64, CFilePage> TNodesCache;
		TNodesCache m_Chache;

	};*/


}
#endif