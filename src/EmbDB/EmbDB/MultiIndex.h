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
		virtual TIndexIterator find(IFieldVariant* pIndexKey, uint64 nOID) = 0;
		virtual TIndexIterator lower_bound(IFieldVariant* pIndexKey, uint64 nOID) = 0;
		virtual TIndexIterator upper_bound(IFieldVariant* pIndexKey, uint64 nOID) = 0;
		virtual bool remove (IFieldVariant* pIndexKey, uint64 nOID) = 0;
	};


}





#endif
