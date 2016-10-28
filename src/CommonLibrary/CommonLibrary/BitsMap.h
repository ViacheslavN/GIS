#ifndef _LIB_COMMON_BITS_MAP_H_
#define _LIB_COMMON_BITS_MAP_H_
#include "general.h"
#include "blob.h"

namespace CommonLib
{

	class CBitsMap
	{
		public:
			CBitsMap(CommonLib::alloc_t *pAlloc = NULL, uint32 nSizeInByte = 0, bool bInitBit = false);
			~CBitsMap();
			void setBit(bool bBit, uint32 nBit);
			bool getBit(uint32 nBit) const;

			void resize(uint32 nSizeInByte);
			void resizeBits(uint32 nSizeInBits);
			uint32 size() const;
			uint32 sizeInBits() const;

			const byte *bits() const;
			byte *bits();

			void fill(bool bBit);
		private:
			CommonLib::CBlob m_blob;
			bool m_bInitBit;

	};
}

#endif