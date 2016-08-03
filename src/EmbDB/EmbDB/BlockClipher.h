#ifndef _EMBEDDED_DATABASE_BLOCK_CLIPHER_H_
#define _EMBEDDED_DATABASE_BLOCK_CLIPHER_H_


namespace embDB
{

	class IBlockCipher
	{
	public:
		IBlockCipher(){}
		virtual ~IBlockCipher(){}
		virtual bool encrypt(byte* pBuf, uint32 len) = 0;
		virtual bool decrypt(byte* pBuf, uint32 len) = 0;
		virtual bool encrypt(byte* pSrcBuf, byte* pDstBuf, uint32 len) = 0;
		virtual bool decrypt(byte* pSrcBuf, byte* pDstBuf, uint32 len) = 0;
		virtual 
	};
}
#endif