#ifndef _LIB_COMMON_BITS_VECTOR_H_
#define _LIB_COMMON_BITS_VECTOR_H_
#include "general.h"
#include "blob.h"

namespace CommonLib
{
	class CBitsVector
	{
	public:
		CBitsVector(CommonLib::alloc_t *pAlloc = NULL, uint32 nSizeInByte = 0);
		~CBitsVector();
		void setBit(bool bBit, uint32 nBit);
		void push_back(bool bBit);


		bool getBit(uint32 nBit) const;

		bool operator [](uint32 nIndex) const;
	 
		void resize(uint32 nSizeInByte);
		void resizeBits(uint32 nSizeInBits);

		void reserve(uint32 nSizeInByte);
		void reserveBits(uint32 nSizeInBits);

		uint32 size() const;
		uint32 sizeInBits()const;

		uint32 capacity() const;
		uint32 capacityInBits() const;

		const byte *bits() const;
		byte *bits();

		void fill(bool bBit);
		void clear(bool bDel = false);
	private:
		void _resize(uint32 nSizeInByte);
		void _reserve(uint32 nSizeInByte);
		//uint32 getBitCapacity() const;
	private:
		byte* m_pBuffer;
		alloc_t *m_pAlloc;
		simple_alloc_t m_alloc;
		uint32 m_nCapacity;
		bool m_bAttach;
		uint32 m_nSizeInBits;
		uint32 m_nCapacityInBits;

	};
}

#endif