#ifndef _EMBEDDED_DATABASE_B_MULTI_INDEX_H_
#define _EMBEDDED_DATABASE_B_MULTI_INDEX_H_
#include "Index.h"
 
namespace embDB

{

	class MultiIndexFiled : public IndexFiled
	{
	public:
		MultiIndexFiled() {}
		virtual ~MultiIndexFiled() {}
		virtual IIndexIteratorPtr find(CommonLib::CVariant* pIndexKey, uint64 nOID) = 0;
		virtual IIndexIteratorPtr lower_bound(CommonLib::CVariant* pIndexKey, uint64 nOID) = 0;
		virtual IIndexIteratorPtr upper_bound(CommonLib::CVariant* pIndexKey, uint64 nOID) = 0;
		virtual bool remove (CommonLib::CVariant* pIndexKey, uint64 nOID) = 0;
	};


}





#endif
