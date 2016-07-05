#ifndef _EMBEDDED_DATABASE_LINK_DIFF_COMPRESS_H_
#define _EMBEDDED_DATABASE_LINK_DIFF_COMPRESS_H_
#include <map>
#include <vector>
#include "BaseValueDiffCompressor.h"
namespace embDB
{

	class InnerLinkCompress
	{
	public:



		InnerLinkCompress(CommonLib::alloc_t *pAlloc, CompressorParamsBaseImp *pParams);
		~InnerLinkCompress();

		void AddLink(int64 nLink);
		void RemoveLink(int64 nLink);
		uint32 GetCompressSize() const;



		void compress( const embDB::TBPVector<int64>& vecLinks, CommonLib::IWriteStream *pStream);
		void decompress(uint32 nSize, embDB::TBPVector<int64>& vecLinks, CommonLib::IReadStream *pStream);
		void clear();
	private:
		TCompressor m_compressor;
	};
}


#endif