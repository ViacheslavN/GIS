#include "stdafx.h"
#include "PageCipher.h"
#include "AES128.h"
#include "PBKDF2.h"
namespace embDB
{


	CPageCipher::CPageCipher(byte* pPWD, uint32 nLenPwd, byte* pSalt,  byte* pIVSalt, uint32 nLenSalt, QryptoALG qryptoAlg) : m_qryptoAlg(qryptoAlg)
	{
		init(pPWD, nLenPwd, pSalt, pIVSalt, nLenSalt);
	}
	CPageCipher::~CPageCipher()
	{

	}

	void CPageCipher::init(byte* pPWD, uint32 nLenPwd, byte* pSalt, byte* pIVSalt, uint32 nLenSalt)
	{
		CreateCiphers();

		if(!m_pCipher.get())
			return;



		byte chipKey[32];
		byte chipInitKey[32];

		CPBKDF2::PBKDF2(pPWD, nLenPwd, pSalt, nLenSalt, chipKey, 32, 1000);
		CPBKDF2::PBKDF2(pPWD, nLenPwd, pIVSalt, nLenSalt, chipInitKey, 32, 1000);

		m_pCipher->setKey(chipKey, 32);
		m_pCipher->setKey(chipInitKey, 32);


	}
	void CPageCipher::CreateCiphers()
	{
		if(m_qryptoAlg != AES128_ALG)
			return;

		m_pCipher.reset(new CAES128());
		m_pCipherForInitVector.reset(new CAES128());
	}

	void CPageCipher::xorInitVector(byte* b, int off, int len, int64 pos)
	{
 
		while (len > 0)
		{
			for (int i = 0; i < CIPHER_FILE_BLOCK_SIZE; i += 8)
			{
				int64 block = (pos + i) >> 3;
				m_vecInitVector[i] = (byte) (block >> 56);
				m_vecInitVector[i + 1] = (byte) (block >> 48);
				m_vecInitVector[i + 2] = (byte) (block >> 40);
				m_vecInitVector[i + 3] = (byte) (block >> 32);
				m_vecInitVector[i + 4] = (byte) (block >> 24);
				m_vecInitVector[i + 5] = (byte) (block >> 16);
				m_vecInitVector[i + 6] = (byte) (block >> 8);
				m_vecInitVector[i + 7] = (byte) block;
			}
			m_pCipherForInitVector->encrypt(&m_vecInitVector[0], &m_vecInitVector[0], CIPHER_FILE_BLOCK_SIZE);
			for (int i = 0; i < CIPHER_FILE_BLOCK_SIZE; i++)
			{
				b[off + i] ^= m_vecInitVector[i];
			}
			pos += CIPHER_FILE_BLOCK_SIZE;
			off += CIPHER_FILE_BLOCK_SIZE;
			len -= CIPHER_FILE_BLOCK_SIZE;
		}
	}

	bool CPageCipher::encrypt(CFilePage *pFilePage)
	{

		xorInitVector(pFilePage->getRowData(), 0, pFilePage->getPageSize(), pFilePage->getAddr());
		m_pCipher->encrypt(pFilePage->getRowData(), pFilePage->getPageSize());

		return true;
	}
	bool CPageCipher::decrypt(CFilePage *pFilePage)
	{
		m_pCipher->decrypt(pFilePage->getRowData(), pFilePage->getPageSize());
		xorInitVector(pFilePage->getRowData(), 0, pFilePage->getPageSize(), pFilePage->getAddr());
		return true;
	}
	bool CPageCipher::encrypt(CFilePage *pFilePage, byte* pDstBuf, uint32 len)
	{
		memcpy(pDstBuf, pFilePage->getRowData(), pFilePage->getPageSize());

		xorInitVector(pDstBuf, 0, pFilePage->getPageSize(), pFilePage->getAddr());
		m_pCipher->encrypt(pDstBuf, pFilePage->getPageSize());
		return true;
	}
	bool CPageCipher::decrypt(CFilePage *pFilePage, byte* pDstBuf, uint32 len)
	{
		memcpy(pDstBuf, pFilePage->getRowData(), pFilePage->getPageSize());
		m_pCipher->decrypt(pDstBuf, pFilePage->getPageSize());
		xorInitVector(pDstBuf, 0, pFilePage->getPageSize(), pFilePage->getAddr());
		return true;
	}
}