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

	pBody->AddPropertyDouble("double", 1.34);
	pBody->AddPropertyBool("bool", true);
	GisEngine::GisCommon::IXMLNodePtr pChild0 =  pBody->CreateChildNode(L"Child0");
	pChild0->AddPropertyInt16(L"int", 10);
	pChild0->AddPropertyString(L"Text", "Текст");
	GisEngine::GisCommon::IXMLNodePtr pChild1 = pBody->CreateChildNode(L"Child1");
	pBody->CreateChildNode(L"Child2");
	for (size_t i = 0; i < 10; ++i)
	{
		GisEngine::GisCommon::IXMLNodePtr pRow = pChild1->CreateChildNode("Row");
		pRow->AddPropertyInt16(L"id", i);
		pRow->AddPropertyInt32(L"td", i + 1);
		pRow->AddPropertyString("text", "row");
		for (size_t j = 0; j < 5; ++j)
		{
			GisEngine::GisCommon::IXMLNodePtr pCell = pRow->CreateChildNode("cell");
			
			CommonLib::str_t stext;
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
	CommonLib::str_t sString;
	uint16 nVal = 30;
	sString.format(L"%u", nVal);
	GisEngine::GisCommon::CXMLDoc xmlDoc;
	xmlDoc.Open(L"D:\\xml\\test.xml");
	xmlDoc.Save(L"D:\\xml\\test2.xml");


}