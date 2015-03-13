#ifndef _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_BASE_
#define _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_BASE_

namespace embDB
{

template <class _TKey>
class IndexTuple
{
public: 
	typedef _TKey TKey;
	IndexTuple() : m_nObjectID(0)
	{}
	IndexTuple(TKey _key, uint64  _nObjectID) : m_key(_key), m_nObjectID(_nObjectID)
	{}
	~IndexTuple(){};

	TKey m_key;
	uint64  m_nObjectID;
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
		return (_Left. m_nObjectID < _Right. m_nObjectID);
	}
	bool EQ(const TIndex& _Left, const TIndex& _Right) const
	{
		return (_Left. m_key  == _Right. m_key &&  _Left. m_nObjectID  == _Right. m_nObjectID);
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