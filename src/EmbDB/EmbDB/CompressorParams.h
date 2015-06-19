#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_COMP_INFO_HEADER_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_COMP_INFO_HEADER_H_

#include "IDBTransactions.h"
#include "CommonLibrary/FixedMemoryStream.h"
namespace embDB
{

	enum eCompressorParamsID
	{
		eBaseCompID = 1,
		eComposeIndexCompID = 2
	};

	class ICompressorParams
	{
	public:
		virtual ~ICompressorParams(){}
		virtual eCompressorParamsID getCompressorParmasID() const = 0;
		virtual int64 getRootPage() const = 0;
		virtual void setRootPage(int64 nPageID) = 0;
		
	};

	template <class _Transaction>
	class CompressorParamsBase : public ICompressorParams
	{
	public:
		CompressorParamsBase()
		{}
		virtual ~CompressorParamsBase(){}

		virtual bool read(_Transaction *pTran) = 0;
		virtual bool save(_Transaction *pTran) = 0;
	};


 
	class CompressorParamsBaseImp : public CompressorParamsBase<IDBTransactions>
	{
	public:
		CompressorParamsBaseImp() : m_nRootPage(-1)
		{}
		virtual ~CompressorParamsBaseImp(){}

			 
		virtual int64 getRootPage() const 
		{
			return m_nRootPage;
		}
		virtual void setRootPage(int64 nPageID)
		{
			m_nRootPage = nPageID;
		}
		virtual bool read(IDBTransactions *pTran)
		{
			return true;
		}
		virtual bool save(IDBTransactions *pTran)
		{
			return true;
		}
	private:
		int64 m_nRootPage;
	};
}




#endif