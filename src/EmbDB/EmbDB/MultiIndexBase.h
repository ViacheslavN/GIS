#ifndef _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_BASE_
#define _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_BASE_

namespace embDB
{

template <class _TKey>
class IndexTuple
{
public: 
	typedef _TKey TKey;
	IndexTyple();
	~IndexTyple();

	TKey key;
	uint64  nObjectID;
};

template <class _TKey>
class MultiIndexBaseComp
{
	typedef _TKey TKey;

	typedef IndexTuple<_TKey> TIndex;

	bool LE(const TIndex& _Left, const TIndex& _Right)
	{
		if(_Left.key != _Right.key)
			return (_Left.key < _Right.key);
		return (_Left.nObjectID < _Right.nObjectID);
	}
	bool EQ(const TIndex& _Left, const TIndex& _Right)
	{
		return (_Left.key  == _Right.key &&  _Left.nObjectID  == _Right.nObjectID);
	}
};

template <class _TKey>
class MultiIndexKeyOnlyComp
{
	typedef _TKey TKey;

	typedef IndexTuple<_TKey> TIndex;

	bool LE(const TIndex& _Left, const TIndex& _Right)
	{
		return (_Left.key < _Right.key);
	}
	bool EQ(const TIndex& _Left, const TIndex& _Right)
	{
		return _Left.key  == _Right.key ;
	}
};

}

#endif