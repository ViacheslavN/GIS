#ifndef _EMBEDDED_COMMON_I_FIXED_BIT_STERAM_H_
#define _EMBEDDED_COMMON_I_FIXED_BIT_STERAM_H_

#include "general.h"
#include "alloc_t.h"
#include "Stream.h"

namespace CommonLib
{
	class FxBitStreamBase : public IStream
	{

	public:

		FxBitStreamBase(alloc_t *pAlloc = NULL);
		~FxBitStreamBase();


		virtual void create(size_t nSize);
		virtual void attach(byte* pBuffer, size_t nSize);
		virtual byte* deattach();
		virtual byte* buffer();
		virtual size_t size() const;
		virtual bool seek(size_t position, enSeekOffset offset );
		virtual size_t pos() const;
		virtual void reset();
		virtual void close();

	protected:
		static const uint32 m_nBitBase = 7;
		byte* m_pBuffer;
		size_t  m_nPos;
		size_t  m_nSize;
		alloc_t *m_pAlloc;
		bool m_bAttach;
		size_t m_nCurrBit;

	};

	class FxBitWriteStream : public FxBitStreamBase 
	{
	public:
		FxBitWriteStream(alloc_t *pAlloc = NULL);
		~FxBitWriteStream();
		virtual void attach(byte* pBuffer, size_t nSize);
		void writeBit(bool bBit);
		void writeBit(byte nBit);
		void writeBits(byte nBits, size_t nCntBits);
		void writeBits(uint16 nBits, size_t nCntBits);
		void writeBits(uint32 nBits, size_t nCntBits);
		void writeBits(uint64 nBits, size_t nCntBits);

	private:
		template <class TVal>
		void _writeBits(TVal bits, size_t nCount)
		{
			for (size_t i = 0; i < nCount; ++i)
			{
				bool bBit = (bits & ((TVal)0x01 << i)) ? true : false;
				writeBit(bBit);
			}
		}

	};



	class FxBitReadStream : public FxBitStreamBase 
	{
	public:
		FxBitReadStream(alloc_t *pAlloc = NULL);
		~FxBitReadStream();

		bool readBit();
		void readBit(byte & nBit);
		void readBits(byte& nBits, size_t nCntBits);
		void readBits(uint16&  nBits, size_t nCntBits);
		void readBits(uint32&  nBits, size_t nCntBits);
		void readBits(uint64&  nBits, size_t nCntBits);
	private:
		template <class TVal>
		void _readBits(TVal& bits, size_t nCount)
		{
			bits = 0;
			for (size_t i = 0; i < nCount; ++i)
			{
				bool bBit = readBit();
				if(bBit)
					bits |= ((TVal)0x01 << i);
				 
			}
		}

	};
}
#endif