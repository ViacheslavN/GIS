#ifndef _EMBEDDED_DATABASE_I_BP_TREE_H_
#define _EMBEDDED_DATABASE_I_BP_TREE_H_

#include "IDBTransactions.h"
#include "IField.h"
namespace embDB
{
	template<class _TKey, class _TValue>
	class IBPTree : public IDBBtree
	{
	public:
		typedef typename _TKey  Tkey;
		typedef typename _TValue  TValue;

		virtual bool insert(const Tkey& key, const TValue& value ) = 0;
		virtual bool remove(const Tkey& key) = 0;
		virtual bool update(const Tkey& key, const TValue& value ) = 0;
		virtual bool search(const Tkey& key, CommonLib::IVariant* value) = 0;

	};
}
#endif