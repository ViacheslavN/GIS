#include "stdafx.h"
#include "BaseHash.h"

namespace embDB
{
	namespace Crypto
	{
		CBaseHash& CBaseHash::reset ()
		{
			m_state = ADD;
			m_buf.clear ();
			m_nLen = 0;
			init_sum ();
			return *this;
		}
		CBaseHash& CBaseHash::add (byte *pData, uint32 nSize)
		{
			if (ADD != m_state)
				reset ();


			if (nSize == 0)
				return *this;
			uint32 nPos = 0;

			std::size_t const blksize = blocksize ();
			if (m_buf.size () > 0 && m_buf.size () < blksize) {
		 
				uint32 n = min (nSize, blksize - m_buf.size ());
				m_buf.push_back (pData, n);
				nPos += n;
				m_nLen += n;
			}
			if (m_buf.size () == blksize)
			{
				update_sum (m_buf.buffer());
				m_buf.clear ();
			}
			if (nSize == 0)
				return *this;

			for (; nPos + blksize < nSize; nPos += blksize)
			{
 				update_sum (pData);
				m_buf.clear ();
				m_nLen += blksize;
			}
			m_buf.copy (pData, nSize);
			m_nLen += m_buf.size ();
			return *this;
 
		}
		CBaseHash& CBaseHash::finish ()
		{
			if (INIT == m_state)
				reset ();
			if (FINISH == m_state)
				return *this;
			m_state = FINISH;
			last_sum ();
			return *this;
		}

	}
}