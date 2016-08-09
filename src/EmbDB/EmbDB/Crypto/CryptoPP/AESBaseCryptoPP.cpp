#include "stdafx.h"
#include "AESBaseCryptoPP.h"

namespace embDB
{

	namespace Crypto
	{
		namespace CryptoPPWrap
		{
			CAESBase::CAESBase()
			{
				memset(iv, 0, sizeof(iv));
			 
			}
			CAESBase::~CAESBase()
			{

			}
			
			bool CAESBase::setEncryptKey(byte *pKey, uint32 nSize)
			{
				if(nSize < getKeyLength())
					return false;
				m_pEncryption.reset( new CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption(pKey, getKeyLength(), iv));
				return true;
			}
			bool CAESBase::setDecryptKey(byte *pKey, uint32 nSize)
			{
				if(nSize < getKeyLength())
					return false;

				m_pDecryption.reset( new CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption(pKey, getKeyLength(), iv));
				return true;
			}
			bool CAESBase::encrypt(byte* pPlain, uint32 len)
			{
				if(!m_pEncryption.get())
					return false;

				m_pEncryption->ProcessData((byte*)pPlain, (byte*)pPlain, len);
				return true;
			}
			bool CAESBase::decrypt(byte* pPlain, uint32 len)
			{
				if(!m_pDecryption.get())
					return false;

				m_pDecryption->ProcessData((byte*)pPlain, (byte*)pPlain, len);
				return true;
			}
			bool CAESBase::encrypt(byte* pPlain, byte* pSecret, uint32 len)
			{
				if(!m_pEncryption.get())
					return false;

				m_pEncryption->ProcessData((byte*)pSecret, (byte*)pPlain, len);
				return true;
			}
			bool CAESBase::decrypt(byte* pSecret, byte* pPlain, uint32 len)
			{
				if(!m_pDecryption.get())
					return false;

				m_pDecryption->ProcessData((byte*)pPlain, (byte*)pSecret, len);
				return true;
			}
		}
	}
}