#include "stdafx.h"
#include "BaseHash.h"

namespace embDB
{
	namespace Crypto
	{
		void CBaseHash::reset ()
		{
			m_state = ADD;
			m_buf.clear ();
			m_nLen = 0;
			init_sum ();
		}
		void CBaseHash::add (const byte *pData, uint32 nSize)
		{
			if (ADD != m_state)
				reset ();
			
			if (nSize == 0)
				return;
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
			if (nSize == nPos)
				return;

			for (; nPos + blksize < nSize; nPos += blksize)
			{
 				update_sum (pData);
				m_buf.clear ();
				m_nLen += blksize;
			}
			m_buf.copy (pData, nSize);
			m_nLen += m_buf.size ();
	
 
		}
		void CBaseHash::add (const CommonLib::CBlob& blob)
		{
			add(blob.buffer(), blob.size());
		}
		void CBaseHash::finish ()
		{
			if (INIT == m_state)
				reset ();
			if (FINISH == m_state)
				return;
			m_state = FINISH;
			last_sum ();
			return;
		}
		void CBaseHash::digest (CommonLib::CBlob& blob)
		{
			blob.resize(length());
			digest(blob.buffer(), blob.size());
		}
	}
}