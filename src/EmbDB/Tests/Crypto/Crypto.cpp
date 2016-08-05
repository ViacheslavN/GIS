// Crypto.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void TestSha256();
void TestAES128();
void TestPageCipher();
int _tmain(int argc, _TCHAR* argv[])
{
	TestPageCipher();
	//TestAES128();
	// TestSha256();
	return 0;
}

