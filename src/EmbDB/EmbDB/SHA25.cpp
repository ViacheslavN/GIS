#include "stdafx.h"
#include "SHA25.h"


#define SHA2_SHFR(x, n)    (x >> n)
//#define SHA2_ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define SHA2_ROTL(x, n)   ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define SHA2_CH(x, y, z)  ((x & y) ^ (~x & z))
#define SHA2_MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#ifdef _ANDROID
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

#else
#define SHA2_ROTR _rotr

#endif
namespace embDB
{
	const uint32 SHA256::k[64] = { 0x428a2f98, 0x71374491, 0xb5c0fbcf,
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

	// Little endian constants
	//{0x982f8a42, 0x91443771, 0xcffbc0b5, 0xa5dbb5e9, 0x5bc25639, 0xf111f159, 0xa4823f92, 0xd55e1cab, //
	// 0x98aa07d8, 0x015b8312, 0xbe853124, 0xc37d0c55, 0x745dbe72, 0xfeb1de80, 0xa706dc9b, 0x74f19bc1, //
	// 0xc1699be4, 0x8647beef, 0xc69dc10f, 0xcca10c24, 0x6f2ce92d, 0xaa84744a, 0xdca9b05c, 0xda88f976, //
	// 0x52513e98, 0x6dc631a8, 0xc82703b0, 0xc77f59bf, 0xf30be0c6, 0x4791a7d5, 0x5163ca06, 0x67292914, //
	// 0x850ab727, 0x38211b2e, 0xfc6d2c4d, 0x130d3853, 0x54730a65, 0xbb0a6a76, 0x2ec9c281, 0x852c7292, //
	// 0xa1e8bfa2, 0x4b661aa8, 0x708b4bc2, 0xa3516cc7, 0x19e892d1, 0x240699d6, 0x85350ef4, 0x70a06a10, //
	// 0x16c1a419, 0x086c371e, 0x4c774827, 0xb5bcb034, 0xb30c1c39, 0x4aaad84e, 0x4fca9c5b, 0xf36f2e68, //
	// 0xee828f74, 0x6f63a578, 0x1478c884, 0x0802c78c, 0xfaffbe90, 0xeb6c50a4, 0xf7a3f9be, 0xf27871c6  //
	//};

 

	SHA256::SHA256(CommonLib::alloc_t *pAlloc) : m_pAlloc(pAlloc)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;
	}
	SHA256::~SHA256()
	{

	}

	void SHA256::getHash(byte* pData, uint32 nLen, byte result[32] )
	{

#ifdef _USE_CRYPTOPP_LIB_
		CryptoPP::SHA256 hashSha256;
		hashSha256.CalculateDigest(result, pData, nLen);
#else

		s_hashValues hash;
		byte* ppMsg = NULL;
		uint32 ppMsgLen = preProcess(ppMsg, pData, nLen);
		uint32 nbChunks = ppMsgLen >> 6 ; 
 
		uint32 szSchedulerArray[256];
		for (uint32 i = 0; i < nbChunks; ++i)
		{
			scheduler(&ppMsg[i << 6], szSchedulerArray);
			compressor(szSchedulerArray, hash);
		}
		m_pAlloc->free(ppMsg);

		// Return appended hash in big-endian
		for (int i = 0; i < 4; ++i)
		{
			result[0  + i] = ((byte*)&hash.h0)[3 - i];
			result[4  + i] = ((byte*)&hash.h1)[3 - i];
			result[8  + i] = ((byte*)&hash.h2)[3 - i];
			result[12 + i] = ((byte*)&hash.h3)[3 - i];
			result[16 + i] = ((byte*)&hash.h4)[3 - i];
			result[20 + i] = ((byte*)&hash.h5)[3 - i];
			result[24 + i] = ((byte*)&hash.h6)[3 - i];
			result[28 + i] = ((byte*)&hash.h7)[3 - i];
		}

#endif
	}

	uint32 SHA256::preProcess(byte* &ppMessage, byte* message, uint32 len)
	{
		//+1 byte for the appended 1 at the end of the message.s
		uint32 paddingSize = (((len + 1) % 64) > 56) ? (56 + (64 - ((len) % 64))) : 56 - ((len) % 64); // TODO: Clean this up and optimize
		uint32 ppMessageLength = len + paddingSize;

		// Append 1 as the first bit, then fill the padding with 0s (+ padding for bitlength over 64bits)
	//	ppMessage = (byte*)calloc(ppMessageLength + 8, 1);

		ppMessage = (byte*)m_pAlloc->alloc(ppMessageLength + 8);
		memset(ppMessage, 0, ppMessageLength + 8);

		memcpy(ppMessage, message, len);
		ppMessage[len] = 0x80;

		// Make the bit length fit on two unsigned longs (64bit);
		uint32 msgBitLen[2] = {0, 0};
		if ((len * 8) > 0xFFFFFFF) ++msgBitLen[1]; msgBitLen[0] += (len * 8);

		// At the end of the padding, add the bit length (big endian) of the original message.
		for (int i = 0; i < 4; ++i)
		{
			(ppMessage + ppMessageLength)[i]	 = (msgBitLen[1] >> ((3 - i) << 3)) & 0xFF;
			(ppMessage + ppMessageLength)[i + 4] = (msgBitLen[0] >> ((3 - i) << 3)) & 0xFF;
		}

		return (ppMessageLength + 8);
	} 


	void SHA256::scheduler(byte* messageChunk, uint32* schedulerArray)
	{
		// The scheduler array.
		// Copy over message chunk to first 16 words, switch endianness in the scheduler array (make big-endian)
		for (int i = 0; i < 16; ++i)
		{
			schedulerArray[i] = messageChunk[(i << 2)    ] << 24 |
				messageChunk[(i << 2) + 1] << 16 |
				messageChunk[(i << 2) + 2] <<  8 |
				messageChunk[(i << 2) + 3];
		}

		uint32 s0, s1;			// Scheduler temporary variables
		for (uint32 i = 16; i < 64; ++i)
		{
			s0 = (SHA2_ROTR(schedulerArray[i-15], 7) ^ (SHA2_ROTR(schedulerArray[i-15], 18)) ^ (schedulerArray[i-15] >> 3));
			s1 = (SHA2_ROTR(schedulerArray[i-2], 17) ^ (SHA2_ROTR(schedulerArray[i-2],  19)) ^ (schedulerArray[i-2] >> 10));
			schedulerArray[i] = schedulerArray[i-16] + s0 + schedulerArray[i-7] + s1;
		}
	}

	void SHA256::compressor(uint32* schedulerArray, s_hashValues &hValues)
	{
		// Compression algorithm temporary variables.
		uint32 a = hValues.h0;
		uint32 b = hValues.h1;
		uint32 c = hValues.h2;
		uint32 d = hValues.h3;
		uint32 e = hValues.h4;
		uint32 f = hValues.h5;
		uint32 g = hValues.h6;
		uint32 h = hValues.h7;

		// Declare temporary work variables
		uint32 S1, ch, temp1, S0, maj, temp2;
		for (uint32 i = 0; i < 64; ++i)
		{
			// Main compression function algorithm.
			S1 = SHA2_ROTR(e, 6) ^ SHA2_ROTR(e, 11) ^ SHA2_ROTR(e, 25);
			ch = (e & f) ^ ((~e) & g);
			temp1 = h + S1 + ch + k[i] + schedulerArray[i];
			S0 = SHA2_ROTR(a, 2) ^ SHA2_ROTR(a, 13) ^ SHA2_ROTR(a, 22);
			maj = (a & b) ^ (a & c) ^ (b & c);
			temp2 = S0 + maj;

			// Affect working variables.
			h = g;
			g = f;
			f = e;
			e = d + temp1;
			d = c;
			c = b;
			b = a;
			a = temp1 + temp2;
		}

		// Update hashing values with current chunk compression.
		hValues.h0 += a;
		hValues.h1 += b;
		hValues.h2 += c;
		hValues.h3 += d;
		hValues.h4 += e;
		hValues.h5 += f;
		hValues.h6 += g;
		hValues.h7 += h;
	}
 

	 

}