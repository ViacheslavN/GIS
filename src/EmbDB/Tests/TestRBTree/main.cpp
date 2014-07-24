// TestRBTree.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestRBTree.h"
#include "TestRBSet.h"
#include "TestMap.h"
#include <set>
#include <map>
#include <iostream>
#include "../../EmbDB/RBMap.h"

template<class _TAggrKey, class _TMemSet>
class A
{
public:
	typedef _TAggrKey       TAggrKey;
	typedef _TMemSet   TMemSet;
	A(){}
	virtual ~A(){std::cout <<"~A()";}

	virtual bool LoadNode(TMemSet& Set) = 0;

	virtual int size() const = 0;
	
};
template<class _TAggrKey, class _TMemSet>
class B : public A<_TAggrKey, _TMemSet>
{
public:
	typedef  A<_TAggrKey, _TMemSet> TBase;
	typedef typename TBase::TAggrKey TAggrKey;
	typedef typename TBase::TMemSet TMemSet;
	B(){}
	virtual ~B( ){std::cout <<"~B()"; }
	bool LoadNode(TMemSet& Set)
	{
		std::cout <<"B::LoadNode";
		_TAggrKey key;
		key = 12;
		Set.insert(key);
		return true;
	}

	virtual int size() const
	{
		return 1;
	}

};
template<class _TAggrKey, class _TMemSet>
class C : public A<_TAggrKey, _TMemSet>
{
public:
	C(){}
	virtual ~C( ){std::cout <<"~C()"; }
	bool LoadNode(_TMemSet& Set)
	{
		std::cout <<"C::LoadNode";
		_TAggrKey key;
		key = 18;
		Set.insert(key);
		key = 22;
		Set.insert(key);
		return true;
	}

	virtual int size() const
	{
		return 2;
	}

};

int _tmain(int argc, _TCHAR* argv[])
{
	{
		std::multimap<int, int> m_map;
		for(int i= 0; i < 100; ++i)
		{
			m_map.insert(std::multimap<int, int>::value_type(i, 0));
		}
		m_map.insert(std::multimap<int, int>::value_type(20, 1));
		m_map.insert(std::multimap<int, int>::value_type(20, 2));
		m_map.insert(std::multimap<int, int>::value_type(20, 3));

		std::multimap<int, int>::iterator it = m_map.find(20);
		for(; it != m_map.end(); ++it)
		{
		}
	}
	{
		CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
		embDB::RBMap<int, int> emb_map(alloc, true);


		for(int i= 0; i < 100; ++i)
		{
			emb_map.insert(i, 0);
		}
		emb_map.insert(20, 1);
		emb_map.insert(20, 2);
		emb_map.insert(20, 3);

		embDB::RBMap<int, int>::iterator it = emb_map.find(20);
		for(; !it.isNull(); ++it)
		{
		}
	}

	/*A<int, std::set<int> > *pB = 0;
	A<float, std::set<float> > *pC = 0;

	pB = new B<int, std::set<int> >();
	pC= new C<float, std::set<float> >();
	std::set<int> setI;
	std::set<float> setF;
	pB->LoadNode(setI);
	pC->LoadNode(setF);
	delete pB;
	delete pC;
	TestMultiMap();*/
//	TestRBMultiSet();
	TestRBSet();
	//TestMap();
	//std::regex(
	//TestRBTree();
	return 0;

	

}

