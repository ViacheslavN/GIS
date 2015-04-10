#ifndef _EMBEDDED_DATABASE_COUNTER_FIELD_H_
#define _EMBEDDED_DATABASE_COUNTER_FIELD_H_
#include "BaseBPMapv2.h"
#include "IDBField.h"
#include "DBFieldInfo.h"
#include "Index.h"
#include "ValueField.h"
namespace embDB
{


	template<class _FType, class _TBTree, int FieldDataType>
	class CounterField : public ValueField<_FType, _TBTree, FieldDataType>
	{
	public:
		typedef ValueField<_FType, _TBTree, FieldDataType> TValueField;
	private:


	};
}


#endif