#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_COMP_INFO_HEADER_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_COMP_INFO_HEADER_H_

#include "embDBInternal.h"
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


 
	class CompressorParamsBaseImp
	{
	public:
		CompressorParamsBaseImp() : m_compressType(ACCoding),m_bCalcOnlineSize(false), m_nErrorCalc(200), m_nMaxRowCoeff(1)
		{}
		virtual ~CompressorParamsBaseImp(){}

			 
		
		virtual bool load(CommonLib::IReadStream *pStream, IDBTransaction* pTran)
		{
			m_compressType = (CompressType)pStream->readintu16();
			m_bCalcOnlineSize = pStream->readBool();
			m_nErrorCalc	= pStream->readIntu32();
			m_nMaxRowCoeff  = pStream->readintu16();
			return true;
		}
		virtual bool save(CommonLib::IWriteStream *pStream, IDBTransaction* pTran)
		{
			pStream->write(uint16(m_compressType));
			pStream->write(m_bCalcOnlineSize);
			pStream->write(m_nErrorCalc);
			pStream->write(m_nMaxRowCoeff);
			return true;
		}
		template<class _Transaction>
		void SaveState(_Transaction * pTransaction)
		{

		}
 
		CompressType m_compressType;
		bool m_bCalcOnlineSize;
		uint32 m_nErrorCalc;
		uint16 m_nMaxRowCoeff;
	};
}




#endif