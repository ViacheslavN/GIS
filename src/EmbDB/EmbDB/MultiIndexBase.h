#ifndef _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_BASE_
#define _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_BASE_

namespace embDB
{

template <class _TKey>
class IndexTuple
{
public: 
	typedef _TKey TKey;
	IndexTuple() : m_nRowID(-1)
	{}
	IndexTuple(TKey _key, uint64  _nObjectID) : m_key(_key), m_nRowID(_nObjectID)
	{}
	~IndexTuple(){};

	TKey m_key;
	int64  m_nRowID;
};

template <class _TKey>
class MultiIndexBaseComp
{
	public: 
	typedef _TKey TKey;

	typedef IndexTuple<_TKey> TIndex;

	bool LE(const TIndex& _Left, const TIndex& _Right) const
	{
		if(_Left. m_key != _Right. m_key)
			return (_Left. m_key < _Right. m_key);
		return (_Left. m_nRowID < _Right. m_nRowID);
	}
	bool EQ(const TIndex& _Left, const TIndex& _Right) const
	{
		return (_Left. m_key  == _Right. m_key &&  _Left. m_nRowID  == _Right. m_nRowID);
	}
};

template <class _TKey>
class MultiIndexKeyOnlyComp
{
public: 
	typedef _TKey TKey;

	typedef IndexTuple<_TKey> TIndex;

	bool LE(const TIndex& _Left, const TIndex& _Right) const
	{
		return (_Left. m_key < _Right. m_key);
	}
	bool EQ(const TIndex& _Left, const TIndex& _Right) const
	{
		return _Left. m_key  == _Right. m_key ;
	}
};

}

#endif