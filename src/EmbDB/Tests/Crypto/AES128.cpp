#include "stdafx.h"

#include "../../EmbDB/AES128.h"
#include "../../EmbDB/SHA25.h"

void TestAES128()
{

	byte key[32];
	byte plainText[32];
	byte plainText1[32];
	byte CipherText[32];

	std::string keytext("12345678910122344556707886544565654653676747567587586588768756787687698796949469469469996996");
	std::string text("1234567887654321");


	embDB::CAES128 aes128;
	embDB::SHA256 sha256;


	sha256.getHash((byte*)text.c_str(), text.size(), plainText);
	sha256.getHash((byte*)keytext.c_str(), keytext.size(), key);

	aes128.setKey(key, 32);
	aes128.encrypt((byte*)keytext.c_str(), CipherText, 32);
	aes128.decrypt(CipherText, plainText1, 32);

	int dd = 0;
	dd++;
}