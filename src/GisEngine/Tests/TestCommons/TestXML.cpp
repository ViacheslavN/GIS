#include "stdafx.h"
#include "CommonLibrary/stream.h"
#include "CommonLibrary/FileStream.h"
#include "Common/GisEngineCommon.h"
#include "Common/XMLDoc.h"

void testWriteXML();
void testLoadXML();
void TestXML()
{
	testWriteXML();
	testLoadXML();
}
 
void testWriteXML()
{
	GisEngine::GisCommon::CXMLDoc xmlDoc;
	GisEngine::GisCommon::IXMLNodePtr pRoot = xmlDoc.GetNodes();

	GisEngine::GisCommon::IXMLNodePtr pBody = pRoot->CreateChildNode(L"Body");
	CommonLib::MemoryStream memStream;

	memStream.write(int64(456));
	memStream.write(int(457));
	memStream.write("строка");
	memStream.write(L"Юникоде");

	CommonLib::CBlob blob(memStream.size(), 0);
	blob.copy(memStream.buffer(), memStream.size());


	pBody->AddPropertyDouble("double", 1.34);
	pBody->AddPropertyBool("bool", true);
	GisEngine::GisCommon::IXMLNodePtr pChild0 =  pBody->CreateChildNode(L"Child0");
	pChild0->SetBlobCDATA(blob);
	pChild0->AddPropertyInt16(L"int", 10);
	pChild0->AddPropertyString(L"Text", "Текст");
	GisEngine::GisCommon::IXMLNodePtr pChild1 = pBody->CreateChildNode(L"Child1");
	pBody->CreateChildNode(L"Child2");
	for (size_t i = 0; i < 10; ++i)
	{
		GisEngine::GisCommon::IXMLNodePtr pRow = pChild1->CreateChildNode("Row");
		pRow->SetText(L"Текст Row");
		pRow->AddPropertyInt16(L"id", i);
		pRow->AddPropertyInt32(L"td", i + 1);
		pRow->AddPropertyString("text", "row");
		for (size_t j = 0; j < 1; ++j)
		{
			GisEngine::GisCommon::IXMLNodePtr pCell = pRow->CreateChildNode("cell");
			
			CommonLib::CString stext;
			pCell->AddPropertyInt32U(L"ID", j);
			stext.format(L"Идентификатор=%d", j);
			pCell->SetText(stext);
		}
	}
	xmlDoc.Save(L"D:\\xml\\test.xml");
}
void testLoadXML()
{

	/*CommonLib::CReadFileStream fStream;
	fStream.open(L"D:\\xml\\1.xml", CommonLib::ofmOpenExisting, CommonLib::arRead, CommonLib::smNoMode);

	char Ch;

	fStream.read(Ch);*/
	CommonLib::CString sString;
	uint16 nVal = 30;
	sString.format(L"%u", nVal);
	GisEngine::GisCommon::CXMLDoc xmlDoc;
	if(xmlDoc.Open(L"D:\\xml\\test.xml"))
	{
		GisEngine::GisCommon::IXMLNodePtr pRoot = xmlDoc.GetNodes();
		GisEngine::GisCommon::IXMLNodePtr pBody = pRoot->GetChild(L"Body");
		GisEngine::GisCommon::IXMLNodePtr pChild0 =  pBody->GetChild(L"Child0");
		if(pChild0.get())
		{
			CommonLib::CBlob blob;
			pChild0->GetBlobCDATA(blob);

			CommonLib::FxMemoryReadStream memStream;
			memStream.attach(blob.buffer(), blob.size());
			int64 nVal;
			int intVal;
			memStream.read(nVal);
			memStream.read(intVal);


		}
		xmlDoc.Save(L"D:\\xml\\test2.xml");
	}



	


}