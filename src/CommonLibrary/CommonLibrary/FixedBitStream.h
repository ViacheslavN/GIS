#ifndef _EMBEDDED_COMMON_I_FIXED_BIT_STERAM_H_
#define _EMBEDDED_COMMON_I_FIXED_BIT_STERAM_H_

#include "general.h"
#include "alloc_t.h"
#include "Stream.h"
#include "BitStream.h"
namespace CommonLib
{




	
	

	class BitStreamBase : public IStream
	{

	public:

		BitStreamBase(alloc_t *pAlloc = NULL);
		~BitStreamBase();


		virtual void create(size_t nSize);
		virtual void attach(byte* pBuffer, size_t nSize, bool bCopy = false);
		virtual void attachBits(byte* pBuffer, size_t nsizeInBits, bool bCopy = false);
		virtual byte* deattach();
		virtual byte* buffer();
		virtual const byte* buffer() const;
		virtual size_t size() const;
		virtual size_t sizeInBits() const;
		virtual bool seek(size_t position, enSeekOffset offset );
		virtual size_t pos() const;
		virtual size_t posInBits() const;
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

		size_t m_nEndBits;

	};

	class FxBitWriteStream : public BitStreamBase, public IWriteBitStreamBase
	{
	public:
		FxBitWriteStream(alloc_t *pAlloc = NULL);
		~FxBitWriteStream();
		virtual void attach(byte* pBuffer, size_t nSize);
		virtual void writeBit(bool bBit);
	private:
		

	};



	class FxBitReadStream : public BitStreamBase , public IReadBitStreamBase
	{
	public:
		FxBitReadStream(alloc_t *pAlloc = NULL);
		~FxBitReadStream();

		virtual bool readBit();
	private:
		

	};
}
#endif