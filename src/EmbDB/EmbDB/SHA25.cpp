#include "stdafx.h"
#include "SHA25.h"


#define SHA2_SHFR(x, n)    (x >> n)
//#define SHA2_ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define SHA2_ROTL(x, n)   ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define SHA2_CH(x, y, z)  ((x & y) ^ (~x & z))
#define SHA2_MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))



template <class T> inline T SHA2_ROTR(T x, unsigned int y)
{
	// Portable rotate that reduces to single instruction...
	// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=57157,
	// https://software.intel.com/en-us/forums/topic/580884 
	// and https://llvm.org/bugs/show_bug.cgi?id=24226
	static const unsigned int THIS_SIZE = sizeof(T)*8;
	static const unsigned int MASK = THIS_SIZE-1;
	assert(y < THIS_SIZE);
	return T((x >> y)|(x<<(-y&MASK)));
}


namespace embDB
{
	const uint32 SHA256::K[64] = { 0x428a2f98, 0x71374491, 0xb5c0fbcf,
		0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74,
		0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
		0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc,
		0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
		0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85,
		0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb,
		0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70,
		0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3,
		0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f,
		0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7,
		0xc67178f2 };

	const uint32 SHA256::HH[8] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372,
		0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };


	SHA256::SHA256()
	{

	}
	SHA256::~SHA256()
	{

	}

	void SHA256::getHash(byte* pData, uint32 nLen)
	{
		int intLen = getIntCount(nLen);
		byte* byteBuff = new byte[intLen * 4];
		uint32* intBuff = new uint32[intLen];

		calculateHash(pData, nLen, byteBuff, intBuff);
	}


	int SHA256::getIntCount(int byteCount) const
	{
		return ((byteCount + 9 + 63) / 64) * 16;
	}

	void SHA256::calculateHash(byte* pData, uint32 nLen,
		byte* byteBuff, uint32* intBuff)
	{
		int intLen = getIntCount(nLen);
		memcpy(byteBuff, pData, nLen);
		byteBuff[nLen] = (byte) 0x80;
		
 

		memset(byteBuff + nLen + 1, 0, intLen * 4 - nLen + 1);

		for (int i = 0, j = 0; j < intLen; i += 4, j++)
		{
			intBuff[j] = readInt(byteBuff, i);
		}
		intBuff[intLen - 2] = nLen >> 29;
		intBuff[intLen - 1] = nLen << 3;
		 
		memcpy(hh, HH, 8*sizeof(uint32));
		for (int block = 0; block < intLen; block += 16) {
			for (int i = 0; i < 16; i++) {
				w[i] = intBuff[block + i];
			}
			for (int i = 16; i < 64; i++) {
				uint32   x = w[i - 2];
				uint32 theta1 = SHA2_ROTR(x, 17) ^ SHA2_ROTR(x, 19) ^ (x >> 10);
				x = w[i - 15];
				uint32 theta0 = SHA2_ROTR(x, 7) ^ SHA2_ROTR(x, 18) ^ (x >> 3);
				w[i] = theta1 + w[i - 7] + theta0 + w[i - 16];
			}

			uint32 a = hh[0], b = hh[1], c = hh[2], d = hh[3];
			uint32 e = hh[4], f = hh[5], g = hh[6], h = hh[7];

			for (int i = 0; i < 64; i++) {
				uint32 t1 = h + (SHA2_ROTR(e, 6) ^ SHA2_ROTR(e, 11) ^ SHA2_ROTR(e, 25))
					+ ((e & f) ^ ((~e) & g)) + K[i] + w[i];
				uint32 t2 = (SHA2_ROTR(a, 2) ^ SHA2_ROTR(a, 13) ^ SHA2_ROTR(a, 22))
					+ ((a & b) ^ (a & c) ^ (b & c));
				h = g;
				g = f;
				f = e;
				e = d + t1;
				d = c;
				c = b;
				b = a;
				a = t1 + t2;
			}
			hh[0] += a;
			hh[1] += b;
			hh[2] += c;
			hh[3] += d;
			hh[4] += e;
			hh[5] += f;
			hh[6] += g;
			hh[7] += h;
		}
		for (int i = 0; i < 8; i++) {
			writeInt(result, i * 4, hh[i]);
		}
	}


 

	uint32 SHA256::readInt(byte* b, int32 i) 
	{
		return ((b[i] & 0xff) << 24) + ((b[i + 1] & 0xff) << 16)
			+ ((b[i + 2] & 0xff) << 8) + (b[i + 3] & 0xff);
	}

	 void SHA256::writeInt(byte* b, int i, uint32 value)
	 {
		b[i] = (byte) (value >> 24);
		b[i + 1] = (byte) (value >> 16);
		b[i + 2] = (byte) (value >> 8);
		b[i + 3] = (byte) value;
	}

}