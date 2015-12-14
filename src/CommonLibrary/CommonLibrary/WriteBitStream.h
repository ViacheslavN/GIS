#ifndef _EMBEDDED_COMMON_I_WRITE_BIT_STERAM_H_
#define _EMBEDDED_COMMON_I_WRITE_BIT_STERAM_H_

namespace CommonLib
{
	class WriteBitStream : public BitStreamBase, public IWriteBitStreamBase
	{
	public:
		WriteBitStream(alloc_t *pAlloc = NULL);
		~WriteBitStream();
		virtual void writeBit(bool bBit);
	private:


	};

}

#endif