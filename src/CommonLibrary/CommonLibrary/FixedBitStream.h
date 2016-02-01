#ifndef _EMBEDDED_COMMON_I_FIXED_BIT_STERAM_H_
#define _EMBEDDED_COMMON_I_FIXED_BIT_STERAM_H_

#include "general.h"
#include "alloc_t.h"
#include "Stream.h"
#include "BitStream.h"
namespace CommonLib
{

	class FxBitWriteStream : public TBaseBitMemryStream<IWriteBitStreamBase>
	{
	public:
		typedef TBaseBitMemryStream<IWriteBitStreamBase> TBase;
		FxBitWriteStream(alloc_t *pAlloc = NULL);
		~FxBitWriteStream();
		virtual void writeBit(bool bBit);
	private:
		

	};



	class FxBitReadStream: public TBaseBitMemryStream<IReadBitStreamBase>
	{
	public:
		typedef  TBaseBitMemryStream<IReadBitStreamBase> TBase;
		FxBitReadStream(alloc_t *pAlloc = NULL);
		~FxBitReadStream();

		virtual bool readBit();
	private:
		

	};
}
#endif