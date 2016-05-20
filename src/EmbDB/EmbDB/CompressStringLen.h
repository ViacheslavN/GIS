#ifndef _EMBEDDED_DATABASE_COMPRESS_STRING_LEN_H_
#define _EMBEDDED_DATABASE_COMPRESS_STRING_LEN_H_

#include "SignedNumLenDiffCompressor2.h"


namespace embDB
{
	class CCompressLen
	{
		public:
			CCompressLen();
			~CCompressLen();

		private:
		 SignedDiffNumLenCompressor232i   m_lenCOmpressor;

	};
}
#endif