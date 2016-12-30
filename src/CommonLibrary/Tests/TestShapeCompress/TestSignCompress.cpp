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