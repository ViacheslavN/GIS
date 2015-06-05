#ifndef _EMBEDDED_DATABASE_COMPOSITE_INDEX_COMPRESS_PARAMS_H_
#define _EMBEDDED_DATABASE_COMPOSITE_INDEX_COMPRESS_PARAMS_H_
#include "CompressorParams.h"
#include "IField.h"


namespace embDB
{
class CompIndexParams : public CompressorParamsBase<IDBTransactions>
	{
	public:

		typedef CompressorParamsBase<IDBTransactions> TBase;
		CompIndexParams(int64 nPageID) : TBase(nPageID), m_nCompID((uint32)eComposeIndexCompID)
		{}
		~CompIndexParams();


		virtual eCompressorParamsID getCompressorParmasID() const {return eComposeIndexCompID;}
		virtual uint32 getCompressorID() const{	return m_nCompID;}
		virtual int64 getRootPage() const{	return m_nPageID;	}

		virtual bool read(CommonLib::FxMemoryReadStream& stream, IDBTransactions *pTran)
		{
			 uint32 nCount = 
		}
		virtual bool save(CommonLib::FxMemoryWriteStream& stream, IDBTransactions *pTran)
		{
			 
		}

	private:

		std::vector<eDataTypes> m_vecScheme;
	};
}
#endif