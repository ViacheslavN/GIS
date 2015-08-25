#include "stdafx.h"
#include "XMLDoc.h"
#include "CommonLibrary/FileStream.h"
#include "XMLNode.h"
namespace GisEngine
{
	namespace GisCommon
	{
		CXMLDoc::CXMLDoc()
		{
			m_char = 0;
			m_token.reserve(1000);
			m_nCurrCol = 0;
			m_nCurrRow = 0;

			m_pRoot = new CXMLNode();
		}
		CXMLDoc::~CXMLDoc()
		{

		}

		bool  CXMLDoc::Open(const CommonLib::str_t& xml)
		{
			CommonLib::CReadFileStream fStream;
			if(!fStream.open(xml.cwstr(), CommonLib::ofmOpenExisting, CommonLib::arRead, CommonLib::smNoMode))
				return false;
			return Open((CommonLib::IReadStream*)&fStream);
		}
		bool  CXMLDoc::Open(CommonLib::IReadStream* pStream)
		{
			m_nCurrCol = 0;
			m_nCurrRow = 0;
			IXMLNodePtr pParentNode;
			IXMLNodePtr pNode;
			enXmlLoadingState state = xlsParseTag;
			while (get_token(pStream))
			{
				switch(state)
				{
				case xlsParseTag:
					if( m_token == "<" )
					{
						state = xlsParseName;
					}
					else
					{
						return false;
					}
					break;
				case xlsParseName:
					if ( m_token=="/" )
					{
						state = xlsParseCloseName;
					}
					else if ( m_token == "?xml" )
					{
						while( get_token(pStream) && m_token != ">" ){}
						state = xlsParseTag;
					}
					else if ( m_token == "!--" )
					{
						while( get_token(pStream) && m_token != "--" ){}
						while( get_token(pStream) && m_token != ">" ){}
						state = xlsParseTag;
					}
					else if ( m_token == "![CDATA[" )
					{
						std::string t = "";
						do
						{
							if( m_char == ']' )
							{
								if( get_char(pStream) && m_char == ']' )
								{
									pNode->SetCDATA(CommonLib::str_t(t.c_str()) );
									break;
								}
								t+= ']';
							}
							t+= m_char;
						}
						while( get_char(pStream) );
						while( get_token(pStream) && m_token != ">" ){}
						state = xlsParseTag;
					}
					else
					{
						if( pNode.get() )
						{
							IXMLNodePtr  pN(new CXMLNode(pNode.get(), m_token.c_str()));
							pNode->AddChildNode(pN.get());
							pNode = pN;
						}
						else
						{
							m_pRoot->AddChildNode(pNode.get());
						}
						state = xlsParseAttributes;
					}
				case xlsParseAttributes:
					/*if( m_token == "/" )
					{
						get_token(pStream);
						if( m_token == ">" )
						{
							pNode = pNode->GetParent();
							state = xlsParseTag;
						}
						else
						{
							return false;
						}
					}            
					else if ( m_token == ">" )
					{
						find_open_tag( );
						state = xlsParseTag;
						pNode->set_text( m_token );
					}
					else
					{
						cXmlNode* i = pNode->create_attrib( m_token );
						get_token();
						if( m_token != "=")
						{
							Error("Xml::Expected '=' but found %s\n", m_token.c_str());
							delete pRoot;
							return NULL;//Assert(true, "Closed tag!");
						}
						get_string();
						i->set_text( m_token );
					}*/
					break;
				case  xlsParseCloseName:
					break;
				}
			}
			return true;
		}

		bool CXMLDoc::get_char(CommonLib::IReadStream* pStream)
		{
			
			pStream->read(m_char);

			if(m_char=='\n')
			{
				m_nCurrRow++;
				m_nCurrCol = 0;
			}
			else
				m_nCurrCol++;

			return !pStream->IsEndOfStream();
		}
		bool CXMLDoc::get_token (CommonLib::IReadStream* pStream)
		{
			m_token = "";
			skip_space(pStream);
			m_token	= m_char;
			if( !is_escape_symbol() )
			{
				while( get_char(pStream) && !is_escape_symbol() ) 
					m_token += m_char;
			}
			else
			{
				return get_char(pStream);
			}
			return !pStream->IsEndOfStream();
		}


		bool CXMLDoc::skip_space(CommonLib::IReadStream* pStream)
		{
			while( is_empty_char() && get_char(pStream));
			return pStream->IsEndOfStream();
		}

		bool CXMLDoc::is_empty_char()
		{
			return m_char == 0 || m_char == ' ' ||  m_char=='\n' || m_char=='\r'|| m_char=='\t';
		}

		bool CXMLDoc::is_escape_symbol( )
		{
			return m_char == '<' || m_char == '"'|| m_char=='>'|| m_char==' '|| m_char == '='|| m_char=='/';
		}

		bool  CXMLDoc::Save(const CommonLib::str_t& xml)
		{

			CommonLib::CWriteFileStream fStream;
			if(!fStream.open(xml.cwstr(), CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode))
				return false;
			return Save((CommonLib::IWriteStream*)&fStream);
		}
		bool  CXMLDoc::Save(CommonLib::IWriteStream* pStream)
		{

			pStream->write("<?xml version=\"1.0\"  encoding=\"utf-8\"?>\n");
	
			for(uint32 i = 0; i < m_pRoot->GetChildCnt(); ++i)
			{
				CXMLNode *pNode = (CXMLNode*)m_pRoot->GetChild(i).get();
				if(pNode)
					pNode->save(pStream);
			}
			return true;
		}
		IXMLNodePtr	CXMLDoc::GetRoot() const
		{
			return m_pRoot;
		}
		void CXMLDoc::SetRoot(IXMLNode* pNode)
		{
			m_pRoot = pNode;
		}
	}
}