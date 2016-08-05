#include "stdafx.h"

 
#include "../../EmbDB/PageCipher.h"
#include "../../EmbDB//CRC.h"
#include "CommonLibrary/FixedMemoryStream.h"

void TestPageCipher()
{
	CommonLib::simple_alloc_t alloc;
	embDB::CFilePage page(&alloc, 8192, 55);
	embDB::CFilePage page1(&alloc, 8192, 55);
	std::string sPWD = "1223345";
	std::string sSalt =  "dfgjfkbjigjg45840ujdfis34bj5j409bjkr55665667";
	std::string sSalt1 = "1111234tr45gjfkbjigjg45840ujdfis34bj5j409bjk";
	embDB::CPageCipher pageCipher((byte*)sPWD.c_str(), sPWD.length(), (byte*)sSalt.c_str(), (byte*)sSalt1.c_str(), sSalt.length(), embDB::AES128_ALG);


	CommonLib::FxMemoryWriteStream stream;
	stream.attachBuffer(page.getRowData(), page.getPageSize());

	byte b = 0;
	for (uint32 i = 0; i < page.getPageSize(); ++i)
	{
		if(b > 254)
			b = 0;

		stream.write(b);
		b++;
	}

	uint32 nCrc32 = embDB::Crc32(page.getRowData(), page.getPageSize());

	pageCipher.encrypt(&page);
	pageCipher.decrypt(&page);

	uint32 nCrc32_1 = embDB::Crc32(page.getRowData(), page.getPageSize());
	pageCipher.encrypt(&page, page1.getRowData(), 8182);
}