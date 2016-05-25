#include "stdafx.h"
#include "../../EmbDB/ZLibCompressor.h"
#include "../../EmbDB/FixedStringZLibCompressor.h"
void TestStringCompress()
{


	typedef embDB::TBPVector<embDB::sFixedStringVal> TValueMemSet;

	CommonLib::CString str;


	CommonLib::simple_alloc_t alloc;

	embDB::TFixedStringZlibCompressor comp(&alloc, 8192, NULL),comp1(&alloc, 8192, NULL);
	CommonLib::CWriteMemoryStream compressStream;
	CommonLib::FxMemoryReadStream decompressStream;
	uint32 nSize = 0;
	TValueMemSet vecValues, vecValues1;


	CommonLib::CWriteMemoryStream lenCompressStream;
	CommonLib::FxMemoryReadStream  lenDecompresstream;
	embDB::SignedDiffNumLenCompressor232i   lenCompressor, lenCompressor1;
	std::vector<int32> vecLens;
	for (int i =0; i < 80000; ++i)
	{
		str.format(L"adrsdfdggdgasd%d", i);

		int nUtf8 = str.calcUTF8Length() + 1;
		embDB::sFixedStringVal sVal;
		sVal.m_pBuf = (byte*)new char[nUtf8];
		sVal.m_nLen = nUtf8;

		if(nUtf8 > 16)
		{
			int dd =0;
			dd++;
		}
	

		str.exportToUTF8((char*)sVal.m_pBuf, nUtf8);


		vecValues.push_back(sVal);
		comp.AddSymbol(vecValues.size(), i, sVal, vecValues);

		nSize = comp.GetComressSize();

		if(i != 0)
		{
			int32 nLen = vecValues[i].m_nLen - vecValues[i - 1].m_nLen;
			vecLens.push_back(nLen);
			lenCompressor.AddSymbol(nLen);
		}

		if(nSize > 8192)
		{
			comp.CompressBlock(vecValues);
			nSize = comp.GetComressSize();
			if(nSize > 8192)
			{
				lenCompressStream.resize(nSize);
				lenCompressor.BeginEncode(&lenCompressStream);

				for (size_t i = 0; i < vecLens.size(); ++i)
				{
					if(i == 8)
					{
						int dd = 0;
						dd++;
					}
					lenCompressor.EncodeSymbol(vecLens[i], i);
				}
				lenCompressor.EncodeFinish();

				compressStream.resize(nSize);
				comp.compress(vecValues, &compressStream);
				break;
			}
			
		}
	 
	}


	lenDecompresstream.attach(&lenCompressStream, 0, lenCompressStream.pos());

	std::vector<int32> vecLens2;

	lenCompressor1.BeginDecode(&lenDecompresstream);
	int32 nDiffLen = 0;
	for (size_t i = 0; i < vecLens.size(); ++i)
	{
		if(i == 8)
		{
			int dd = 0;
			dd++;
		}

		lenCompressor1.DecodeSymbol(nDiffLen, i);
		vecLens2.push_back(nDiffLen);
	}
	//CommonLib::CWriteFileStream file;
	//file.open(L"D:\\1.data", CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode);
	for (size_t i = 0; i < vecLens.size(); ++i)
	{
	//	file.write(byte(vecLens[i]));
		if(vecLens[i] != vecLens2[i])
		{
			int32 nDiff1 = vecLens[i];
			int32 nDiff2= vecLens2[i];
			int dd = 0;
			dd++;
		}
	}
	


	decompressStream.attach( &compressStream, 0, compressStream.pos());
	comp1.decompress1(vecValues.size(), vecValues1, &decompressStream);


	int dd = 0;
	dd++;
}