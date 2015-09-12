#include "stdafx.h"
#include "XMLDoc.h"
#include "CommonLibrary/FileStream.h"
#include "XMLNode.h"
#include "XMLUtils.h"
namespace GisEngine
{
	namespace GisCommon
	{
		CXMLDoc::CXMLDoc()
		{
			m_char = 0;
			m_token.reserve(1000);
			m_nCurrCol = 1;
			m_nCurrRow = 1;
			m_pRoot = new CXMLNode();
		}
		CXMLDoc::~CXMLDoc()
		{

		}
		const CommonLib::CString& CXMLDoc::GetError() const
		{
			return m_sError;
		}
		bool  CXMLDoc::Open(const CommonLib::CString& xml)
		{
			CommonLib::CReadFileStream fStream;
			if(!fStream.open(xml.cwstr(), CommonLib::ofmOpenExisting, CommonLib::arRead, CommonLib::smNoMode))
				return false;
			return Open((CommonLib::IReadStream*)&fStream);
		}
		bool  CXMLDoc::Open(CommonLib::IReadStream* pStream)
		{

			Clear();
			m_nCurrCol = 1;
			m_nCurrRow = 1;

			IXMLNodePtr pNode;
			enXmlLoadingState state = xlsParseTag;
			//enXmlLoadingState prevState = xlsParseTag;
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
						m_sError.format(L"Open tag waiting, but found: %s, row: %d, col %d", m_token.c_str(), m_nCurrRow, m_nCurrCol);
						Clear();
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
						//prevState = state;
						//state = xlsParseTag;
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
									pNode->SetCDATA(CommonLib::CString(t.c_str()) );
									break;
								}
								//t+= ']';
							}
							if(m_char != ' ')
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
							pNode = m_pRoot->CreateChildNode(m_token.c_str());
						}
						state = xlsParseAttributes;
					}
					break;
				case xlsParseAttributes:
					if( m_token == "/" )
					{
						get_token(pStream);
						if( m_token == ">" )
						{
							pNode = pNode->GetParent();
							state = xlsParseTag;
						}
						else
						{
							m_sError.format(L"Missed close branch in node: %s, row: %d, col %d", pNode->GetName().cstr(), m_nCurrRow, m_nCurrCol);
							Clear();
							return false;
						}
					}            
					else if ( m_token == ">" )
					{
						find_open_tag(pStream );
						state = xlsParseTag;

						if(!m_vecText.empty())
						{
							if(m_vecText.back() != 0)
								m_vecText.push_back(0);
							CommonLib::CString sText;
							utf8_to_utf16((const char *)&m_vecText[0], sText);
							pNode->SetText( sText );
						}

						
					}
					else
					{
						CommonLib::CString sAttrName = m_token.c_str();
						get_token(pStream);
						if( m_token != "=")
						{
							m_sError.format(L"Expected '=' but found: %s, row: %d, col %d", m_token.c_str(), m_nCurrRow, m_nCurrCol);
							Clear();
							return false; 
						}
						get_string(pStream);
						if(!m_vecText.empty())
						{
							if(m_vecText.back() != 0)
								m_vecText.push_back(0);
							CommonLib::CString sText;
							utf8_to_utf16((const char *)&m_vecText[0], sText);
							pNode->AddPropertyString(sAttrName, sText);
						}
					}
					break;
				case  xlsParseCloseName:
					if( CommonLib::CString(m_token.c_str()) != pNode->GetName() )
					{						
						m_sError.format(L"Error not close tag: %s, row: %d, col %d", m_token.c_str(), m_nCurrRow, m_nCurrCol);
						return false;
					}
					else
					{
						pNode = pNode->GetParent();
						while( get_token(pStream) && m_token != ">" ){}

						find_open_tag(pStream );
						if(!m_vecText.empty())
						{
							if(m_vecText.back() != 0)
								m_vecText.push_back(0);
							CommonLib::CString sText;
							utf8_to_utf16((const char *)&m_vecText[0], sText);
							pNode->SetText( sText );
						}
						
						state = xlsParseTag;
					}
					break;
				}
			}
			return true;
		}

		bool CXMLDoc::get_char(CommonLib::IReadStream* pStream)
		{
			if(pStream->IsEndOfStream())
				return false;
			pStream->read(m_char);

			if(m_char=='\n')
			{
				m_nCurrRow++;
				m_nCurrCol = 1;
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
		bool CXMLDoc::find_open_tag(CommonLib::IReadStream* pStream )
		{
			m_vecText.clear();
			while( is_empty_char() && get_char(pStream) );

			m_token = "";
			while( m_char != '<' )
			{
				m_vecText.push_back(m_char);
				if( !get_char(pStream) )
					break;
			}
			m_token = m_char;
			//utils::trim( m_token );
			return pStream->IsEndOfStream();
		}
		bool CXMLDoc::get_string( CommonLib::IReadStream* pStream )
		{
			m_vecText.clear();
			skip_space(pStream);
			while( m_char != '\"' && get_char(pStream) ) ;
			m_token = "";
			while( get_char(pStream) && m_char != '\"' ) 
				m_vecText.push_back(m_char);
			m_char = 0;
			return pStream->IsEndOfStream();
		}
		bool  CXMLDoc::Save(const CommonLib::CString& xml)
		{

			CommonLib::CWriteFileStream fStream;
			if(!fStream.open(xml.cwstr(), CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode))
				return false;
			return Save((CommonLib::IWriteStream*)&fStream);
		}
		bool  CXMLDoc::Save(CommonLib::IWriteStream* pStream)
		{

			pStream->write("<?xml version=\"1.0\"  encoding=\"UTF-8\"?>\n");
	
			for(uint32 i = 0; i < m_pRoot->GetChildCnt(); ++i)
			{
				CXMLNode *pNode = (CXMLNode*)m_pRoot->GetChild(i).get();
				if(pNode)
					pNode->save(pStream);
			}
			return true;
		}
		IXMLNodePtr	CXMLDoc::GetNodes() const
		{
			return m_pRoot;
		}
		 
		void CXMLDoc::Clear()
		{
			CXMLNode *pNode = (CXMLNode*)(m_pRoot.get());
			if(pNode)
				pNode->clear();
		}
	}
}