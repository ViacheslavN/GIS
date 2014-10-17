#include "stdafx.h"
#include "TestRBSet.h"

#include <iostream>

#include <set>

void TestRBSet(){

	std::set<int64> stlMultiSet;


	/*stlMultiSet.insert(1);
	stlMultiSet.insert(7);
	stlMultiSet.insert(15);
	std::multiset<int64>::iterator sit = stlMultiSet.find(3);*/
	/*std::set<int64> stlSet;
	stlSet.insert(1);
	stlSet.insert(2);
	stlSet.insert(3);
	stlSet.insert(4);
	stlSet.insert(3);
	stlSet.erase(3);*/
	/*stlMultiSet.insert(1);
	stlMultiSet.insert(2);
	stlMultiSet.insert(3);
	stlMultiSet.insert(4);
	stlMultiSet.erase(3);

	std::multiset<int64>::iterator it = stlMultiSet.find(3);
	stlMultiSet.erase(it);
	it = stlMultiSet.find(3);*/

	/*for(int64 i = 1000; i > 0; --i){
		for(int64 j = 1000; j > 0; --j)
		{
			stlSet.insert(j);
		}
	}*/
	/*for(int64 i = 16; i > 0; --i)
	{
			stlSet.insert(i);
	 
	}
	for(int64 i = 16; i > 0; --i)
	{
		stlSet.erase(i);

	}*/


CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
	embDB::RBSet<int64> set(alloc, false);
	embDB::RBSet<int64> set1(alloc, false);

	set.insert(100);
	set.insert(770);
	
	set.insert(771);
	set.insert(770);
	set.insert(514);
	set.insert(515);
	set.insert(134);
	set.insert(458);
	set.insert(762);
	set.insert(10);
	set.remove(695) ;
	set.insert(697) ;
	set.insert(697) ;
	set.remove(696) ;
	set.insert(698) ;
	set.insert(698) ;
	set.remove(698) ;
	set.insert(698) ;
	set.remove(697) ;
	set.insert(699) ;
	set.insert(699) ;
	set.remove(698) ;
	set.insert(700) ;
	set.insert(700) ;
	set.remove(700) ;
	set.insert(700) ;
	set.remove(699) ;
	set.insert(701) ;
	set.insert(701) ;
	set.remove(700) ;
	set.insert(702) ;
	set.insert(702) ;
	set.remove(702) ;
	set.insert(702) ;
	set.remove(701) ;
	set.insert(703) ;
	set.insert(703) ;
	set.remove(702) ;
	set.insert(704) ;
	set.insert(704) ;
	set.remove(704) ;
	set.insert(704) ;
	set.remove(703) ;
	set.insert(705) ;
	set.insert(705) ;
	set.remove(704) ;
	set.insert(706) ;
	set.insert(706) ;
	set.remove(706) ;
	set.insert(706) ;
	set.remove(705) ;
	set.insert(707) ;
	set.insert(707) ;
	set.remove(706) ;
	set.insert(708) ;
	set.insert(708) ;
	set.remove(708) ;
	set.insert(708) ;
	set.remove(707) ;
	set.insert(709) ;
	set.insert(709) ;
	set.remove(708) ;
	set.insert(710) ;
	set.insert(710) ;
	set.remove(710) ;
	set.insert(710) ;
	set.remove(709) ;
	set.insert(711) ;
	set.insert(711) ;
	set.remove(710) ;
	set.insert(712) ;
	set.insert(712) ;
	set.remove(712) ;
	set.insert(712) ;
	set.remove(711) ;
	set.insert(713) ;
	set.insert(713) ;
	set.remove(712) ;
	set.insert(714) ;
	set.insert(714) ;
	set.remove(714) ;
	set.insert(714) ;
	set.remove(713) ;
	set.insert(715) ;
	set.insert(715) ;
	set.remove(714) ;
	set.insert(716) ;
	set.insert(716) ;
	set.remove(716) ;
	set.insert(716) ;
	set.remove(715) ;
	set.insert(717) ;
	set.insert(717) ;
	set.remove(716) ;
	set.insert(718) ;
	set.insert(718) ;
	set.remove(718) ;
	set.insert(718) ;
	set.remove(717) ;
	set.insert(719) ;
	set.insert(719) ;
	set.remove(718) ;
	set.insert(720) ;
	set.insert(720) ;
	set.remove(720) ;
	set.insert(720) ;
	set.remove(719) ;
	set.insert(721) ;
	set.insert(721) ;
	set.remove(720) ;
	set.insert(722) ;
	set.insert(722) ;
	set.remove(722) ;
	set.insert(722) ;
	set.remove(721) ;
	set.insert(723) ;
	set.insert(723) ;
	set.remove(722) ;
	set.insert(724) ;
	set.insert(724) ;
	set.remove(724) ;
	set.insert(724) ;
	set.remove(723) ;
	set.insert(725) ;
	set.insert(725) ;
	set.remove(724) ;
	set.insert(726) ;
	set.insert(726) ;
	set.remove(726) ;
	set.insert(726) ;
	set.remove(725) ;
	set.insert(727) ;
	set.insert(727) ;
	set.remove(726) ;
	set.insert(728) ;
	set.insert(728) ;
	set.remove(728) ;
	set.insert(728) ;
	set.remove(727) ;
	set.insert(729) ;
	set.insert(729) ;
	set.remove(728) ;
	set.insert(730) ;
	set.insert(730) ;
	set.remove(730) ;
	set.insert(730) ;
	set.remove(729) ;
	set.insert(731) ;
	set.insert(731) ;
	set.remove(730) ;
	set.insert(732) ;
	set.insert(732) ;
	set.remove(732) ;
	set.insert(732) ;
	set.remove(731) ;
	set.insert(733) ;
	set.insert(733) ;
	set.remove(732) ;
	set.insert(734) ;
	set.insert(734) ;
	set.remove(734) ;
	set.insert(734) ;
	set.remove(733) ;
	set.insert(735) ;
	set.insert(735) ;
	set.remove(734) ;
	set.insert(736) ;
	set.insert(736) ;
	set.remove(736) ;
	set.insert(736) ;
	set.remove(735) ;
	set.insert(737) ;
	set.insert(737) ;
	set.remove(736) ;
	set.insert(738) ;
	set.insert(738) ;
	set.remove(738) ;
	set.insert(738) ;
	set.remove(737) ;
	set.insert(739) ;
	set.insert(739) ;
	set.remove(738) ;
	set.insert(740) ;
	set.insert(740) ;
	set.remove(740) ;
	set.insert(740) ;
	set.remove(739) ;
	set.insert(741) ;
	set.insert(741) ;
	set.remove(740) ;
	set.insert(742) ;
	set.insert(742) ;
	set.remove(742) ;
	set.insert(742) ;
	set.remove(741) ;
	set.insert(743) ;
	set.insert(743) ;
	set.remove(742) ;
	set.insert(744) ;
	set.insert(744) ;
	set.remove(744) ;
	set.insert(744) ;
	set.remove(743) ;
	set.insert(745) ;
	set.insert(745) ;
	set.remove(744) ;
	set.insert(746) ;
	set.insert(746) ;
	set.remove(746) ;
	set.insert(746) ;
	set.remove(745) ;
	set.insert(747) ;
	set.insert(747) ;
	set.remove(746) ;
	set.insert(748) ;
	set.insert(748) ;
	set.remove(748) ;
	set.insert(748) ;
	set.remove(747) ;
	set.insert(749) ;
	set.insert(749) ;
	set.remove(748) ;
	set.insert(750) ;
	set.insert(750) ;
	set.remove(750) ;
	set.insert(750) ;
	set.remove(749) ;
	set.insert(751) ;
	set.insert(751) ;
	set.remove(750) ;
	set.insert(752) ;
	set.insert(752) ;
	set.remove(752) ;
	set.insert(752) ;
	set.remove(751) ;
	set.insert(753) ;
	set.insert(753) ;
	set.remove(752) ;
	set.insert(754) ;
	set.insert(754) ;
	set.remove(754) ;
	set.insert(754) ;
	set.remove(753) ;
	set.insert(755) ;
	set.insert(755) ;
	set.remove(754) ;
	set.insert(756) ;
	set.insert(756) ;
	set.remove(756) ;
	set.insert(756) ;
	set.remove(755) ;
	set.insert(757) ;
	set.insert(757) ;
	set.remove(756) ;
	set.insert(758) ;
	set.insert(758) ;
	set.remove(758) ;
	set.insert(758) ;
	set.remove(757) ;
	set.insert(759) ;
	set.insert(759) ;
	set.remove(758) ;
	set.insert(760) ;
	set.insert(760) ;
	set.remove(760) ;
	set.insert(760) ;
	set.remove(759) ;
	set.insert(761) ;
	set.insert(761) ;
	set.remove(760) ;
	set.insert(762) ;
	set.insert(762) ;
	set.remove(762) ;
	set.insert(762) ;
	set.remove(761) ;
	set.insert(763) ;
	set.insert(763) ;
	set.remove(762) ;
	set.insert(764) ;
	set.insert(764) ;
	set.remove(764) ;
	set.insert(764) ;
	set.remove(763) ;
	set.insert(765) ;
	set.insert(765) ;
	set.remove(764) ;
	set.insert(766) ;
	set.insert(766) ;
	set.remove(766) ;
	set.insert(766) ;
	set.remove(765) ;
	set.insert(767) ;
	set.insert(767) ;
	set.remove(766) ;
	set.insert(768) ;
	set.insert(768) ;
	set.remove(768) ;
	set.insert(768) ;
	set.remove(767) ;
	set.insert(769) ;
	set.insert(769) ;
	set.remove(768) ;
	set.insert(770) ;
	set.insert(770) ;
	set.insert(771) ;
	set.insert(771) ;
	set.insert(768) ;

	embDB::RBSet<int64>::iterator it = set.find(768);
	set.insert(768) ;
	{
		std::set<int64> set;

		set.insert(100);
		set.insert(770);

		set.insert(771);
		set.insert(770);
		set.insert(514);
		set.insert(515);
		set.insert(134);
		set.insert(458);
		set.insert(762);
		set.insert(10);
		set.erase(695) ;
		set.insert(697) ;
		set.insert(697) ;
		set.erase(696) ;
		set.insert(698) ;
		set.insert(698) ;
		set.erase(698) ;
		set.insert(698) ;
		set.erase(697) ;
		set.insert(699) ;
		set.insert(699) ;
		set.erase(698) ;
		set.insert(700) ;
		set.insert(700) ;
		set.erase(700) ;
		set.insert(700) ;
		set.erase(699) ;
		set.insert(701) ;
		set.insert(701) ;
		set.erase(700) ;
		set.insert(702) ;
		set.insert(702) ;
		set.erase(702) ;
		set.insert(702) ;
		set.erase(701) ;
		set.insert(703) ;
		set.insert(703) ;
		set.erase(702) ;
		set.insert(704) ;
		set.insert(704) ;
		set.erase(704) ;
		set.insert(704) ;
		set.erase(703) ;
		set.insert(705) ;
		set.insert(705) ;
		set.erase(704) ;
		set.insert(706) ;
		set.insert(706) ;
		set.erase(706) ;
		set.insert(706) ;
		set.erase(705) ;
		set.insert(707) ;
		set.insert(707) ;
		set.erase(706) ;
		set.insert(708) ;
		set.insert(708) ;
		set.erase(708) ;
		set.insert(708) ;
		set.erase(707) ;
		set.insert(709) ;
		set.insert(709) ;
		set.erase(708) ;
		set.insert(710) ;
		set.insert(710) ;
		set.erase(710) ;
		set.insert(710) ;
		set.erase(709) ;
		set.insert(711) ;
		set.insert(711) ;
		set.erase(710) ;
		set.insert(712) ;
		set.insert(712) ;
		set.erase(712) ;
		set.insert(712) ;
		set.erase(711) ;
		set.insert(713) ;
		set.insert(713) ;
		set.erase(712) ;
		set.insert(714) ;
		set.insert(714) ;
		set.erase(714) ;
		set.insert(714) ;
		set.erase(713) ;
		set.insert(715) ;
		set.insert(715) ;
		set.erase(714) ;
		set.insert(716) ;
		set.insert(716) ;
		set.erase(716) ;
		set.insert(716) ;
		set.erase(715) ;
		set.insert(717) ;
		set.insert(717) ;
		set.erase(716) ;
		set.insert(718) ;
		set.insert(718) ;
		set.erase(718) ;
		set.insert(718) ;
		set.erase(717) ;
		set.insert(719) ;
		set.insert(719) ;
		set.erase(718) ;
		set.insert(720) ;
		set.insert(720) ;
		set.erase(720) ;
		set.insert(720) ;
		set.erase(719) ;
		set.insert(721) ;
		set.insert(721) ;
		set.erase(720) ;
		set.insert(722) ;
		set.insert(722) ;
		set.erase(722) ;
		set.insert(722) ;
		set.erase(721) ;
		set.insert(723) ;
		set.insert(723) ;
		set.erase(722) ;
		set.insert(724) ;
		set.insert(724) ;
		set.erase(724) ;
		set.insert(724) ;
		set.erase(723) ;
		set.insert(725) ;
		set.insert(725) ;
		set.erase(724) ;
		set.insert(726) ;
		set.insert(726) ;
		set.erase(726) ;
		set.insert(726) ;
		set.erase(725) ;
		set.insert(727) ;
		set.insert(727) ;
		set.erase(726) ;
		set.insert(728) ;
		set.insert(728) ;
		set.erase(728) ;
		set.insert(728) ;
		set.erase(727) ;
		set.insert(729) ;
		set.insert(729) ;
		set.erase(728) ;
		set.insert(730) ;
		set.insert(730) ;
		set.erase(730) ;
		set.insert(730) ;
		set.erase(729) ;
		set.insert(731) ;
		set.insert(731) ;
		set.erase(730) ;
		set.insert(732) ;
		set.insert(732) ;
		set.erase(732) ;
		set.insert(732) ;
		set.erase(731) ;
		set.insert(733) ;
		set.insert(733) ;
		set.erase(732) ;
		set.insert(734) ;
		set.insert(734) ;
		set.erase(734) ;
		set.insert(734) ;
		set.erase(733) ;
		set.insert(735) ;
		set.insert(735) ;
		set.erase(734) ;
		set.insert(736) ;
		set.insert(736) ;
		set.erase(736) ;
		set.insert(736) ;
		set.erase(735) ;
		set.insert(737) ;
		set.insert(737) ;
		set.erase(736) ;
		set.insert(738) ;
		set.insert(738) ;
		set.erase(738) ;
		set.insert(738) ;
		set.erase(737) ;
		set.insert(739) ;
		set.insert(739) ;
		set.erase(738) ;
		set.insert(740) ;
		set.insert(740) ;
		set.erase(740) ;
		set.insert(740) ;
		set.erase(739) ;
		set.insert(741) ;
		set.insert(741) ;
		set.erase(740) ;
		set.insert(742) ;
		set.insert(742) ;
		set.erase(742) ;
		set.insert(742) ;
		set.erase(741) ;
		set.insert(743) ;
		set.insert(743) ;
		set.erase(742) ;
		set.insert(744) ;
		set.insert(744) ;
		set.erase(744) ;
		set.insert(744) ;
		set.erase(743) ;
		set.insert(745) ;
		set.insert(745) ;
		set.erase(744) ;
		set.insert(746) ;
		set.insert(746) ;
		set.erase(746) ;
		set.insert(746) ;
		set.erase(745) ;
		set.insert(747) ;
		set.insert(747) ;
		set.erase(746) ;
		set.insert(748) ;
		set.insert(748) ;
		set.erase(748) ;
		set.insert(748) ;
		set.erase(747) ;
		set.insert(749) ;
		set.insert(749) ;
		set.erase(748) ;
		set.insert(750) ;
		set.insert(750) ;
		set.erase(750) ;
		set.insert(750) ;
		set.erase(749) ;
		set.insert(751) ;
		set.insert(751) ;
		set.erase(750) ;
		set.insert(752) ;
		set.insert(752) ;
		set.erase(752) ;
		set.insert(752) ;
		set.erase(751) ;
		set.insert(753) ;
		set.insert(753) ;
		set.erase(752) ;
		set.insert(754) ;
		set.insert(754) ;
		set.erase(754) ;
		set.insert(754) ;
		set.erase(753) ;
		set.insert(755) ;
		set.insert(755) ;
		set.erase(754) ;
		set.insert(756) ;
		set.insert(756) ;
		set.erase(756) ;
		set.insert(756) ;
		set.erase(755) ;
		set.insert(757) ;
		set.insert(757) ;
		set.erase(756) ;
		set.insert(758) ;
		set.insert(758) ;
		set.erase(758) ;
		set.insert(758) ;
		set.erase(757) ;
		set.insert(759) ;
		set.insert(759) ;
		set.erase(758) ;
		set.insert(760) ;
		set.insert(760) ;
		set.erase(760) ;
		set.insert(760) ;
		set.erase(759) ;
		set.insert(761) ;
		set.insert(761) ;
		set.erase(760) ;
		set.insert(762) ;
		set.insert(762) ;
		set.erase(762) ;
		set.insert(762) ;
		set.erase(761) ;
		set.insert(763) ;
		set.insert(763) ;
		set.erase(762) ;
		set.insert(764) ;
		set.insert(764) ;
		set.erase(764) ;
		set.insert(764) ;
		set.erase(763) ;
		set.insert(765) ;
		set.insert(765) ;
		set.erase(764) ;
		set.insert(766) ;
		set.insert(766) ;
		set.erase(766) ;
		set.insert(766) ;
		set.erase(765) ;
		set.insert(767) ;
		set.insert(767) ;
		set.erase(766) ;
		set.insert(768) ;
		set.insert(768) ;
		set.erase(768) ;
		set.insert(768) ;
		set.erase(767) ;
		set.insert(769) ;
		set.insert(769) ;
		set.erase(768) ;
		set.insert(770) ;
		set.insert(770) ;
		set.insert(771) ;
		set.insert(771) ;
		set.insert(768) ;
	}


	 it = set.find(768);
	it = set.find(515);
	set.deleteNode(set.root()->m_pLeft);
	set.deleteNode(set.root()->m_pRight);
	for(int64 i = 1000; i > 0; --i)
	{
		set.insert(i);
	}
	for(int64 i = 0; i < 500;++i)
	{
		set.remove(i);
	}

	for(int64 i = 0; i < 500; ++i)
	{
		set.insert(i);
	}
	for(int64 i = 1000; i > 0; --i)
	{
		embDB::RBSet<int64>::iterator it = set.find(i);
		set1.remove(i);
	}

	set.insert(1);
	set.insert(2);
	set.insert(3);
	for(int64 i = 100000; i > 0; --i)
	{
		set1.insert(i);
	}




	
	for(int64 i = 1600000; i > 0; --i)
	{
		embDB::RBSet<int64>::iterator it = set1.find(i);
		
		set1.remove(i);
	}


	/*

	set.insert(3);
	set.insert(3);
	set.insert(3);
	set.insert(3);
	set.insert(3);
	set.insert(3);

	set.remove(3);
	set.remove(3);
	set.remove(3);
	set.remove(3);
	set.remove(3);
	set.remove(3);*/
	/*for(int64 i = 3; i > 0; --i){
		for(int64 j = 3; j > 0; --j)
		{
			stlMultiSet.insert(j);
		}
	}
	for(int64 i = 3; i > 0; --i){
		for(int64 j = 3; j > 0; --j)
		{
			std::multiset<int64> ::iterator it = stlMultiSet.find(j);
			if(it == stlMultiSet.end())
			{
				int dd = 0;
				dd++;
			}
			stlMultiSet.erase(it);
		}
	}*/
	
  /*for(int64 i = 3; i > 0; --i){
		for(int64 j = 3; j > 0; --j)
		{
			set.insert(j);
		}
	}
	for(int64 i = 3; i > 0; --i){
		for(int64 j = 3; j > 0; --j)
		{
			embDB::RBSet<int64>::iterator it = set.find(j);
			if(it.isNull()|| it.getKey() != j)
			{
				int dd = 0;
				dd++;
			}
			set.remove(j);
		}
	}*/

	/*for(int64 i = 1600000; i > 0; --i)
	{
		set.insert(i);
	}
	embDB::RBSet<int64>::TTreeNode* pNode =  set.m_NullHeadNode.m_pParent;
	int nCnt = 0;
	while(pNode != &set.m_NullHeadNode)
	{
		pNode = pNode->m_pNext;
		nCnt++;
	}
	for(int64 i = 1600000; i > 0; --i)
	{
		embDB::RBSet<int64>::iterator it = set.find(i);
		if(it.isNull())
		{
			int dd = 0;
			dd++;
		}
	}

	for(int64 i = 1600000; i > 0; --i)
	{
		embDB::RBSet<int64>::iterator it = set.find(i);
		if(it.isNull())
		{
			int dd = 0;
			dd++;
		}
		set.remove(i);

	}*/
	

	for(int64 i = 1000; i > 0; --i){
		for(int64 j = 1000; j > 0; --j)
		{
			set.insert(j);
		}
	}
	
	for(int64 i = 1000; i > 0; --i){
		for(int64 j = 1000; j > 0; --j)
		{
			embDB::RBSet<int64>::iterator it = set.find(j);
		
			set.deleteNode(it.m_pNode, true, true);
		}
	}

	for(int64 i = 16; i > 0; --i)
	{
		set.insert(i);
	}
	{
		embDB::RBSet<int64>::iterator it = set.begin();
		while(!it.isNull())
		{
			std::cout << it.key() << std::endl;
			it.next();
		}
	}
	
	for(int64 i = 16; i > 0; --i)
	{
		set.insert(i);
	}
	for(int64 i = 16; i > 0; --i)
	{
		set.insert(i);
	}
	for(int64 i = 100; i > 0; --i)
	{
		set.insert(i);
	}
	{
		embDB::RBSet<int64>::iterator it = set.begin();
		while(!it.isNull())
		{
			std::cout << it.key() << std::endl;
			it.next();
		}
	}
	for(int64 i = 100; i > 0; --i)
	{
		embDB::RBSet<int64>::iterator it = set.find(i);
		if(it.isNull())
		{
			assert(false);
		}
		int64 tt = it.key();
		tt = *it;
		set.remove(it);
	}
	{
		embDB::RBSet<int64>::iterator it = set.begin();
		
		while(!it.isNull())
		{
			std::cout << it.key() << std::endl;
			it.next();
		}
 
			
	}
	
	for(int64 i = 1000; i > 0; --i){
		for(int64 j = 1000; j > 0; --j)
		{
			embDB::RBSet<int64>::iterator it = set.find(j);
			int64 tt = it.key();
			tt = *it;
			set.remove(it);
		}
	}
	{
		embDB::RBSet<int64>::iterator it = set.begin();

		while(!it.isNull())
		{
			std::cout << it.key() << std::endl;
			it.next();
		}


	}
}


void TestRBMultiSet(){
	CommonLib::simple_alloc_t alloc;
	embDB::RBMultiSet<int64> set(&alloc);

	for(int64 i = 1000; i > 0; --i){
		for(int64 i = 1000; i > 0; --i)
		{
			set.insert(i);
		}
	}

	embDB::RBMultiSet<int64>::TTreeNode *pNode = set.findNodeForBTreeInsert(-1);


	for(int64 i = 1000; i > 0; --i){
		for(int64 i = 1000; i > 0; --i)
		{
			embDB::RBMultiSet<int64>::iterator it = set.find(i);
			int64 tt = it.getKey();
			tt = *it;
			set.remove(it);
		}
	}


	for(int64 i = 1000; i > 0; --i){
		for(int64 i = 1000; i > 0; --i)
		{
			set.insert(i);
		}
	}
	for(int64 i = 1000; i > 0; --i){
		for(int64 i = 1000; i > 0; --i)
		{
			set.remove(i);
		}
	}
	embDB::RBMultiSet<int64>::iterator it = set.begin();
	assert(!!it.IsNull());
	for(int64 i = 1000; i > 0; --i){
		for(int64 i = 1000; i > 0; --i)
		{
			set.insert(i);
		}
	}
	it = set.begin();
	while(!it.IsNull()){
		int64 tt = it.getKey();
		tt = *it;
		it++;
	}
	it = set.last();
	while(!it.IsNull()){
		int64 tt = it.getKey();
		tt = *it;
		it--;
	}
	it = set.begin();
	while(!it.IsNull()){
		int64 tt = it.getKey();
		tt = *it;
		it = set.remove(it);
	}
}