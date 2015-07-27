#ifndef _EMBEDDED_DATABASE_COMPOSITE_INDEX_COMPRESS_PARAMS_H_
#define _EMBEDDED_DATABASE_COMPOSITE_INDEX_COMPRESS_PARAMS_H_
#include "CompressorParams.h"
#include "IField.h"


namespace embDB
{
class CompIndexParams : public CompressorParamsBaseImp
{
	public:
		CompIndexParams();
		~CompIndexParams();

		virtual bool read(IDBTransactions *pTran);
		virtual bool save(IDBTransactions *pTran);
		const std::vector<uint16>& getScheme() const {return m_vecScheme;}
		uint32 getRowSize() const{ return m_nRowSize;}
		void addParams(CommonLib::eDataTypes type, uint32 nSize = 0);
	private:

		std::vector<uint16> m_vecScheme;
		std::vector<std::pair<uint16, uint32>> m_vecSchemeParams;
		uint32 m_nRowSize;

	};
}
#endif