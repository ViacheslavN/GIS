#ifndef _EMBEDDED_DATABASE_LINK_COMPRESS_H_
#define _EMBEDDED_DATABASE_LINK_COMPRESS_H_
#include <map>
#include <vector>
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "BPVector.h"
namespace embDB
{

	class InnerLinkCompress
	{
		public:
			InnerLinkCompress();
			~InnerLinkCompress();

			void AddLink(int64 nLink);
			void RemoveLink(int64 nLink);


			void compress(TBPVector<int64>& oids, CommonLib::IWriteStream* pStream);
			void read(uint32 nSize, TBPVector<int64>& oids, CommonLib::IReadStream* pStream);
	private:
			
	};
}


#endif