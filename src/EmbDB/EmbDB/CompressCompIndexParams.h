#ifndef _EMBEDDED_DATABASE_COMPOSITE_INDEX_COMPRESS_PARAMS_H_
#define _EMBEDDED_DATABASE_COMPOSITE_INDEX_COMPRESS_PARAMS_H_
#include "embDBInternal.h"
#include "CompressorParams.h"
 


namespace embDB
{
class CompIndexParams : public CompressorParamsBaseImp
{
	public:
		CompIndexParams();
		~CompIndexParams();

		virtual bool load(CommonLib::IReadStream* pStream, IDBTransaction *pTran);
		virtual bool save(CommonLib::IWriteStream* pStream,  IDBTransaction *pTran);
		const std::vector<uint16>& getScheme() const {return m_vecScheme;}
		uint32 getRowSize() const{ return m_nRowSize;}
		void addParams(eDataTypes type, uint32 nSize = 0);
	private:

		std::vector<uint16> m_vecScheme;
		std::vector<std::pair<uint16, uint32>> m_vecSchemeParams;
		uint32 m_nRowSize;

	};
}
#endif