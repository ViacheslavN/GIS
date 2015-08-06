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
		virtual IndexIteratorPtr find(IVariant* pIndexKey, uint64 nOID) = 0;
		virtual IndexIteratorPtr lower_bound(IVariant* pIndexKey, uint64 nOID) = 0;
		virtual IndexIteratorPtr upper_bound(IVariant* pIndexKey, uint64 nOID) = 0;
		virtual bool remove (IVariant* pIndexKey, uint64 nOID) = 0;
	};


}





#endif
