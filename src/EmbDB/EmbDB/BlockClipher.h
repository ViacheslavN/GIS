#ifndef _EMBEDDED_DATABASE_BLOCK_CLIPHER_H_
#define _EMBEDDED_DATABASE_BLOCK_CLIPHER_H_


#include "CommonLibrary/general.h"
#include "CommonLibrary/alloc_t.h"

namespace embDB
{

#define  CIPHER_FILE_BLOCK_SIZE    16

	class IBlockCipher
	{
	public:
		IBlockCipher(){}
		virtual ~IBlockCipher(){}
		virtual bool setKey(byte *pKey, uint32 nLen) = 0;
		virtual bool encrypt(byte* pPlain, uint32 len) = 0;
		virtual bool decrypt(byte* pPlain, uint32 len) = 0;
		virtual bool encrypt(byte* pPlain, byte* pDst, uint32 len) = 0;
		virtual bool decrypt(byte* pPlain, byte* pDst, uint32 len) = 0;

		virtual uint32 getBlockSIze() const = 0;

		virtual uint32 getKeyLength() const  = 0;
 
	};
}
#endif