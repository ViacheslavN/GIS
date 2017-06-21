#ifndef _EMBEDDED_DATABASE_BLOCK_CLIPHER_H_
#define _EMBEDDED_DATABASE_BLOCK_CLIPHER_H_


#include "CommonLibrary/general.h"
#include "CommonLibrary/alloc_t.h"

namespace embDB
{

 

	class IBlockCipher
	{
	public:
		IBlockCipher(){}
		virtual ~IBlockCipher(){}
		//virtual bool setKey(byte *pKey, uint32 nLen) = 0;

		virtual bool setEncryptKey(byte *pKey, uint32 nSize) = 0;
		virtual bool setDecryptKey(byte *pKey, uint32 nSize) = 0;
		virtual bool encrypt(byte* pPlain, uint32 len) = 0;
		virtual bool decrypt(byte* pSecret, uint32 len) = 0;
		virtual bool encrypt(byte* pPlain, byte* pSecret, uint32 len) = 0;
		virtual bool decrypt(byte* pSecret, byte* pPlain, uint32 len) = 0;

		virtual uint32 getBlockSize() const = 0;
		virtual uint32 getKeyLength() const  = 0;
 
	};
}
#endif