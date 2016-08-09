#ifndef _EMBEDDED_DATABASE_HMAC_H_
#define _EMBEDDED_DATABASE_HMAC_H_

#include "CommonLibrary/general.h"
#include "CommonLibrary/blob.h"
#include "BaseHash.h"

namespace embDB
{
	namespace Crypto
	{
		template<class HASH >
		class HMAC : public CBaseHash 
		{
			HASH m_ihash;
			HASH m_ohash;
			CommonLib::CBlob m_key;
		public:
			HMAC (const byte* key, uint32 nSize, CommonLib::alloc_t *pAlloc = NULL) : CBaseHash (pAlloc), m_ihash (pAlloc), m_ohash (pAlloc), m_key(pAlloc)
			{

				m_key.copy(key, nSize);
			}

			virtual void digest (CommonLib::CBlob& blob) 
			{ 
				blob.resize(m_ohash.length());
				digest (blob.buffer(), blob.size()); 
			}
			virtual bool digest (byte *pData, uint32 nSize) 
			{ 
				finish ();
				return m_ohash.digest (pData, nSize); 
			}
			uint32 blocksize () const { return m_ihash.blocksize (); }
			virtual uint32 length() const {return m_ihash.length();}
			void	reset ()
			{
				uint32 const blksize = blocksize ();
				if (m_key.size () > blksize) {
					HASH khash;
					khash.add(m_key);
					khash.digest (m_key);
				}
				if (m_key.size () < blksize)
					m_key.resize_fill(blksize, 0);
				CommonLib::CBlob kipad;
				kipad.copy(m_key.buffer(), m_key.size());
				for (std::size_t i = 0; i < blksize; ++i)
					kipad[i] ^= 0x36;
				m_ihash.reset ();
				m_ihash.add (kipad);
				m_state = ADD;
			}

			void add (const byte *pDate, uint32 nSize)
			{
				if (ADD != m_state)
					reset ();
				m_ihash.add (pDate, nSize);
			}

			void add (const CommonLib::CBlob& blob)
			{
				add (blob.buffer(), blob.size());
			}

			void	finish (void)
			{
				if (FINISH == m_state)
					return;
				if (ADD != m_state)
					reset ();
				m_state = FINISH;
				CommonLib::CBlob kopad;
				kopad.copy(m_key.buffer(), m_key.size());
			 
				std::size_t const blksize = blocksize ();
				for (std::size_t i = 0; i < blksize; ++i)
					kopad[i] ^= 0x5c;
				m_ohash.reset ();
				m_ohash.add (kopad);

				CommonLib::CBlob hash;
				m_ihash.digest (hash);
				m_ohash.add (hash);
			}

		protected:
			void init_sum () {}
			void update_sum (const byte*) {}
			void last_sum () {}
		};
	}
}

#endif