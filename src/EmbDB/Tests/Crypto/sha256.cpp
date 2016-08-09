#include "stdafx.h"

#include "../../EmbDB/Crypto/SHA256.h"


 

#include <iostream>
#include <string.h>
using namespace std;
#define uchar unsigned char
#define uint unsigned long

struct s_hashValues
{
	uint h0;
	uint h1;
	uint h2;
	uint h3;
	uint h4;
	uint h5;
	uint h6;
	uint h7;
};

uint k[64] = 
	// Big endian constants
{0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

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


/*
 * Desc: This method prepares the message for hashing.
 *
 * PSEUDO CODE:
 * Pre-processing:
 * append the bit '1' to the message
 * append k bits '0', where k is the minimum number >= 0 such that the resulting message
 *   length (modulo 512 in bits) is 448.
 * append length of message (before pre-processing), in bits, as 64-bit big-endian integer
 * 
 */
uint preProcess(uchar* &ppMessage, uchar* message, uint len)
{
	//+1 byte for the appended 1 at the end of the message.s
	uint paddingSize = (((len + 1) % 64) > 56) ? (56 + (64 - ((len) % 64))) : 56 - ((len) % 64); // TODO: Clean this up and optimize
	uint ppMessageLength = len + paddingSize;
	
	// Append 1 as the first bit, then fill the padding with 0s (+ padding for bitlength over 64bits)
	ppMessage = (uchar*)calloc(ppMessageLength + 8, 1);
	memcpy(ppMessage, message, len);
	ppMessage[len] = 0x80;

	// Make the bit length fit on two unsigned longs (64bit);
	uint msgBitLen[2] = {0, 0};
	if ((len * 8) > 0xFFFFFFF) ++msgBitLen[1]; msgBitLen[0] += (len * 8);

	// At the end of the padding, add the bit length (big endian) of the original message.
	for (int i = 0; i < 4; ++i)
	{
		(ppMessage + ppMessageLength)[i]	 = (msgBitLen[1] >> ((3 - i) << 3)) & 0xFF;
		(ppMessage + ppMessageLength)[i + 4] = (msgBitLen[0] >> ((3 - i) << 3)) & 0xFF;
	}

	return (ppMessageLength + 8);
} 

void scheduler(uchar* messageChunk, uint* schedulerArray)
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

	uint s0, s1;			// Scheduler temporary variables
	for (uint i = 16; i < 64; ++i)
	{
		s0 = (_rotr(schedulerArray[i-15], 7) ^ (_rotr(schedulerArray[i-15], 18)) ^ (schedulerArray[i-15] >> 3));
		s1 = (_rotr(schedulerArray[i-2], 17) ^ (_rotr(schedulerArray[i-2],  19)) ^ (schedulerArray[i-2] >> 10));
		schedulerArray[i] = schedulerArray[i-16] + s0 + schedulerArray[i-7] + s1;
	}
}

void compressor(uint* schedulerArray, s_hashValues &hValues)
{
	// Compression algorithm temporary variables.
	uint a = hValues.h0;
	uint b = hValues.h1;
	uint c = hValues.h2;
	uint d = hValues.h3;
	uint e = hValues.h4;
	uint f = hValues.h5;
	uint g = hValues.h6;
	uint h = hValues.h7;
	
	// Declare temporary work variables
	uint S1, ch, temp1, S0, maj, temp2;
	for (uint i = 0; i < 64; ++i)
	{
		// Main compression function algorithm.
		S1 = _rotr(e, 6) ^ _rotr(e, 11) ^ _rotr(e, 25);
		ch = (e & f) ^ ((~e) & g);
		temp1 = h + S1 + ch + k[i] + schedulerArray[i];
		S0 = _rotr(a, 2) ^ _rotr(a, 13) ^ _rotr(a, 22);
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

/*
 * Desc: This method Hashes a message using the SHA256 method.
 *
 * Input:	unsigned char finalHash[32]:
 *				array to contain the final hash (string format)
 *			unsigned char* message:
 *				pointer to the message to hash (data or string)
 *			int len:
 *				Length of the message passed as input
 *
 * Output:	
 *
 */
void SHA256(uchar finalHash[32], uchar* message, uint len)
{
	// Initialize the hash values
	s_hashValues hash;

	// Big-endian constants
	hash.h0 = 0x6a09e667;
	hash.h1 = 0xbb67ae85;
	hash.h2 = 0x3c6ef372;
	hash.h3 = 0xa54ff53a;
	hash.h4 = 0x510e527f;
	hash.h5 = 0x9b05688c;
	hash.h6 = 0x1f83d9ab;
	hash.h7 = 0x5be0cd19;

	// Little-endian constants
	//hash.h0 = 0x67e6096a;
	//hash.h1 = 0x85ae67bb;
	//hash.h2 = 0x72f36e3c;
	//hash.h3 = 0x3af54fa5;
	//hash.h4 = 0x7f520e51;
	//hash.h5 = 0x8c68059b;
	//hash.h6 = 0xabd9831f;
	//hash.h7 = 0x19cde05b;

	// Preprocess the message
	uchar* ppMsg = NULL;
	uint ppMsgLen = preProcess(ppMsg, message, len);
	uint nbChunks = ppMsgLen >> 6 ;	// Devide by 64bytes (512bits).

	// Initialize the scheduler array
	uint* schedulerArray = (uint*) malloc(256);

	// For each message chunk, go through the scheduler and compressor
	for (uint i = 0; i < nbChunks; ++i)
	{
		scheduler(&ppMsg[i << 6], schedulerArray);
		compressor(schedulerArray, hash);
	}

	// Cleanup
	free(schedulerArray);
	free(ppMsg);

	// Return appended hash in big-endian
	for (int i = 0; i < 4; ++i)
	{
		finalHash[0  + i] = ((uchar*)&hash.h0)[3 - i];
		finalHash[4  + i] = ((uchar*)&hash.h1)[3 - i];
		finalHash[8  + i] = ((uchar*)&hash.h2)[3 - i];
		finalHash[12 + i] = ((uchar*)&hash.h3)[3 - i];
		finalHash[16 + i] = ((uchar*)&hash.h4)[3 - i];
		finalHash[20 + i] = ((uchar*)&hash.h5)[3 - i];
		finalHash[24 + i] = ((uchar*)&hash.h6)[3 - i];
		finalHash[28 + i] = ((uchar*)&hash.h7)[3 - i];
	}
}

void displayHash(uchar hash[32])
{
	
	
	for (int i = 0; i < 32; ++i)
	{
		printf("%x", hash[i]);
	}

	printf("\n");
}

void TestSha256()
{
	byte str[] = "The quick brown fox jumps over the lazy dog";
	uchar finalHash[32];
	embDB::Crypto::CSHA256 sha256;
	sha256.add((byte*)str, sizeof(str) - 1);
	sha256.digest(finalHash, 32);
	displayHash(finalHash);


	uchar test[] = "The quick brown fox jumps over the lazy dog";
	 
	SHA256(finalHash, test, sizeof(test) - 1);

	displayHash(finalHash);

	int dd = 0;
	dd++;


}