#ifndef _EMBEDDED_DATABASE_B_KEY_H_
#define _EMBEDDED_DATABASE_B_KEY_H_
#include "embDBInternal.h"
namespace embDB
{




	template <typename _TKey, typename _TVal>
	class TBaseNodeElem
	{
	public:
		typedef _TKey TKey;
		typedef _TVal TVal;
		 TKey m_key;
		 TVal m_val;
		 int64 m_nLink;
	};

	


	 
	template <typename TNodeElem >
	struct TBNodeComp
	{	
		bool LE(const TNodeElem& _Left, const TNodeElem& _Right){
			return (_Left.m_key < _Right.m_key);
		}
		bool EQ(const TNodeElem& _Left, const TNodeElem& _Right){
			return (_Left.m_key == _Right.m_key);
		}
	};

	template <typename _TVal>
	class TBaseIndexKey
	{
	public:
		typedef _TVal TVal;
		TVal m_nKey;
		int64 m_nOID;
		 int64 m_nLink;

	};
}
#endif