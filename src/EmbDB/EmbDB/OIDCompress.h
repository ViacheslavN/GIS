#ifndef _EMBEDDED_DATABASE_FIELD_OID_COMPRESS_H_
#define _EMBEDDED_DATABASE_FIELD_OID_COMPRESS_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include <set>
#include <vector>
#include <map>
namespace embDB
{
	class OIDCompress
	{

		 typedef std::map<int32, int32> TLenFreq;
		 typedef std::map<int64, int32> TDiffFreq;
		public:

			OIDCompress();
			~OIDCompress();

			uint32 GetRowSize();
			void AddSymbol(int64 nDiff);

			double Log2( double n )  
			{  
	 
				return log( n ) / log( (double)2 );  
			}

	private:

		TLenFreq m_LenFreq;
		TDiffFreq m_DiffFreq;

	};

}

#endif