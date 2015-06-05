#ifndef _EMBEDDED_COMMON_I_FIXED_MEMORY_STERAM_H_
#define _EMBEDDED_COMMON_I_FIXED_MEMORY_STERAM_H_

#include "general.h"
#include "alloc_t.h"
#include "Stream.h"
namespace CommonLib
{

	class FxStreamBase : public IStream
	{

		public:

			FxStreamBase(alloc_t *pAlloc = NULL);
			~FxStreamBase();


			virtual void create(size_t nSize);
			virtual void attach(byte* pBuffer, size_t nSize);
			virtual byte* deattach();
			virtual byte* buffer();
			virtual size_t size() const;
			virtual bool seek(size_t position, enSeekOffset offset );
			virtual size_t pos() const;
			virtual void reset();
			virtual void close();


			template <typename T>
			void readT(T& val)
			{

				if(m_bIsBigEndian)
					read_inverse((byte*)&val, sizeof(T));
				else
					read_bytes((byte*)&val, sizeof(T));
			}

			template <typename T>
			void writeT(T value)
			{
				if(m_bIsBigEndian)
					write_inverse((byte*)&value, sizeof(T));
				else
					write_bytes((byte*)&value, sizeof(T));
			}

			template <typename T>
			T readTR()
			{
				T ret;
				if(m_bIsBigEndian)
					read_inverse((byte*)&ret, sizeof(T));
				else
					read_bytes((byte*)&ret, sizeof(T));
				return ret;
			}

		protected:
			void read_bytes(byte* dst, size_t size);
			void read_inverse(byte* buffer, size_t size);
			void write_bytes(const byte* buffer, size_t size);
			void write_inverse(const byte* buffer, size_t size);
		
	    protected:
			byte* m_pBuffer;
			size_t  m_nPos;
			size_t  m_nSize;
			bool m_bIsBigEndian;
			alloc_t *m_pAlloc;
			bool m_bAttach;
	};
	class FxMemoryReadStream : public FxStreamBase, public IReadStream
	{
	public:

		FxMemoryReadStream(alloc_t *pAlloc = NULL);
		~FxMemoryReadStream();

		virtual void read( byte* pBuffer, size_t bufLen );
		virtual void read(bool& value);
		virtual void read(byte& value) ;
 		virtual void read(int16& value);
		virtual void read(uint16& value);
		virtual void read(uint32& value);
		virtual void read(int32& value);
		virtual void read(int64& value);
		virtual void read(uint64& value);
		virtual void read(float& value);
		virtual void read(double& value);
		virtual void read(CommonLib::str_t& str);

		virtual bool         readBool();
		virtual byte         readByte();
		virtual int16		 readint16();
		virtual uint16       readintu16();
		virtual uint32       readDword();
		virtual int32        readInt32();
		virtual uint32       readIntu32();
		virtual int64        readInt64();
		virtual uint64       readIntu64();
		virtual float        readFloat();
		virtual double       readDouble();
	private:
	
	};

	class FxMemoryWriteStream : public FxStreamBase, public IWriteStream
	{
	public:
		FxMemoryWriteStream(alloc_t *pAlloc = NULL);
		~FxMemoryWriteStream();

		virtual void write(const byte* pBuffer, size_t bufLen );
		virtual void write(bool value);
		virtual void write(byte value);
		virtual void write(int16 value);
		virtual void write(uint16 value);
		virtual void write(uint32 value);
		virtual void write(int32 value);
		virtual void write(int64 value);
		virtual void write(uint64 value);
		virtual void write(float value);
		virtual void write(double value);
		virtual void write(const CommonLib::str_t& str);
	};

}

#endif