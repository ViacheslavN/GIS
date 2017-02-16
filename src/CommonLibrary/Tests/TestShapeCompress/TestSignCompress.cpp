#include "stdafx.h"


#include "..\..\CommonLibrary\BitStream.h"
#include "..\..\CommonLibrary\MemoryStream.h"
#include "..\..\CommonLibrary\SignCompressor2.h"
#include <iostream>

void Compress(const std::vector<bool>& vecSign)
{
	CommonLib::TSignCompressor2 signCompress;

	CommonLib::CWriteMemoryStream writeStream;

	writeStream.resize(vecSign.size() * sizeof(int32));

	CommonLib::CReadMemoryStream readStream;

	std::vector<bool>  vecDecodeSign;
	for (size_t i = 0; i < vecSign.size(); ++i)
	{

		signCompress.AddSymbol(vecSign[i], i);
	}

	signCompress.BeginCompress(&writeStream);

	if(signCompress.IsNeedEncode())
	{
		for (size_t i = 0; i < vecSign.size(); ++i)
		{
			signCompress.EncodeSign(vecSign[i], i);
		}
	}
	

	readStream.attachBuffer(writeStream.buffer(), writeStream.pos());

	signCompress.clear();
	signCompress.BeginDecompress(&readStream, vecSign.size());

	for (size_t i = 0; i < vecSign.size(); ++i)
	{
		vecDecodeSign.push_back(signCompress.DecodeSign(i));
	}

	for (size_t i = 0; i < vecSign.size(); ++i)
	{
		 if(vecSign[i] != vecDecodeSign[i])
		 {
			 std::cout << "error sign i " << i << " enocode "; 
			 std::cout <<  vecSign[i] ? 1 : 0;
			std::cout  << " decode "  <<  vecDecodeSign[i] ? 1 : 0;
			std::cout << std::endl;
		 }
	}

}

void TestSignCompress()
{

	std::vector<bool> vecSignOnePlus;
	std::vector<bool> vecSignOneMunus;
	std::vector<bool> vecSignPosPlus;
	std::vector<bool> vecSignPosMunus;
	std::vector<bool> vecSignBit;
	std::vector<bool> vecSignCompressPosPlus;
	std::vector<bool> vecSignCompressPosMunus;

	std::vector<bool> vecSignOnePlusDecomp;
	std::vector<bool> vecSignOneMunusDecomp;
	std::vector<bool> vecSignPosDecomp;
	std::vector<bool> vecSignCompressPosDecomp;


	std::vector<bool> vecSign1;
	std::vector<short> vec;

	vec.push_back(46);
	vec.push_back(55);
	vec.push_back(65);
	vec.push_back(75);
	vec.push_back(80);
	vec.push_back(81);
	vec.push_back(83);
	vec.push_back(85);
	vec.push_back(86);
	vec.push_back(87);
	vec.push_back(88);
	vec.push_back(89);
	vec.push_back(90);
	vec.push_back(92);
	vec.push_back(93);
	vec.push_back(94);
	vec.push_back(95);
	vec.push_back(97);
	vec.push_back(99);
	vec.push_back(101);
	vec.push_back(102);
	vec.push_back(103);
	vec.push_back(104);
	vec.push_back(105);
	vec.push_back(106);
	vec.push_back(109);
	vec.push_back(116);
	vec.push_back(118);
	vec.push_back(120);
	vec.push_back(122);
	vec.push_back(123);
	vec.push_back(125);
	vec.push_back(127);
	vec.push_back(128);
	vec.push_back(130);
	vec.push_back(131);
	vec.push_back(133);
	vec.push_back(136);
	vec.push_back(137);
	vec.push_back(139);
	vec.push_back(140);
	vec.push_back(153);
	vec.push_back(155);
	vec.push_back(156);
	vec.push_back(157);
	vec.push_back(159);
	vec.push_back(161);
	vec.push_back(187);
	vec.push_back(189);
	vec.push_back(190);
	vec.push_back(191);
	vec.push_back(192);
	vec.push_back(193);
	vec.push_back(202);
	vec.push_back(203);
	vec.push_back(244);
	vec.push_back(251);
	vec.push_back(256);
	vec.push_back(257);
	vec.push_back(258);
	vec.push_back(262);
	vec.push_back(265);
	vec.push_back(267);
	vec.push_back(269);
	vec.push_back(271);
	vec.push_back(274);
	vec.push_back(276);
	vec.push_back(278);
	vec.push_back(280);
	vec.push_back(282);
	vec.push_back(284);
	vec.push_back(336);
	vec.push_back(340);
	vec.push_back(343);
	vec.push_back(376);
	vec.push_back(380);
	vec.push_back(382);
	vec.push_back(383);
	vec.push_back(385);
	vec.push_back(389);
	vec.push_back(391);
	vec.push_back(393);
	vec.push_back(394);
	vec.push_back(396);
	vec.push_back(398);
	vec.push_back(400);
	vec.push_back(402);
	vec.push_back(404);
	vec.push_back(406);
	vec.push_back(407);
	vec.push_back(408);
	vec.push_back(411);
	vec.push_back(413);
	vec.push_back(417);
	vec.push_back(418);
	vec.push_back(434);
	vec.push_back(479);
	vec.push_back(482);
	vec.push_back(484);
	vec.push_back(487);
	vec.push_back(489);
	vec.push_back(491);
	vec.push_back(511);
	vec.push_back(519);
	vec.push_back(520);
	vec.push_back(521);
	vec.push_back(530);
	vec.push_back(548);
	vec.push_back(560);
	vec.push_back(593);
	vec.push_back(595);
	vec.push_back(597);
	vec.push_back(616);
	vec.push_back(618);
	vec.push_back(619);
	vec.push_back(620);
	vec.push_back(622);
	vec.push_back(623);
	vec.push_back(624);
	vec.push_back(625);
	vec.push_back(628);
	vec.push_back(629);
	vec.push_back(631);
	vec.push_back(632);
	vec.push_back(639);
	vec.push_back(644);
	vec.push_back(645);
	vec.push_back(646);
	vec.push_back(647);
	vec.push_back(650);
	vec.push_back(652);
	vec.push_back(654);
	vec.push_back(657);
	vec.push_back(658);
	vec.push_back(660);
	vec.push_back(662);
	vec.push_back(664);
	vec.push_back(665);
	vec.push_back(666);
	vec.push_back(670);
	vec.push_back(676);
	vec.push_back(678);
	vec.push_back(680);
	vec.push_back(682);
	vec.push_back(683);
	vec.push_back(685);
	vec.push_back(687);
	vec.push_back(689);
	vec.push_back(691);
	vec.push_back(693);
	vec.push_back(695);
	vec.push_back(696);
	vec.push_back(699);
	vec.push_back(701);
	vec.push_back(702);
	vec.push_back(703);
	vec.push_back(704);
	vec.push_back(705);
	vec.push_back(707);
	vec.push_back(710);
	vec.push_back(711);
	vec.push_back(712);
	vec.push_back(713);
	vec.push_back(714);
	vec.push_back(715);
	vec.push_back(722);
	vec.push_back(724);
	vec.push_back(726);
	vec.push_back(728);
	vec.push_back(729);
	vec.push_back(732);
	vec.push_back(734);
	vec.push_back(735);
	vec.push_back(737);
	vec.push_back(738);
	vec.push_back(740);
	vec.push_back(741);
	vec.push_back(743);
	vec.push_back(744);
	vec.push_back(745);
	vec.push_back(746);
	vec.push_back(748);
	vec.push_back(751);
	vec.push_back(752);
	vec.push_back(754);
	vec.push_back(756);
	vec.push_back(758);
	vec.push_back(760);
	vec.push_back(763);
	vec.push_back(765);
	vec.push_back(767);
	vec.push_back(769);
	vec.push_back(770);
	vec.push_back(773);
	vec.push_back(775);
	vec.push_back(777);
	vec.push_back(779);
	vec.push_back(781);
	vec.push_back(784);
	vec.push_back(797);
	vec.push_back(798);
	vec.push_back(800);
	vec.push_back(801);
	vec.push_back(802);
	vec.push_back(811);
	vec.push_back(815);
	vec.push_back(817);
	vec.push_back(818);
	vec.push_back(820);
	vec.push_back(821);
	vec.push_back(823);
	vec.push_back(825);
	vec.push_back(827);
	vec.push_back(829);
	vec.push_back(830);
	vec.push_back(832);
	vec.push_back(834);
	vec.push_back(835);
	vec.push_back(837);
	vec.push_back(839);
	vec.push_back(841);
	vec.push_back(842);
	vec.push_back(844);
	vec.push_back(846);
	vec.push_back(848);
	vec.push_back(850);
	vec.push_back(855);
	vec.push_back(879);
	vec.push_back(887);
	vec.push_back(888);
	vec.push_back(890);
	vec.push_back(891);
	vec.push_back(892);
	vec.push_back(894);
	vec.push_back(907);
	vec.push_back(909);
	vec.push_back(911);
	vec.push_back(912);
	vec.push_back(913);
	vec.push_back(917);
	vec.push_back(919);
	vec.push_back(920);
	vec.push_back(921);
	vec.push_back(923);
	vec.push_back(925);
	vec.push_back(928);
	vec.push_back(929);
	vec.push_back(931);
	vec.push_back(933);
	vec.push_back(955);
	vec.push_back(982);
	vec.push_back(985);
	vec.push_back(986);
	vec.push_back(989);
	vec.push_back(992);
	vec.push_back(994);
	vec.push_back(996);
	vec.push_back(1001);
	vec.push_back(1003);
	vec.push_back(1005);
	vec.push_back(1008);
	vec.push_back(1010);
	vec.push_back(1017);
	vec.push_back(1019);
	vec.push_back(1020);
	vec.push_back(1023);
	vec.push_back(1025);
	vec.push_back(1027);
	vec.push_back(1030);
	vec.push_back(1032);
	vec.push_back(1033);
	vec.push_back(1034);
	vec.push_back(1035);
	vec.push_back(1042);
	vec.push_back(1044);
	vec.push_back(1045);
	vec.push_back(1046);
	vec.push_back(1047);
	vec.push_back(1048);
	vec.push_back(1049);
	vec.push_back(1051);
	vec.push_back(1052);
	vec.push_back(1053);
	vec.push_back(1056);
	vec.push_back(1058);
	vec.push_back(1060);
	vec.push_back(1061);
	vec.push_back(1063);
	vec.push_back(1064);
	vec.push_back(1066);
	vec.push_back(1067);
	vec.push_back(1068);
	vec.push_back(1069);
	vec.push_back(1070);
	vec.push_back(1072);
	vec.push_back(1073);
	vec.push_back(1074);
	vec.push_back(1075);
	vec.push_back(1078);
	vec.push_back(1079);
	vec.push_back(1083);
	vec.push_back(1104);
	vec.push_back(1106);
	vec.push_back(1107);
	vec.push_back(1108);
	vec.push_back(1109);
	vec.push_back(1113);
	vec.push_back(1114);
	vec.push_back(1116);
	vec.push_back(1117);
	vec.push_back(1118);
	vec.push_back(1121);
	vec.push_back(1123);
	vec.push_back(1124);
	vec.push_back(1125);
	vec.push_back(1127);
	vec.push_back(1130);
	vec.push_back(1131);
	vec.push_back(1138);
	vec.push_back(1140);
	vec.push_back(1142);
	vec.push_back(1143);
	vec.push_back(1148);
	vec.push_back(1153);
	vec.push_back(1159);
	vec.push_back(1170);
	vec.push_back(1171);
	vec.push_back(1185);
	vec.push_back(1219);
	vec.push_back(1221);
	vec.push_back(1222);
	vec.push_back(1227);
	vec.push_back(1228);
	vec.push_back(1230);
	vec.push_back(1231);
	vec.push_back(1233);
	vec.push_back(1236);
	vec.push_back(1247);
	vec.push_back(1251);
	vec.push_back(1254);
	vec.push_back(1257);
	vec.push_back(1261);
	vec.push_back(1264);
	vec.push_back(1266);
	vec.push_back(1270);
	vec.push_back(1271);
	vec.push_back(1309);
	vec.push_back(1310);
	vec.push_back(1319);
	vec.push_back(1321);
	vec.push_back(1322);
	vec.push_back(1323);
	vec.push_back(1324);
	vec.push_back(1325);
	vec.push_back(1330);
	vec.push_back(1331);
	vec.push_back(1332);


	

	for (size_t i = 0; i < 2000; ++i)
	{
		if (std::binary_search(vec.begin(), vec.end(), i))
			vecSign1.push_back(true);
		else
			vecSign1.push_back(false);
	}
	Compress(vecSign1);


	for (size_t i = 0; i < 100; ++i)
	{
		vecSignBit.push_back(i%2 == 0 ? true : false);
	}

	for (size_t i = 0; i < 1000; ++i)
	{

		vecSignOnePlus.push_back(false);
		vecSignOneMunus.push_back(true);
		vecSignPosPlus.push_back(true);
		vecSignPosMunus.push_back(false);
		vecSignCompressPosPlus.push_back(true);
		vecSignCompressPosMunus.push_back(false);

		
	}

 
	for (size_t i = 0; i < 100; ++i)
	{
		vecSignCompressPosPlus.push_back(false);
		vecSignCompressPosMunus.push_back(true);
	}


	vecSignPosPlus[1] = false;
	vecSignPosPlus[10] = false;
	vecSignPosPlus[151] = false;


	vecSignPosMunus[14] = true;
	vecSignPosMunus[510] = true;
	vecSignPosMunus[777] = true;




 
	Compress(vecSignOnePlus);
	Compress(vecSignOneMunus);
	Compress(vecSignBit);
	Compress(vecSignPosPlus);
	Compress(vecSignPosMunus);
	Compress(vecSignCompressPosPlus);
	Compress(vecSignCompressPosMunus);
}