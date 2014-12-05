#ifndef _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_
#define _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_
#include "BaseBPSetv2.h"
namespace embDB
{

	template <class _TIndexType, class _TVal>
	class IndexTyple
	{
	public: 
		typedef _TIndexType TIndexKey;
		typedef _TVal TValueKey;
		IndexTyple();
		~IndexTyple();

		TIndexType nIndexKey;
		TValueKey  nValueKey;
	};

	template <class _TIndexType, class _TVal>
	class MultiIndexBaseComp
	{
		typedef _TIndexType TIndexType;
		typedef _TVal TValue;

		typedef IndexTyple<TIndexType, TValue> TIndex;

		bool LE(const TIndex& _Left, const TIndex& _Right)
		{
			if(_Left.nIndexKey != _Right.nIndexKey)
				return (_Left.nIndexKey < _Right.nIndexKey);
			return (_Left.nValueKey < _Right.nValueKey);
		}
		bool EQ(const TIndex& _Left, const TIndex& _Right)
		{
			return (_Left.nIndexKey  == _Right.nIndexKey || _Left.nValueKey  == _Right.nValueKey);
		}
	};

	template<class _TIndexType, class _TVal,  class _Transaction>
	class BPMultiIndex : public TBPlusTreeSetV2<IndexTyple<_TIndexType, _TVal>
		, MultiIndexBaseComp<_TIndexType, _TVal>, _Transaction   >
}

#endif
