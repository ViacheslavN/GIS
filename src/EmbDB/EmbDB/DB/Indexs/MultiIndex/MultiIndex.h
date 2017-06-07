#ifndef _EMBEDDED_DATABASE_B_MULTI_INDEX_H_
#define _EMBEDDED_DATABASE_B_MULTI_INDEX_H_
#include "../UniqueIndex/Index.h"
 
namespace embDB

{

	class MultiIndexFiled : public IndexFiled
	{
	public:
		MultiIndexFiled() {}
		virtual ~MultiIndexFiled() {}
		virtual IIndexIteratorPtr find(const CommonLib::CVariant* pIndexKey, int64 nOID) = 0;
		virtual IIndexIteratorPtr lower_bound(const CommonLib::CVariant* pIndexKey, int64 nOID) = 0;
		virtual IIndexIteratorPtr upper_bound(const CommonLib::CVariant* pIndexKey, int64 nOID) = 0;
		virtual bool remove (const CommonLib::CVariant* pIndexKey, int64 nOID) = 0;
	};


}





#endif
