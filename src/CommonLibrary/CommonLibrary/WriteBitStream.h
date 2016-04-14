#ifndef _EMBEDDED_COMMON_I_WRITE_BIT_STERAM_H_
#define _EMBEDDED_COMMON_I_WRITE_BIT_STERAM_H_

namespace CommonLib
{
	class WriteBitStream : public TBaseBitMemryStream<IWriteBitStreamBase>
	{
	public:
		typedef TBaseBitMemryStream<IWriteBitStreamBase> TBase;
		WriteBitStream(alloc_t *pAlloc = NULL);
		~WriteBitStream();
		virtual void writeBit(bool bBit);
		virtual bool resize(uint32 nSize);
	private:


	};

}

#endif