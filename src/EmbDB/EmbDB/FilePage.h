#ifndef _EMBEDDED_DATABASE_FILE_PAGE_H_
#define _EMBEDDED_DATABASE_FILE_PAGE_H_

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "FilePageType.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "CRC.h"
#include "IRefCnt.h"
namespace embDB
{
	enum ePageFlags
	{

		eFP_NEW = 1,
		eFP_CHANGE = 2,
		eFP_EMPTY = 4,
		eFP_REMOVE = 8,
		eFP_FROM_FREE_PAGES = 16
	};

struct sFilePageHeader
{
	uint32 m_nCRC32;
	uint32 m_nSize;
	uint16 m_nObjectPageType;
	uint16 m_nSubObjectPageType;
	uint32 m_nCalcCRC32;


	sFilePageHeader() : m_nObjectPageType(0), m_nSubObjectPageType(0), m_nCRC32(0), m_nCalcCRC32(0), m_nSize(0)
	{}

	sFilePageHeader(CommonLib::FxMemoryReadStream& stream, bool bCalcCRC = true)
	{
		read(stream, bCalcCRC);
	}

	sFilePageHeader(CommonLib::FxMemoryWriteStream& stream, uint16 nObjectPageType, uint16 nSubObjectPageType) :
		m_nObjectPageType(nObjectPageType), m_nSubObjectPageType(nSubObjectPageType), m_nCRC32(0), m_nCalcCRC32(0)
	{
		write(stream);
	}


	static uint32 size() 
	{
		return 3* (sizeof(uint16) )+ 2 * sizeof(uint32);
	}

	void write(CommonLib::FxMemoryWriteStream& stream)
	{
		stream.write(m_nCRC32);
		stream.write(m_nSize);
		stream.write(m_nObjectPageType);
		stream.write(m_nSubObjectPageType);
	}

	void writeCRC32(CommonLib::FxMemoryWriteStream& stream)
	{
		m_nSize = stream.pos() - 2 *sizeof(uint32);
		uint32 nCRC = Crc32(stream.buffer() + 2 *sizeof(uint32), m_nSize);
		size_t pos = stream.pos();
		stream.seek(0, CommonLib::soFromBegin);
		stream.write(nCRC);
		stream.write(m_nSize);
		stream.seek(pos, CommonLib::soFromBegin );
	}

	void read(CommonLib::FxMemoryReadStream& stream, bool bCalcCRC = true)
	{
		stream.read(m_nCRC32);
		stream.read(m_nSize);
		stream.read(m_nObjectPageType);
		stream.read(m_nSubObjectPageType);
		if(bCalcCRC)
		{
			m_nCalcCRC32 = Crc32(stream.buffer() + 2 *sizeof(uint32), m_nSize);
		}
	}

	bool isValid() const 
	{
		return m_nCalcCRC32 == m_nCRC32;
	}
};

class CFilePage : public RefCounter
{
public: 
	CFilePage(CommonLib::alloc_t *m_pAlloc, byte *pData, size_t nSize, int64 nAddr);
	CFilePage(CommonLib::alloc_t *m_pAlloc, size_t nSize, int64 nAddr);
	~CFilePage();
	byte* getRowData() const;
	size_t getPageSize() const;
	int64 getAddr() const;
	int getFlags() const;
	void setFlag(int nFlag, bool bSet);
	void setAddr(int64 nAddr);
	bool IsFree();
	bool copyFrom(CFilePage *pPage);
	bool isValid() const;
	void setValid(bool bValid);
	bool isCheck() const;
	void setCheck(bool bCheck);
protected:
	byte *m_pData;
	size_t m_nSize;
	int64 m_nAddr;
	CommonLib::alloc_t *m_pAlloc;
	int m_nFlags;
	bool m_bValid;
	bool m_bCheck;
	
};

typedef IRefCntPtr<CFilePage> FilePagePtr;
}
#endif //_EMBEDDED_DATABASE_FILE_PAGE_H_