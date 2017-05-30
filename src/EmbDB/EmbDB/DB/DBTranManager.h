#ifndef _EMBEDDED_DATABASE_DB_TRAN_MANAGER_H_
#define _EMBEDDED_DATABASE_DB_TRAN_MANAGER_H_

#include "CommonLibrary/String.h"
#include "../embDBInternal.h"
#include "../storage/storage.h"
#include "../storage/FilePage.h"
#include "Database.h"
namespace embDB
{
	
	struct sTranInfo
	{
		byte szTranName[33]; //DDMMYY_HHMMCCMM_000000000000000
	};


	/*class BPTranManagerLeafNodeCompressor 
	{
	public:
		typedef RBMap<int64, sTranInfo, embDB::comp<int64> >   TLeafMemSet;
		typedef TLeafMemSet::TTreeNode TTreeNode;

		BPTranManagerLeafNodeCompressor() : m_nSize(0)
		{}
		virtual ~BPTranManagerLeafNodeCompressor(){}
		virtual bool Load(TLeafMemSet& Set, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValStreams;
			uint32 nSize = stream.readInt32();
			if(!nSize)
				return true;

			Set.reserve(nSize);

			uint32 nKeySize = stream.readInt32();
			uint32 nValSize = stream.readInt32();

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			ValStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValSize);

			int64 nkey;
			sTranInfo nval;
			for (uint32 nIndex = 0; nIndex < nSize; ++nIndex)
			{
				KeyStreams.read(nkey);
				ValStreams.read(nval.szTranName, sizeof(nval.szTranName));
				Set.insert(nkey, nval);
			}
			assert(ValStreams.pos() < stream.size());
			return true;
		}
		virtual bool Write(TLeafMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)Set.size();
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream ValStreams;

			uint32 nKeySize =  nSize * sizeof(int64);
			uint32 nValSize =  nSize * sizeof(sTranInfo);

			stream.write(nKeySize);
			stream.write(nValSize);
			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			ValStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValSize);

			TLeafMemSet::iterator it = Set.begin();
			for(; !it.isNull(); ++it)
			{
				KeyStreams.write(it.key());
				ValStreams.write(it.value().szTranName, sizeof(it.value().szTranName));
				 
			}
			assert(ValStreams.pos() < stream.size());
			return true;
		}

		virtual bool insert(TTreeNode *pNode)
		{
			m_nSize++;
			return true;
		}
		virtual bool update(TTreeNode *pNode, const sTranInfo& nValue)
		{
			return true;
		}
		virtual bool remove(TTreeNode *pNode)
		{
			m_nSize--;
			return true;
		}
		virtual uint32 size() const
		{
			//return (sizeof(int64) + sizeof(sFileTranPageInfo)) *  m_nSize;
			return sizeof(sTranInfo) *  m_nSize + 3 * sizeof(uint32);
		}
		virtual uint32 headSize() const
		{
			//return (sizeof(int64) + sizeof(sFileTranPageInfo)) *  m_nSize;
			return  3 * sizeof(uint32);
		}
		virtual uint32 rowSize() const
		{
			//return (sizeof(int64) + sizeof(sFileTranPageInfo)) *  m_nSize;
			return (sizeof(sTranInfo)  + sizeof(int64))*  m_nSize;
		}
		uint32 count()
		{
			return m_nSize;
		}
		uint32 tupleSize() const
		{
			return (sizeof(sTranInfo)  + sizeof(int64));
		}
	private:
		uint32 m_nSize;
	};
	*/

 
	class CDBTranManager
	{
	public:
		struct sTranManagerHeader
		{
			sTranManagerHeader() : nLastTranID(0), nRootPageTree(-1)
			{}
			int64 nLastTranID;
			int64 nRootPageTree;


		};

	/*	typedef embDB::BPInnerNodeSimpleCompressor<int64, int64, embDB::comp<int64> > TInnerCompressor;
		typedef embDB::TBPlusTreeMap<int64, sTranInfo, int64, embDB::comp<int64>, 
			CStorage, TInnerCompressor, BPTranManagerLeafNodeCompressor> TBTreePlus;*/

		CDBTranManager(CommonLib::alloc_t *pAlloc, CDatabase *pDB);
		~CDBTranManager();
		bool open(const CommonLib::CString &sFileName, const CommonLib::CString& sWorkingPath, ILogger *pLogger);
		bool close();
		ITransactionPtr CreateTransaction(eTransactionDataType trType, IDBConnection *pConn, eDBTransationType trDbType);
		bool releaseTransaction(ITransaction* pTran);
		void SetPageCipher(CPageCipher* pPageCliper);
	private:
		bool SaveHeader(CFilePage *pPage);
		bool LoadHeader(CFilePage *pPage);
	private:
		CStorage m_Storage;
		sTranManagerHeader m_Info;
	//	std::auto_ptr<TBTreePlus> m_pBPtree;
		int64 m_nTranID;
		CommonLib::alloc_t *m_pAlloc;
		typedef std::set<ITransactionPtr> TDBTransactions;
		TDBTransactions m_Transactions;
		CDatabase* m_pDB;
		CommonLib::CString m_sWorkingPath;
		ILoggerPtr m_pLogger;
		CPageCipher* m_pPageCliper;
	};
}

#endif