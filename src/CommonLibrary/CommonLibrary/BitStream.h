#ifndef _EMBEDDED_COMMON_I_BIT_STERAM_H_
#define _EMBEDDED_COMMON_I_BIT_STERAM_H_

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
		virtual void readBit(byte & nBit) = 0;
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




}
#endif