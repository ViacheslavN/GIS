#ifndef _EMBEDDED_DATABASE_BIT_MAP_H_
#define _EMBEDDED_DATABASE_BIT_MAP_H_

#include "Commonlibrary/general.h"
#include "Commonlibrary/alloc_t.h"

namespace embDB
{

	class CBitMap
	{
		public:
			CBitMap();
			CBitMap(uint32 nSizeInByte);
			CBitMap(byte *pBuf, uint32 nSize);
			CBitMap(const CBitMap&);
			~CBitMap();
			

			void setBits(byte *pBuf, uint32 nSize);
			byte *getBits();
			uint32 getBitSize() const;
			uint32 size() const;

			bool getBit(uint32 nBit) const;
			bool getBit(uint32 nBit, uint32& nVal) const;
			bool setBit(uint32 nBit, bool bBit);
	private:
		byte *m_pBuf;
		uint32 m_nSize;

	};
}

#endif