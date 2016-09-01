#ifndef _EMBEDDED_DATABASE_FILE_PAGE_H_
#define _EMBEDDED_DATABASE_FILE_PAGE_H_

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "FilePageType.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "CRC.h"
#include "CommonLibrary/IRefCnt.h"

namespace embDB
{
	enum ePageFlags
	{

		eFP_NEW = 1,
		eFP_CHANGE = 2,
		eFP_EMPTY = 4,
		eFP_REMOVE = 8,
		eFP_FROM_FREE_PAGES = 16,
		eFP_INNER_TRAN_PAGE = 32
	};

struct sFilePageHeader
{
	/*uint64 m_nTranID:
	uint32 m_nTime;
	uint32 m_nOps;*/

	uint32 m_nCRC32;
	uint32 m_nSize;
	uint16 m_nObjectPageType;
	uint16 m_nSubObjectPageType;
	uint32 m_nCalcCRC32;
	uint32 m_nPageSize;
	bool m_bCheckCRC;
	bool m_bCheckPageType;
	sFilePageHeader(uint32 nPageSize) : m_nObjectPageType(0), m_nSubObjectPageType(0), m_nCRC32(0),
		m_nCalcCRC32(0), m_nSize(0), m_nPageSize(nPageSize), m_bCheckPageType(true)
	{}

	sFilePageHeader(CommonLib::FxMemoryReadStream& stream, uint32 nPageSize, bool bCalcCRC, bool bCheckPageType = true): m_nPageSize(nPageSize), m_nSize(0),
		m_bCheckCRC(bCalcCRC), m_bCheckPageType(bCheckPageType)
	{
		read(stream);
	}

	sFilePageHeader(CommonLib::FxMemoryWriteStream& stream, uint16 nObjectPageType, uint16 nSubObjectPageType, uint32 nPageSize, bool bCalcCRC, bool bCheckPageType = true) :
		m_nObjectPageType(nObjectPageType), m_nSubObjectPageType(nSubObjectPageType), m_nCRC32(0), m_nCalcCRC32(0), m_nPageSize(nPageSize), m_nSize(0),
			m_bCheckCRC(bCalcCRC), m_bCheckPageType(bCheckPageType)
	{
		write(stream);
	}


	sFilePageHeader(uint16 nObjectPageType, uint16 nSubObjectPageType, uint32 nPageSize, bool bCalcCRC) :
	m_nObjectPageType(nObjectPageType), m_nSubObjectPageType(nSubObjectPageType), m_nCRC32(0), m_nCalcCRC32(0), m_nPageSize(nPageSize), m_nSize(0), m_bCheckCRC(bCalcCRC), m_bCheckPageType(true)
	{}


	static uint32 size(bool bCheckCRC = true, bool bCheckPageType = true) 
	{
		return (bCheckPageType ? 2* (sizeof(uint16) ) : 0 ) +  (bCheckCRC ? 2 * sizeof(uint32) : 0);
	}

	void write(CommonLib::FxMemoryWriteStream& stream)
	{
		assert(m_nPageSize != 0);
		 if(m_bCheckCRC)
		 {
			 stream.write(m_nCRC32);
			 stream.write(m_nSize);
		 }
		 if(m_bCheckPageType)
		 {
			 stream.write(m_nObjectPageType);
			 stream.write(m_nSubObjectPageType);
		 }

	}

	void writeCRC32(CommonLib::FxMemoryWriteStream& stream)
	{
		if(!m_bCheckCRC)
			return;

		assert(m_nPageSize != 0);
		m_nSize = stream.pos() - 2 *sizeof(uint32);
		assert(m_nSize <= (m_nPageSize - 2 *sizeof(uint32)));
		uint32 nCRC = Crc32(stream.buffer() + 2 *sizeof(uint32), m_nSize);
		uint32 pos = stream.pos();
		stream.seek(0, CommonLib::soFromBegin);
		stream.write(nCRC);
		stream.write(m_nSize);
		stream.seek(pos, CommonLib::soFromBegin );
	}

	void read(CommonLib::FxMemoryReadStream& stream)
	{
		assert(m_nPageSize != 0);
		if(m_bCheckCRC)
		{
			stream.read(m_nCRC32);
			stream.read(m_nSize);
		}
		if(m_bCheckPageType)
		{
			stream.read(m_nObjectPageType);
			stream.read(m_nSubObjectPageType);
		}
 
		if(m_bCheckCRC && m_nSize <= m_nPageSize)
		{

			m_nCalcCRC32 = Crc32(stream.buffer() + 2 *sizeof(uint32), m_nSize);
		}
	}

	bool isValid() const 
	{
		if(!m_bCheckCRC)
			return true;

		return m_nCalcCRC32 == m_nCRC32 && m_nSize <= m_nPageSize;
	}
};

class CFilePage : public CommonLib::AutoRefCounter
{
public: 
	CFilePage(CommonLib::alloc_t *m_pAlloc, byte *pData, uint32 uint32, int64 nAddr);
	CFilePage(CommonLib::alloc_t *m_pAlloc, uint32 nSize, int64 nAddr);
	CFilePage( byte *pData, uint32 nSize, int64 nAddr);
	~CFilePage();
	byte* getRowData() const;
	uint32 getPageSize() const;
	int64 getAddr() const;
	uint32 getFlags() const;
	void setFlag(uint32 nFlag, bool bSet);
	void setAddr(int64 nAddr);
	bool IsFree();
	bool copyFrom(CFilePage *pPage);
	bool isValid() const;
	void setValid(bool bValid);
	/*bool isIgnoreOffset() const;
	void setIgnoreOffset(bool bIgnore);*/

	//bool isCheck() const;
	//void setCheck(bool bCheck);
	bool isNeedEncrypt() const;
	void setNeedEncrypt(bool bEncrypt);
public:



protected:
	byte *m_pData;
	uint32 m_nSize;
	int64 m_nAddr;
	CommonLib::alloc_t *m_pAlloc;
	uint32 m_nFlags;
	bool m_bValid;
	bool m_bCheck;
	bool m_bIsNeedEncrypt;
	bool m_bIgnoreOffset;
};

typedef CommonLib::IRefCntPtr<CFilePage> FilePagePtr;
}
#endif //_EMBEDDED_DATABASE_FILE_PAGE_H_