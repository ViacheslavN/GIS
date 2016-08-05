#include "stdafx.h"
#include "BaseHash.h"

namespace embDB
{
	namespace Crypto
	{
		CHashBase& CHashBase::reset ()
		{
			m_state = ADD;
			m_buf.clear ();
			init_sum ();
			return *this;
		}
		/*CHashBase& CHashBase::add (byte *pData, uint32 nSize)
		{
			if (ADD != m_state)
				reset ();


			if (nSize == 0)
				return *this;
			std::size_t const blksize = blocksize ();
			if (m_buf.size () > 0 && m_buf.size () < blksize) {
		 
				uint32 n = min (nSize, blksize - m_buf.size ());
				m_buf.push_back (pData, n);
				nSize -= n;
				m_nLen += n;
			}
			if (m_buf.size () == blksize)
			{
			 
				update_sum (m_buf.buffer(), m_buf.size());
				m_buf.clear ();
			}
			if (nSize == 0)
				return *this;

			for (; s + blksize < e; s += blksize) {
				std::string::const_iterator t = s;
				update_sum (t);
				mbuf.clear ();
				mlen += blksize;
			}
			mbuf.assign (s, e);
			mlen += mbuf.size ();
			return *this;
 
		}*/
		CHashBase& CHashBase::finish ()
		{
			return *this;
		}

	}
}