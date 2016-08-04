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
		virtual bool setKey(byte *pKey, uint32 nLen) = 0;
		virtual bool encrypt(byte* pBuf, uint32 len) = 0;
		virtual bool decrypt(byte* pBuf, uint32 len) = 0;
		virtual bool encrypt(byte* pBuf, byte* pDst, uint32 len) = 0;
		virtual bool decrypt(byte* pBuf, byte* pDst, uint32 len) = 0;
		virtual int getKeyLength() const  = 0;
 
	};
}
#endif