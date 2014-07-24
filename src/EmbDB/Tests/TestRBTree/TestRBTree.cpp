#include "stdafx.h"
#include "TestRBTree.h"

#include <iostream>

void TestRBTree()
{
	/*CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
	embDB::TRBTree<int, int> tree(alloc);
	embDB::TRBTree<int, int> tree1(alloc);
	embDB::TRBTree<int, int> tree2(alloc);
   for(int i = 0; i < 100; ++i)
	{
		tree1.insert(i, i);
	}
  embDB::RBTreeIterator<int, int> iter = tree1.begin();

  int val;
  while(iter.IsEnd())
  {
    std::cout << "  " << iter.GetKey() <<" \n";
    //iter.next();
	++iter;
  }

  iter.Reset(false);
   while(iter.IsEnd())
  {
    std::cout << "  " << iter.GetKey() <<" \n";
    //iter.back();
	--iter;
  }

    iter.Reset(true);
   while(iter.IsEnd())
  {
	  std::cout << "  " << iter.GetKey() << "size " << tree1.size()<<" \n";
    //iter.back();
	iter = tree1.remove(iter);
  }

  //return;

  for(int i = 100; i > 0; --i)
	{
		tree2.insert(i, i);
	}
  embDB::RBTreeIterator<int, int> iter2 = tree2.begin();


  while(iter2.IsEnd())
  {
    std::cout << "  " << iter2.GetKey() <<" \n";
    iter2.next();
  }


  for(int j = 0; j < 3; ++j)
  {
  	embDB::TRBTree<__int64, __int64> tree1(alloc);

    for(__int64 i = 4000000; i > 0; --i)
	  {
		  tree1.insert(i, i);
 	  }
  }
   for(int i = 0; i < 100; ++i)
	{
		tree.insert(i, i);
	}

	for(int i = 0; i < 1000000; ++i)
	{
		tree.insert(i, i);
	}




	for(int i = 0; i < 1000000; ++i)
	{
		tree.remove(i);
	}

	for(int i = 1000000; i > 0; --i)
	{
		tree.insert(i, i);
	}

	for(int i = 1000000; i > 0; --i)
	{
		assert(tree.findNode(i) != 0);
		tree.remove(i);
		assert(tree.findNode(i) == 0);
	}

 	for(int i = 1000000; i > 0; --i)
	{
		tree.insert(i, i);
	}

  while(tree.size())
  {
    tree.remove(tree.GetRoot()->key_);
  }
  for(int i = 0; i < 1000000; ++i)
	{
			tree.insert(i, i);
	}
  while(tree.size())
  {
    tree.remove(tree.GetRoot()->key_);
  }
  for(int i = 0; i < 1000000; ++i)
	{
			tree.insert(i, i);
	}

  //std::map*/

}