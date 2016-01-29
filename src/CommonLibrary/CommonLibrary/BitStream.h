#ifndef _EMBEDDED_COMMON_I_BIT_STERAM_H_
#define _EMBEDDED_COMMON_I_BIT_STERAM_H_
#include "stream.h"
namespace CommonLib
{
	class IBitWriteStream
	{
	public:
		IBitWriteStream(){}
		virtual ~IBitWriteStream(){}
		virtual void writeBit(bool bBit) = 0;
		virtual void writeBit(byte nBit) = 0;
		virtual void writeBits(byte nBits, size_t nCntBits) = 0;
		virtual void writeBits(uint16 nBits, size_t nCntBits) = 0;
		virtual void writeBits(uint32 nBits, size_t nCntBits) = 0;
		virtual void writeBits(uint64 nBits, size_t nCntBits) = 0;
	};


	class IBitReadStream
	{
	public:
		IBitReadStream(){}
		virtual ~IBitReadStream(){}
		virtual bool readBit() = 0;
		virtual void readBits(byte& nBits, size_t nCntBits) = 0;
		virtual void readBits(uint16&  nBits, size_t nCntBits) = 0;
		virtual void readBits(uint32&  nBits, size_t nCntBits) = 0;
		virtual void readBits(uint64&  nBits, size_t nCntBits) = 0;
	};



	class IReadBitStreamBase : public IBitReadStream
	{
	public:


		IReadBitStreamBase();
		virtual ~IReadBitStreamBase();
		virtual void readBits(byte& nBits, size_t nCntBits);
		virtual void readBits(uint16&  nBits, size_t nCntBits);
		virtual void readBits(uint32&  nBits, size_t nCntBits);
		virtual void readBits(uint64&  nBits, size_t nCntBits);
	
	protected:

		template <class TVal>
		void _readBits(TVal& bits, size_t nCount)
		{
			bits = 0;
			for (int i = nCount - 1; i >= 0; --i)
			{
				bool bBit = readBit();
				if(bBit)
					bits |= ((TVal)0x01 << i);

			}
		}
	};


	class IWriteBitStreamBase : public IBitWriteStream
	{
	public:


		IWriteBitStreamBase();
		virtual ~IWriteBitStreamBase();
		
		virtual void writeBit(byte nBit);
		virtual void writeBits(byte nBits, size_t nCntBits);
		virtual void writeBits(uint16 nBits, size_t nCntBits);
		virtual void writeBits(uint32 nBits, size_t nCntBits);
		virtual void writeBits(uint64 nBits, size_t nCntBits);

	protected:

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




	class BitStreamBase : public IStream
	{

	public:

		BitStreamBase(alloc_t *pAlloc = NULL);
		~BitStreamBase();


		virtual bool create(uint32 nSize);
		virtual bool attach(byte* pBuffer, uint32 nSize, bool bCopy = false);
		virtual void attachBits(byte* pBuffer, uint32 nsizeInBits, bool bCopy = false);
		virtual byte* deattach();
		virtual byte* buffer();
		virtual const byte* buffer() const;
		virtual int32 size() const;
		virtual int64 size64() const;
		virtual uint32 sizeInBits() const;
		virtual bool seek(uint32 position, enSeekOffset offset );
		virtual bool seek64(uint64 position, enSeekOffset offset );
		virtual int32 pos() const;
		virtual int64 pos64() const;
		virtual uint32 posInBits() const;
		virtual void reset();
		virtual void close();

	protected:
		static const uint32 m_nBitBase = 7;
		byte* m_pBuffer;
		uint32  m_nPos;
		uint32  m_nSize;
		alloc_t *m_pAlloc;
		bool m_bAttach;
		uint32 m_nCurrBit;

		uint32 m_nEndBits;

	};


}
#endif