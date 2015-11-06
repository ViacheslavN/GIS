#include "stdafx.h"
/*#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../EmbDB/BPVectorNoPod.h"
#include <vector>
#include <algorithm>
#include <set>
#include "CommonLibrary/DebugTime.h"
#include "../../EmbDB/CompositeIndexKey.h"
#include "../../EmbDB/VariantField.h"*/


/*

class Test
{
	int i;
public:
	Test(CommonLib::alloc_t* pAlloc, int _i = 10): m_pAlloc(pAlloc)
	{
		i = _i;
		m_pBuf = (char*)m_pAlloc->alloc(1000);
	}
	~Test()
	{
		i = 0;
		m_pAlloc->free(m_pBuf);
		m_pBuf = nullptr;
	}


	Test(const Test& key)
	{
		i = key.i + 10;
	}
	Test& operator=(const Test& key)
	{
		i = key.i + 20;
		return *this;
	}

	CommonLib::alloc_t* m_pAlloc;
	char *m_pBuf;
};

void ShowNoPodVector(embDB::TBPVectorNoPOD<embDB::CompositeIndexKey>& vec)
{
	for (size_t i = 0, sz = vec.size(); i < sz; ++i)
	{
		int nVal= 0;
		vec[i].getValue(0)->getVal(nVal);
		std::cout << nVal << ", ";
	}

	std::cout << std::endl;
}

void TestPODTypeVector()
{
	CommonLib::simple_alloc_t alloc;

	Test Testt(&alloc);
	{
		std::vector<Test> stdvec;
		stdvec.reserve(150);
		//stdvec.push_back(Testt);
		
		int i = 0;
		i++;
	}
	
 
	
	embDB::TVarINT32 valint32;
	valint32.set(12);
	embDB::TVarINT64 valint64;
	valint64.set(124587536454);
	
	int n = 10;
	//vec.resize(n);
	{
		embDB::TBPVectorNoPOD<embDB::CompositeIndexKey> vec(&alloc), vec1(&alloc), vec2(&alloc),
		vec3(&alloc), vec4(&alloc);
		

		for (int i = 0; i < n; ++i)
		{
			embDB::CompositeIndexKey key(&alloc);
			valint32.setVal(i);
			valint64.setVal((int64)i);
			key.addValue(&valint32);
			key.addValue(&valint64);

			vec.push_back(key);
			vec1.push_back(key);
			vec2.push_back(key);
		}
		ShowNoPodVector(vec);
		std::cout << "erase 4" << std::endl;
		vec.remove(4);
		ShowNoPodVector(vec);
		std::cout << "erase 0" << std::endl;
		vec.remove(0);
		ShowNoPodVector(vec);
		std::cout << "erase last" << std::endl;
		vec.remove(vec.size() - 1);
		ShowNoPodVector(vec);

		std::cout << "erase end" << std::endl;
		vec.remove(vec.size());
		ShowNoPodVector(vec);


		std::cout << "inset vec" << std::endl;
		vec1.insert(vec, vec1.size(), 0, vec.size());
		ShowNoPodVector(vec1);
		/*std::cout << "mover 0 2 " << std::endl;
		vec.mover(0, 3);

		ShowNoPodVector(vec);

		std::cout << "mover 3 5 " << std::endl;
		vec1.mover(3, 5);

		ShowNoPodVector(vec1);

		std::cout << "movel 3 3 " << std::endl;
		vec2.movel(3, 3);

		ShowNoPodVector(vec2);

		vec2.copy(vec1, 1, 2, 5);
		std::cout << "vec2 copy 1 3 5 " << std::endl;
		ShowNoPodVector(vec2);

	}
	int i = 0;
	i++;
	//vec.movel(4, 5);

}*/