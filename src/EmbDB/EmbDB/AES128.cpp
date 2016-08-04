#include "stdafx.h"
#include "AES128.h"

namespace embDB
{


	CAES128::CAES128()
	{
#ifndef _USE_CRYPTOPP_LIB_
		init();
#endif
	}
	CAES128::~CAES128()
	{

	}

	  static uint32 rot8(uint32 x) {
		return (x >> 8) | (x << 24);
	}

	 static uint32 xtime(uint32 x) {
		return ((x << 1) ^ (((x & 0x80) != 0) ? 0x1b : 0)) & 255;
	}

	 static uint32 mul(uint32* pow, uint32* log, uint32 x, uint32 y) {
		return (x != 0 && y != 0) ? pow[(log[x] + log[y]) % 255] : 0;
	}


#ifndef _USE_CRYPTOPP_LIB_

	 uint32 CAES128::getDec(uint32 t)
	 {
		 return RT0[FS[(t >> 24) & 255]] ^ RT1[FS[(t >> 16) & 255]]
		 ^ RT2[FS[(t >> 8) & 255]] ^ RT3[FS[t & 255]];
	 }

	void CAES128::init()
	{
		uint32 pow[256];
		uint32 log[256];
		for (uint32 i = 0, x = 1; i < 256; i++, x ^= xtime(x)) {
			pow[i] = x;
			log[x] = i;
		}
		for (uint32 i = 0, x = 1; i < 10; i++, x = xtime(x)) {
			RCON[i] = x << 24;
		}
		FS[0x00] = 0x63;
		RS[0x63] = 0x00;
		for (uint32 i = 1; i < 256; i++) {
			uint32 x = pow[255 - log[i]], y = x;
			y = ((y << 1) | (y >> 7)) & 255;
			x ^= y;
			y = ((y << 1) | (y >> 7)) & 255;
			x ^= y;
			y = ((y << 1) | (y >> 7)) & 255;
			x ^= y;
			y = ((y << 1) | (y >> 7)) & 255;
			x ^= y ^ 0x63;
			FS[i] = x & 255;
			RS[x] = i & 255;
		}
		for (uint32 i = 0; i < 256; i++) {
			uint32 x = FS[i], y = xtime(x);
			FT0[i] = (x ^ y) ^ (x << 8) ^ (x << 16) ^ (y << 24);
			FT1[i] = rot8(FT0[i]);
			FT2[i] = rot8(FT1[i]);
			FT3[i] = rot8(FT2[i]);
			y = RS[i];
			RT0[i] = mul(pow, log, 0x0b, y) ^ (mul(pow, log, 0x0d, y) << 8)
				^ (mul(pow, log, 0x09, y) << 16) ^ (mul(pow, log, 0x0e, y) << 24);
			RT1[i] = rot8(RT0[i]);
			RT2[i] = rot8(RT1[i]);
			RT3[i] = rot8(RT2[i]);
		}
	}
#endif
	bool CAES128::setKey(byte *key, uint32 nLen)
	{
		if(nLen < getKeyLength())
			return false;

#ifndef _USE_CRYPTOPP_LIB_
		for (uint32 i = 0, j = 0; i < 4; i++) {
			encKey[i] = decKey[i] = ((key[j++] & 255) << 24)
				| ((key[j++] & 255) << 16) | ((key[j++] & 255) << 8)
				| (key[j++] & 255);
		}
		uint32 e = 0;
		for (uint32 i = 0; i < 10; i++, e += 4) {
			encKey[e + 4] = encKey[e] ^ RCON[i]
			^ (FS[(encKey[e + 3] >> 16) & 255] << 24)
				^ (FS[(encKey[e + 3] >> 8) & 255] << 16)
				^ (FS[(encKey[e + 3]) & 255] << 8)
				^ FS[(encKey[e + 3] >> 24) & 255];
			encKey[e + 5] = encKey[e + 1] ^ encKey[e + 4];
			encKey[e + 6] = encKey[e + 2] ^ encKey[e + 5];
			encKey[e + 7] = encKey[e + 3] ^ encKey[e + 6];
		}
		uint32 d = 0;
		decKey[d++] = encKey[e++];
		decKey[d++] = encKey[e++];
		decKey[d++] = encKey[e++];
		decKey[d++] = encKey[e++];
		for (uint32 i = 1; i < 10; i++) {
			e -= 8;
			decKey[d++] = getDec(encKey[e++]);
			decKey[d++] = getDec(encKey[e++]);
			decKey[d++] = getDec(encKey[e++]);
			decKey[d++] = getDec(encKey[e++]);
		}
		e -= 8;
		decKey[d++] = encKey[e++];
		decKey[d++] = encKey[e++];
		decKey[d++] = encKey[e++];
		decKey[d] = encKey[e];
#else
		memcpy( key, key, CryptoPP::AES::DEFAULT_KEYLENGTH );
		memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );
#endif
		return true;
	}
	bool CAES128::encrypt(byte* pBuf, uint32 len)
	{
#ifndef _USE_CRYPTOPP_LIB_
		for (uint32 i = 0; i <  len; i += 16) 
		{
			encryptBlock(pBuf, pBuf, i);
		}
#else
		CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfbEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH, iv);
		cfbEncryption.ProcessData((byte*)pBuf, (byte*)pBuf, len);
#endif

	 
		return true;
	}
	bool CAES128::decrypt(byte* pBuf, uint32 len)
	{
#ifndef _USE_CRYPTOPP_LIB_
		for (uint32 i = 0; i < len; i += 16) {
			decryptBlock(pBuf, pBuf, i);
		}
#else
		CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cfbDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH, iv);
		cfbDecryption.ProcessData((byte*)pBuf, (byte*)pBuf, len);
#endif
		return true;
	}


	bool CAES128::encrypt(byte* pBuf, byte* pDst, uint32 len)
	{
#ifndef _USE_CRYPTOPP_LIB_
		for (uint32 i = 0; i <  len; i += 16) 
		{
			encryptBlock(pBuf, pDst, i);
		}
#else
		CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfbEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH, iv);
		cfbEncryption.ProcessData((byte*)pDst, (byte*)pBuf, len);
#endif
		return true;
	}
	bool CAES128::decrypt(byte* pBuf, byte* pDst, uint32 len)
	{
#ifndef _USE_CRYPTOPP_LIB_
		for (uint32 i = 0; i < len; i += 16) 
		{
			decryptBlock(pBuf, pDst, i);
		}
#else
		CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cfbDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH, iv);
		cfbDecryption.ProcessData((byte*)pDst, (byte*)pBuf, len);
#endif
		return true;
	}


#ifndef _USE_CRYPTOPP_LIB_
	void CAES128::encryptBlock(byte*in, byte* out, uint32 off)
	{
		uint32* k = &encKey[0];
		uint32 x0 = ((in[off] << 24) | ((in[off + 1] & 255) << 16)
			| ((in[off + 2] & 255) << 8) | (in[off + 3] & 255)) ^ k[0];
		uint32 x1 = ((in[off + 4] << 24) | ((in[off + 5] & 255) << 16)
			| ((in[off + 6] & 255) << 8) | (in[off + 7] & 255)) ^ k[1];
		uint32 x2 = ((in[off + 8] << 24) | ((in[off + 9] & 255) << 16)
			| ((in[off + 10] & 255) << 8) | (in[off + 11] & 255)) ^ k[2];
		uint32 x3 = ((in[off + 12] << 24) | ((in[off + 13] & 255) << 16)
			| ((in[off + 14] & 255) << 8) | (in[off + 15] & 255)) ^ k[3];
		uint32 y0 = FT0[(x0 >> 24) & 255] ^ FT1[(x1 >> 16) & 255]
		^ FT2[(x2 >> 8) & 255] ^ FT3[x3 & 255] ^ k[4];
		uint32 y1 = FT0[(x1 >> 24) & 255] ^ FT1[(x2 >> 16) & 255]
		^ FT2[(x3 >> 8) & 255] ^ FT3[x0 & 255] ^ k[5];
		uint32 y2 = FT0[(x2 >> 24) & 255] ^ FT1[(x3 >> 16) & 255]
		^ FT2[(x0 >> 8) & 255] ^ FT3[x1 & 255] ^ k[6];
		uint32 y3 = FT0[(x3 >> 24) & 255] ^ FT1[(x0 >> 16) & 255]
		^ FT2[(x1 >> 8) & 255] ^ FT3[x2 & 255] ^ k[7];
		x0 = FT0[(y0 >> 24) & 255] ^ FT1[(y1 >> 16) & 255]
		^ FT2[(y2 >> 8) & 255] ^ FT3[y3 & 255] ^ k[8];
		x1 = FT0[(y1 >> 24) & 255] ^ FT1[(y2 >> 16) & 255]
		^ FT2[(y3 >> 8) & 255] ^ FT3[y0 & 255] ^ k[9];
		x2 = FT0[(y2 >> 24) & 255] ^ FT1[(y3 >> 16) & 255]
		^ FT2[(y0 >> 8) & 255] ^ FT3[y1 & 255] ^ k[10];
		x3 = FT0[(y3 >> 24) & 255] ^ FT1[(y0 >> 16) & 255]
		^ FT2[(y1 >> 8) & 255] ^ FT3[y2 & 255] ^ k[11];
		y0 = FT0[(x0 >> 24) & 255] ^ FT1[(x1 >> 16) & 255]
		^ FT2[(x2 >> 8) & 255] ^ FT3[x3 & 255] ^ k[12];
		y1 = FT0[(x1 >> 24) & 255] ^ FT1[(x2 >> 16) & 255]
		^ FT2[(x3 >> 8) & 255] ^ FT3[x0 & 255] ^ k[13];
		y2 = FT0[(x2 >> 24) & 255] ^ FT1[(x3 >> 16) & 255]
		^ FT2[(x0 >> 8) & 255] ^ FT3[x1 & 255] ^ k[14];
		y3 = FT0[(x3 >> 24) & 255] ^ FT1[(x0 >> 16) & 255]
		^ FT2[(x1 >> 8) & 255] ^ FT3[x2 & 255] ^ k[15];
		x0 = FT0[(y0 >> 24) & 255] ^ FT1[(y1 >> 16) & 255]
		^ FT2[(y2 >> 8) & 255] ^ FT3[y3 & 255] ^ k[16];
		x1 = FT0[(y1 >> 24) & 255] ^ FT1[(y2 >> 16) & 255]
		^ FT2[(y3 >> 8) & 255] ^ FT3[y0 & 255] ^ k[17];
		x2 = FT0[(y2 >> 24) & 255] ^ FT1[(y3 >> 16) & 255]
		^ FT2[(y0 >> 8) & 255] ^ FT3[y1 & 255] ^ k[18];
		x3 = FT0[(y3 >> 24) & 255] ^ FT1[(y0 >> 16) & 255]
		^ FT2[(y1 >> 8) & 255] ^ FT3[y2 & 255] ^ k[19];
		y0 = FT0[(x0 >> 24) & 255] ^ FT1[(x1 >> 16) & 255]
		^ FT2[(x2 >> 8) & 255] ^ FT3[x3 & 255] ^ k[20];
		y1 = FT0[(x1 >> 24) & 255] ^ FT1[(x2 >> 16) & 255]
		^ FT2[(x3 >> 8) & 255] ^ FT3[x0 & 255] ^ k[21];
		y2 = FT0[(x2 >> 24) & 255] ^ FT1[(x3 >> 16) & 255]
		^ FT2[(x0 >> 8) & 255] ^ FT3[x1 & 255] ^ k[22];
		y3 = FT0[(x3 >> 24) & 255] ^ FT1[(x0 >> 16) & 255]
		^ FT2[(x1 >> 8) & 255] ^ FT3[x2 & 255] ^ k[23];
		x0 = FT0[(y0 >> 24) & 255] ^ FT1[(y1 >> 16) & 255]
		^ FT2[(y2 >> 8) & 255] ^ FT3[y3 & 255] ^ k[24];
		x1 = FT0[(y1 >> 24) & 255] ^ FT1[(y2 >> 16) & 255]
		^ FT2[(y3 >> 8) & 255] ^ FT3[y0 & 255] ^ k[25];
		x2 = FT0[(y2 >> 24) & 255] ^ FT1[(y3 >> 16) & 255]
		^ FT2[(y0 >> 8) & 255] ^ FT3[y1 & 255] ^ k[26];
		x3 = FT0[(y3 >> 24) & 255] ^ FT1[(y0 >> 16) & 255]
		^ FT2[(y1 >> 8) & 255] ^ FT3[y2 & 255] ^ k[27];
		y0 = FT0[(x0 >> 24) & 255] ^ FT1[(x1 >> 16) & 255]
		^ FT2[(x2 >> 8) & 255] ^ FT3[x3 & 255] ^ k[28];
		y1 = FT0[(x1 >> 24) & 255] ^ FT1[(x2 >> 16) & 255]
		^ FT2[(x3 >> 8) & 255] ^ FT3[x0 & 255] ^ k[29];
		y2 = FT0[(x2 >> 24) & 255] ^ FT1[(x3 >> 16) & 255]
		^ FT2[(x0 >> 8) & 255] ^ FT3[x1 & 255] ^ k[30];
		y3 = FT0[(x3 >> 24) & 255] ^ FT1[(x0 >> 16) & 255]
		^ FT2[(x1 >> 8) & 255] ^ FT3[x2 & 255] ^ k[31];
		x0 = FT0[(y0 >> 24) & 255] ^ FT1[(y1 >> 16) & 255]
		^ FT2[(y2 >> 8) & 255] ^ FT3[y3 & 255] ^ k[32];
		x1 = FT0[(y1 >> 24) & 255] ^ FT1[(y2 >> 16) & 255]
		^ FT2[(y3 >> 8) & 255] ^ FT3[y0 & 255] ^ k[33];
		x2 = FT0[(y2 >> 24) & 255] ^ FT1[(y3 >> 16) & 255]
		^ FT2[(y0 >> 8) & 255] ^ FT3[y1 & 255] ^ k[34];
		x3 = FT0[(y3 >> 24) & 255] ^ FT1[(y0 >> 16) & 255]
		^ FT2[(y1 >> 8) & 255] ^ FT3[y2 & 255] ^ k[35];
		y0 = FT0[(x0 >> 24) & 255] ^ FT1[(x1 >> 16) & 255]
		^ FT2[(x2 >> 8) & 255] ^ FT3[x3 & 255] ^ k[36];
		y1 = FT0[(x1 >> 24) & 255] ^ FT1[(x2 >> 16) & 255]
		^ FT2[(x3 >> 8) & 255] ^ FT3[x0 & 255] ^ k[37];
		y2 = FT0[(x2 >> 24) & 255] ^ FT1[(x3 >> 16) & 255]
		^ FT2[(x0 >> 8) & 255] ^ FT3[x1 & 255] ^ k[38];
		y3 = FT0[(x3 >> 24) & 255] ^ FT1[(x0 >> 16) & 255]
		^ FT2[(x1 >> 8) & 255] ^ FT3[x2 & 255] ^ k[39];
		x0 = ((FS[(y0 >> 24) & 255] << 24) | (FS[(y1 >> 16) & 255] << 16)
			| (FS[(y2 >> 8) & 255] << 8) | FS[y3 & 255]) ^ k[40];
		x1 = ((FS[(y1 >> 24) & 255] << 24) | (FS[(y2 >> 16) & 255] << 16)
			| (FS[(y3 >> 8) & 255] << 8) | FS[y0 & 255]) ^ k[41];
		x2 = ((FS[(y2 >> 24) & 255] << 24) | (FS[(y3 >> 16) & 255] << 16)
			| (FS[(y0 >> 8) & 255] << 8) | FS[y1 & 255]) ^ k[42];
		x3 = ((FS[(y3 >> 24) & 255] << 24) | (FS[(y0 >> 16) & 255] << 16)
			| (FS[(y1 >> 8) & 255] << 8) | FS[y2 & 255]) ^ k[43];
		out[off] = (byte) (x0 >> 24); out[off+1] = (byte) (x0 >> 16);
		out[off+2] = (byte) (x0 >> 8); out[off+3] = (byte) x0;
		out[off+4] = (byte) (x1 >> 24); out[off+5] = (byte) (x1 >> 16);
		out[off+6] = (byte) (x1 >> 8); out[off+7] = (byte) x1;
		out[off+8] = (byte) (x2 >> 24); out[off+9] = (byte) (x2 >> 16);
		out[off+10] = (byte) (x2 >> 8); out[off+11] = (byte) x2;
		out[off+12] = (byte) (x3 >> 24); out[off+13] = (byte) (x3 >> 16);
		out[off+14] = (byte) (x3 >> 8); out[off+15] = (byte) x3;
	}

	void CAES128::decryptBlock(byte *in, byte* out, uint32 off)
	{
		uint32* k = &decKey[0];
		uint32 x0 = ((in[off] << 24) | ((in[off + 1] & 255) << 16)
			| ((in[off + 2] & 255) << 8) | (in[off + 3] & 255)) ^ k[0];
		uint32 x1 = ((in[off + 4] << 24) | ((in[off + 5] & 255) << 16)
			| ((in[off + 6] & 255) << 8) | (in[off + 7] & 255)) ^ k[1];
		uint32 x2 = ((in[off + 8] << 24) | ((in[off + 9] & 255) << 16)
			| ((in[off + 10] & 255) << 8) | (in[off + 11] & 255)) ^ k[2];
		uint32 x3 = ((in[off + 12] << 24) | ((in[off + 13] & 255) << 16)
			| ((in[off + 14] & 255) << 8) | (in[off + 15] & 255)) ^ k[3];
		uint32 y0 = RT0[(x0 >> 24) & 255] ^ RT1[(x3 >> 16) & 255]
		^ RT2[(x2 >> 8) & 255] ^ RT3[x1 & 255] ^ k[4];
		uint32 y1 = RT0[(x1 >> 24) & 255] ^ RT1[(x0 >> 16) & 255]
		^ RT2[(x3 >> 8) & 255] ^ RT3[x2 & 255] ^ k[5];
		uint32 y2 = RT0[(x2 >> 24) & 255] ^ RT1[(x1 >> 16) & 255]
		^ RT2[(x0 >> 8) & 255] ^ RT3[x3 & 255] ^ k[6];
		uint32 y3 = RT0[(x3 >> 24) & 255] ^ RT1[(x2 >> 16) & 255]
		^ RT2[(x1 >> 8) & 255] ^ RT3[x0 & 255] ^ k[7];
		x0 = RT0[(y0 >> 24) & 255] ^ RT1[(y3 >> 16) & 255]
		^ RT2[(y2 >> 8) & 255] ^ RT3[y1 & 255] ^ k[8];
		x1 = RT0[(y1 >> 24) & 255] ^ RT1[(y0 >> 16) & 255]
		^ RT2[(y3 >> 8) & 255] ^ RT3[y2 & 255] ^ k[9];
		x2 = RT0[(y2 >> 24) & 255] ^ RT1[(y1 >> 16) & 255]
		^ RT2[(y0 >> 8) & 255] ^ RT3[y3 & 255] ^ k[10];
		x3 = RT0[(y3 >> 24) & 255] ^ RT1[(y2 >> 16) & 255]
		^ RT2[(y1 >> 8) & 255] ^ RT3[y0 & 255] ^ k[11];
		y0 = RT0[(x0 >> 24) & 255] ^ RT1[(x3 >> 16) & 255]
		^ RT2[(x2 >> 8) & 255] ^ RT3[x1 & 255] ^ k[12];
		y1 = RT0[(x1 >> 24) & 255] ^ RT1[(x0 >> 16) & 255]
		^ RT2[(x3 >> 8) & 255] ^ RT3[x2 & 255] ^ k[13];
		y2 = RT0[(x2 >> 24) & 255] ^ RT1[(x1 >> 16) & 255]
		^ RT2[(x0 >> 8) & 255] ^ RT3[x3 & 255] ^ k[14];
		y3 = RT0[(x3 >> 24) & 255] ^ RT1[(x2 >> 16) & 255]
		^ RT2[(x1 >> 8) & 255] ^ RT3[x0 & 255] ^ k[15];
		x0 = RT0[(y0 >> 24) & 255] ^ RT1[(y3 >> 16) & 255]
		^ RT2[(y2 >> 8) & 255] ^ RT3[y1 & 255] ^ k[16];
		x1 = RT0[(y1 >> 24) & 255] ^ RT1[(y0 >> 16) & 255]
		^ RT2[(y3 >> 8) & 255] ^ RT3[y2 & 255] ^ k[17];
		x2 = RT0[(y2 >> 24) & 255] ^ RT1[(y1 >> 16) & 255]
		^ RT2[(y0 >> 8) & 255] ^ RT3[y3 & 255] ^ k[18];
		x3 = RT0[(y3 >> 24) & 255] ^ RT1[(y2 >> 16) & 255]
		^ RT2[(y1 >> 8) & 255] ^ RT3[y0 & 255] ^ k[19];
		y0 = RT0[(x0 >> 24) & 255] ^ RT1[(x3 >> 16) & 255]
		^ RT2[(x2 >> 8) & 255] ^ RT3[x1 & 255] ^ k[20];
		y1 = RT0[(x1 >> 24) & 255] ^ RT1[(x0 >> 16) & 255]
		^ RT2[(x3 >> 8) & 255] ^ RT3[x2 & 255] ^ k[21];
		y2 = RT0[(x2 >> 24) & 255] ^ RT1[(x1 >> 16) & 255]
		^ RT2[(x0 >> 8) & 255] ^ RT3[x3 & 255] ^ k[22];
		y3 = RT0[(x3 >> 24) & 255] ^ RT1[(x2 >> 16) & 255]
		^ RT2[(x1 >> 8) & 255] ^ RT3[x0 & 255] ^ k[23];
		x0 = RT0[(y0 >> 24) & 255] ^ RT1[(y3 >> 16) & 255]
		^ RT2[(y2 >> 8) & 255] ^ RT3[y1 & 255] ^ k[24];
		x1 = RT0[(y1 >> 24) & 255] ^ RT1[(y0 >> 16) & 255]
		^ RT2[(y3 >> 8) & 255] ^ RT3[y2 & 255] ^ k[25];
		x2 = RT0[(y2 >> 24) & 255] ^ RT1[(y1 >> 16) & 255]
		^ RT2[(y0 >> 8) & 255] ^ RT3[y3 & 255] ^ k[26];
		x3 = RT0[(y3 >> 24) & 255] ^ RT1[(y2 >> 16) & 255]
		^ RT2[(y1 >> 8) & 255] ^ RT3[y0 & 255] ^ k[27];
		y0 = RT0[(x0 >> 24) & 255] ^ RT1[(x3 >> 16) & 255]
		^ RT2[(x2 >> 8) & 255] ^ RT3[x1 & 255] ^ k[28];
		y1 = RT0[(x1 >> 24) & 255] ^ RT1[(x0 >> 16) & 255]
		^ RT2[(x3 >> 8) & 255] ^ RT3[x2 & 255] ^ k[29];
		y2 = RT0[(x2 >> 24) & 255] ^ RT1[(x1 >> 16) & 255]
		^ RT2[(x0 >> 8) & 255] ^ RT3[x3 & 255] ^ k[30];
		y3 = RT0[(x3 >> 24) & 255] ^ RT1[(x2 >> 16) & 255]
		^ RT2[(x1 >> 8) & 255] ^ RT3[x0 & 255] ^ k[31];
		x0 = RT0[(y0 >> 24) & 255] ^ RT1[(y3 >> 16) & 255]
		^ RT2[(y2 >> 8) & 255] ^ RT3[y1 & 255] ^ k[32];
		x1 = RT0[(y1 >> 24) & 255] ^ RT1[(y0 >> 16) & 255]
		^ RT2[(y3 >> 8) & 255] ^ RT3[y2 & 255] ^ k[33];
		x2 = RT0[(y2 >> 24) & 255] ^ RT1[(y1 >> 16) & 255]
		^ RT2[(y0 >> 8) & 255] ^ RT3[y3 & 255] ^ k[34];
		x3 = RT0[(y3 >> 24) & 255] ^ RT1[(y2 >> 16) & 255]
		^ RT2[(y1 >> 8) & 255] ^ RT3[y0 & 255] ^ k[35];
		y0 = RT0[(x0 >> 24) & 255] ^ RT1[(x3 >> 16) & 255]
		^ RT2[(x2 >> 8) & 255] ^ RT3[x1 & 255] ^ k[36];
		y1 = RT0[(x1 >> 24) & 255] ^ RT1[(x0 >> 16) & 255]
		^ RT2[(x3 >> 8) & 255] ^ RT3[x2 & 255] ^ k[37];
		y2 = RT0[(x2 >> 24) & 255] ^ RT1[(x1 >> 16) & 255]
		^ RT2[(x0 >> 8) & 255] ^ RT3[x3 & 255] ^ k[38];
		y3 = RT0[(x3 >> 24) & 255] ^ RT1[(x2 >> 16) & 255]
		^ RT2[(x1 >> 8) & 255] ^ RT3[x0 & 255] ^ k[39];
		x0 = ((RS[(y0 >> 24) & 255] << 24) | (RS[(y3 >> 16) & 255] << 16)
			| (RS[(y2 >> 8) & 255] << 8) | RS[y1 & 255]) ^ k[40];
		x1 = ((RS[(y1 >> 24) & 255] << 24) | (RS[(y0 >> 16) & 255] << 16)
			| (RS[(y3 >> 8) & 255] << 8) | RS[y2 & 255]) ^ k[41];
		x2 = ((RS[(y2 >> 24) & 255] << 24) | (RS[(y1 >> 16) & 255] << 16)
			| (RS[(y0 >> 8) & 255] << 8) | RS[y3 & 255]) ^ k[42];
		x3 = ((RS[(y3 >> 24) & 255] << 24) | (RS[(y2 >> 16) & 255] << 16)
			| (RS[(y1 >> 8) & 255] << 8) | RS[y0 & 255]) ^ k[43];
		out[off] = (byte) (x0 >> 24);
		out[off + 1] = (byte) (x0 >> 16);
		out[off+2] = (byte) (x0 >> 8); out[off+3] = (byte) x0;
		out[off+4] = (byte) (x1 >> 24); out[off+5] = (byte) (x1 >> 16);
		out[off+6] = (byte) (x1 >> 8); out[off+7] = (byte) x1;
		out[off+8] = (byte) (x2 >> 24); out[off+9] = (byte) (x2 >> 16);
		out[off+10] = (byte) (x2 >> 8); out[off+11] = (byte) x2;
		out[off+12] = (byte) (x3 >> 24); out[off+13] = (byte) (x3 >> 16);
		out[off+14] = (byte) (x3 >> 8); out[off+15] = (byte) x3;
	}
#endif
}