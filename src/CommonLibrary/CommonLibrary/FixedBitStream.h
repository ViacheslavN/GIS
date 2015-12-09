#ifndef _EMBEDDED_COMMON_I_FIXED_BIT_STERAM_H_
#define _EMBEDDED_COMMON_I_FIXED_BIT_STERAM_H_

#include "general.h"
#include "alloc_t.h"
#include "Stream.h"

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


	class IWriteBitStreamBase : public IBitWriteStream
	{
	public:


		IBitReadStreamBase()  {}
		virtual ~IBitReadStreamBase()  {}


		virtual void readBits(byte& nBits, size_t nCntBits)
		{
			assert(nCntBits < 9);
			_readBits<byte>(nBits, nCntBits); 
		}
		virtual void readBits(uint16&  nBits, size_t nCntBits)
		{
			assert(nCntBits < 17);
			_readBits<uint16>(nBits, nCntBits);
		}
		virtual void readBits(uint32&  nBits, size_t nCntBits)
		{
			assert(nCntBits < 33);
			_readBits<uint32>(nBits, nCntBits); 
		}
		virtual void readBits(uint64&  nBits, size_t nCntBits)
		{
			assert(nCntBits < 65);
			_readBits<uint64>(nBits, nCntBits); 
		}
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


	class IBitReadStream
	{
	public:
		IBitReadStream(){}
		virtual ~IBitReadStream(){}
		virtual bool readBit() = 0;
		virtual void readBit(byte & nBit) = 0;
		virtual void readBits(byte& nBits, size_t nCntBits) = 0;
		virtual void readBits(uint16&  nBits, size_t nCntBits) = 0;
		virtual void readBits(uint32&  nBits, size_t nCntBits) = 0;
		virtual void readBits(uint64&  nBits, size_t nCntBits) = 0;
	};


	class IReadBitStreamBase : public IBitReadStream
	{
	public:


		IReadBitStreamBase()  {}
		virtual ~IReadBitStreamBase()  {}


		virtual void readBits(byte& nBits, size_t nCntBits)
		{
			assert(nCntBits < 9);
			_readBits<byte>(nBits, nCntBits); 
		}
		virtual void readBits(uint16&  nBits, size_t nCntBits)
		{
			assert(nCntBits < 17);
			_readBits<uint16>(nBits, nCntBits);
		}
		virtual void readBits(uint32&  nBits, size_t nCntBits)
		{
			assert(nCntBits < 33);
			_readBits<uint32>(nBits, nCntBits); 
		}
		virtual void readBits(uint64&  nBits, size_t nCntBits)
		{
			assert(nCntBits < 65);
			_readBits<uint64>(nBits, nCntBits); 
		}
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

	class FxBitWriteStream : public BitStreamBase, public 
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



	class FxBitReadStream : public BitStreamBase , public IReadBitStreamBase
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
		

	};
}
#endif