#include "stdafx.h"
#include "PageCipher.h"
 
#include "PBKDF2.h"

#ifdef _USE_CRYPTOPP_LIB_
	#include "Crypto/CryptoPP/AES128.h"
	#include "Crypto/CryptoPP/AES256.h"
#else
	#include "Crypto/AES128.h"
	#include "Crypto/AES256.h"
#endif


namespace embDB
{


	CPageCipher::CPageCipher(QryptoALG qryptoAlg) : m_qryptoAlg(qryptoAlg)
	{
		 CreateCiphers();
	}
	CPageCipher::~CPageCipher()
	{

	}

	void CPageCipher::SetKey(const byte* pPWD, uint32 nLenPwd, const byte* pSalt, const byte* pIVSalt, uint32 nLenSalt)
	{
 

		if(!m_pCipher.get())
			return;


		m_vecInitVector.resize(m_pCipher->getBlockSize());
		std::vector<byte> chipKey(m_pCipher->getKeyLength());
		std::vector<byte> chipInitKey(m_pCipher->getKeyLength());
 

		CPBKDF2::PBKDF2(pPWD, nLenPwd, pSalt, nLenSalt, &chipKey[0], m_pCipher->getKeyLength(), 6400);
		CPBKDF2::PBKDF2(pPWD, nLenPwd, pIVSalt, nLenSalt, &chipInitKey[0], m_pCipher->getKeyLength(), 6400);

		m_pCipher->setEncryptKey(&chipKey[0], m_pCipher->getKeyLength());
		m_pCipher->setDecryptKey(&chipKey[0], m_pCipher->getKeyLength());
	
		m_pCipherForInitVector->setEncryptKey(&chipInitKey[0], m_pCipherForInitVector->getKeyLength());
		m_pCipherForInitVector->setDecryptKey(&chipInitKey[0], m_pCipherForInitVector->getKeyLength());


	}
	void CPageCipher::CreateCiphers()
	{


#ifdef _USE_CRYPTOPP_LIB_
		switch(m_qryptoAlg)
		{
		case AES128:
			m_pCipher.reset(new Crypto::CryptoPPWrap::CAES128());
			m_pCipherForInitVector.reset(new Crypto::CryptoPPWrap::CAES128());
			break;
		case AES256:
			m_pCipher.reset(new Crypto::CryptoPPWrap::CAES256());
			m_pCipherForInitVector.reset(new Crypto::CryptoPPWrap::CAES256());
			break;
		}

#else
		switch(m_qryptoAlg)
		{
			case AES128:
				m_pCipher.reset(new Crypto::CAES128());
				m_pCipherForInitVector.reset(new Crypto::CAES128());
				break;
			case AES256:
				m_pCipher.reset(new Crypto::CAES256());
				m_pCipherForInitVector.reset(new Crypto::CAES256());
				break;
		}

#endif
	
	}

	void CPageCipher::xorInitVector(byte* b, int off, int len, int64 pos)
	{
 
		uint32 nBlockSIze = m_pCipherForInitVector->getBlockSize();
		while (len > 0)
		{
			for (int i = 0; i < nBlockSIze; i += 8)
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
			m_pCipherForInitVector->encrypt(&m_vecInitVector[0], &m_vecInitVector[0], nBlockSIze);
			for (int i = 0; i < nBlockSIze; i++)
			{
				b[off + i] ^= m_vecInitVector[i];
			}
			pos += nBlockSIze;
			off += nBlockSIze;
			len -= nBlockSIze;
		}
	}

	bool CPageCipher::encrypt(CFilePage *pFilePage)
	{

		assert(m_pCipher.get());

		xorInitVector(pFilePage->getRowData(), 0, pFilePage->getPageSize(), pFilePage->getAddr());
		m_pCipher->encrypt(pFilePage->getRowData(), pFilePage->getPageSize());

		return true;
	}
	bool CPageCipher::decrypt(CFilePage *pFilePage)
	{
		assert(m_pCipher.get());

		m_pCipher->decrypt(pFilePage->getRowData(), pFilePage->getPageSize());
		xorInitVector(pFilePage->getRowData(), 0, pFilePage->getPageSize(), pFilePage->getAddr());
		return true;
	}
	bool CPageCipher::encrypt(CFilePage *pFilePage, byte* pDstBuf, uint32 len)
	{
		assert(m_pCipher.get());

		memcpy(pDstBuf, pFilePage->getRowData(), pFilePage->getPageSize());

		xorInitVector(pDstBuf, 0, pFilePage->getPageSize(), pFilePage->getAddr());
		m_pCipher->encrypt(pDstBuf, pFilePage->getPageSize());
		return true;
	}
	bool CPageCipher::decrypt(CFilePage *pFilePage, byte* pDstBuf, uint32 len)
	{
		assert(m_pCipher.get());

		memcpy(pDstBuf, pFilePage->getRowData(), pFilePage->getPageSize());
		m_pCipher->decrypt(pDstBuf, pFilePage->getPageSize());
		xorInitVector(pDstBuf, 0, pFilePage->getPageSize(), pFilePage->getAddr());
		return true;
	}
}