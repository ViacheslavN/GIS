#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_COMP_INFO_HEADER_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_COMP_INFO_HEADER_H_

#include "IDBTransactions.h"
#include "CommonLibrary/FixedMemoryStream.h"
namespace embDB
{

	enum eCompressorParamsID
	{
		eBaseCompID = 1
	};

	class ICompressorParams
	{
	public:
		virtual ~ICompressorParams(){}
		virtual eCompressorParamsID getCompressorParmasID() const = 0;
		virtual uint32 getCompressorID() const = 0;
		virtual int64 getRootPage() const = 0;
		
	};

	template <class _Transaction>
	class CompressorParamsBase : public ICompressorParams
	{
	public:
		virtual eCompressorParamsID getCompressorParmasID() const {return eBaseCompID;}
		virtual uint32 getCompressorID() const{	return m_nCompID;}
		virtual int64 getRootPage() const{	return m_nPageID;	}

		CompressorParamsBase(int64 nPageID) : m_nPageID(nPageID)
		{}
		virtual ~CompressorParamsBase(){}

		virtual bool read(CommonLib::FxMemoryReadStream& stream, _Transaction *pTran)
		{
			m_nCompID = stream.readInt32();
		}
		virtual bool save(CommonLib::FxMemoryWriteStream& stream, _Transaction *pTran)
		{
			stream.write(m_nCompID);
		}
	private:
		int64 m_nPageID;
		uint32 m_nCompID;
	};
}




#endif