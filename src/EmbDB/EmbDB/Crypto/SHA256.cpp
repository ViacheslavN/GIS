#include "stdafx.h"
#include "SHA256.h"

namespace embDB
{
	namespace Crypto
	{

		static inline void
			unpack_big_endian (byte *pData, uint32 const i, uint32 const x)
		{
			pData[i + 0] = (x >> 24) & 0xff;    
			pData[i + 1] = (x >> 16) & 0xff;
			pData[i + 2] = (x >>  8) & 0xff;
			pData[i + 3] = x & 0xff;
		}

		static inline uint32
			rotate_right (uint32 const x, uint32 const n)
		{
			return (x >> n) | (x << (32 - n));
		}

		static inline uint32
			ch (uint32 const x, uint32 const y, uint32 const z)
		{
			return (x & y) ^ ((~x) & z);
		}

		static inline uint32
			ma (uint32 const x, uint32 const y, uint32 const z)
		{
			return (x & y) ^ (x & z) ^ (y & z);
		}

		static inline uint32
			gamma0 (uint32 const x)
		{
			return rotate_right (x, 7) ^ rotate_right (x, 18) ^ (x >> 3);
		}

		static inline uint32
			gamma1 (uint32 const x)
		{
			return rotate_right (x, 17) ^ rotate_right (x, 19) ^ (x >> 10);
		}

		static inline uint32
			sigma0 (uint32 const x)
		{
			return rotate_right (x, 2) ^ rotate_right (x, 13) ^ rotate_right (x, 22);
		}

		static inline uint32
			sigma1 (uint32 const x)
		{
			return rotate_right (x, 6) ^ rotate_right (x, 11) ^ rotate_right (x, 25);
		}

		static inline void
			round (
			uint32 const a, uint32 const b, uint32 const c,
			uint32& d,
			uint32 const e, uint32 const f, uint32 const g,
			uint32& h,
			uint32 const k, uint32 const w)
		{
			uint32 const t0 = h + sigma1 (e) + ch (e, f, g) + k + w;
			uint32 const t1 = sigma0 (a) + ma (a, b, c);
			d += t0;
			h = t0 + t1;
		}

		CSHA256::CSHA256(CommonLib::alloc_t *pAlloc) : CBaseHash(pAlloc)
		{ 
		}
		CSHA256::~CSHA256()
		{

		}
		bool CSHA256::digest (byte *pData, uint32 nSize)
		{
			if(nSize < 32)
				return false;

			finish ();
			uint32* p = m_sum;
			for (uint32 i = 0; i < 32; i += 4)
				unpack_big_endian (pData, i, *p++);
			return true;
		}
		uint32 CSHA256::blocksize () const
		{
			return 64;
		}

		void CSHA256::init_sum ()
		{
			m_sum[0] = 0x6a09e667; m_sum[1] = 0xbb67ae85; m_sum[2] = 0x3c6ef372;
			m_sum[3] = 0xa54ff53a; m_sum[4] = 0x510e527f; m_sum[5] = 0x9b05688c;
			m_sum[6] = 0x1f83d9ab; m_sum[7] = 0x5be0cd19;

		}

		void CSHA256::update_sum (byte *pData)
		{
			static const uint32 K[64] = {
				0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
				0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
				0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
				0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
				0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
				0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
				0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
				0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
				0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
				0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
				0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
				0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
				0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
				0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
				0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
				0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
			};
			uint32 w[64];
			uint32 a = m_sum[0], b = m_sum[1], c = m_sum[2], d = m_sum[3];
			uint32 e = m_sum[4], f = m_sum[5], g = m_sum[6], h = m_sum[7];
			for (uint32 i = 0; i < 16U; i++)
				w[i] = pData[i << 2] << 24
				| pData[(i << 2) + 1] << 16
				| pData[(i << 2) + 2] <<  8
				| pData[(i << 2) + 3] ;
			for (uint32 i = 16U; i < 64U; i++)
				w[i] = gamma1 (w[i - 2]) + w[i - 7] + gamma0 (w[i - 15]) + w[i - 16];
			for (uint32 i = 0; i < 64; i += 8) {
				round (a, b, c, d, e, f, g, h, K[i + 0], w[i + 0]);
				round (h, a, b, c, d, e, f, g, K[i + 1], w[i + 1]);
				round (g, h, a, b, c, d, e, f, K[i + 2], w[i + 2]);
				round (f, g, h, a, b, c, d, e, K[i + 3], w[i + 3]);
				round (e, f, g, h, a, b, c, d, K[i + 4], w[i + 4]);
				round (d, e, f, g, h, a, b, c, K[i + 5], w[i + 5]);
				round (c, d, e, f, g, h, a, b, K[i + 6], w[i + 6]);
				round (b, c, d, e, f, g, h, a, K[i + 7], w[i + 7]);
			}
			m_sum[0] += a; m_sum[1] += b; m_sum[2] += c; m_sum[3] += d;
			m_sum[4] += e; m_sum[5] += f; m_sum[6] += g; m_sum[7] += h;
		}
		void CSHA256::last_sum ()
		{
			m_buf.push_back (0x80);
			uint32 n = (m_buf.size () + 8U + 64U - 1U) / 64U * 64U;
			m_buf.reserve_fill (n, 0);
			unpack_big_endian (m_buf.buffer(), n - 8, m_nLen >> 29);
			unpack_big_endian (m_buf.buffer(), n - 4, m_nLen <<  3);
 
			for (uint32 i = 0; i < n; i += 64U)
				update_sum (m_buf.buffer() + i);
		}

	}
}