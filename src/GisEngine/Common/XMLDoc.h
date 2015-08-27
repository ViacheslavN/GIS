#ifndef _LIB_GIS_ENGINE_COMMON_XML_DOC_H_
#define _LIB_GIS_ENGINE_COMMON_XML_DOC_H_

#include "GisEngineCommon.h"

namespace GisEngine
{
	namespace GisCommon
	{
		class CXMLDoc : public IXMLDoc
		{
			public:

				enum enXmlLoadingState
				{
					//xlsBeginParse,
					//xlsOpenTag
					xlsParseTag,
					//xlsParseInTag,
					xlsParseName,
					xlsParseCloseName,
					xlsParseAttributes

				};

				CXMLDoc();
				~CXMLDoc();

				virtual bool  Open(const CommonLib::str_t& xml);
				virtual bool  Open(CommonLib::IReadStream* pStream);

				virtual bool  Save(const CommonLib::str_t& xml);
				virtual bool  Save(CommonLib::IWriteStream* pStream);

				virtual IXMLNodePtr			   GetNodes() const;
				virtual void Clear();

				const CommonLib::str_t& GetError() const;
		private:
				//praser
			bool get_char(CommonLib::IReadStream* pStream);
			bool get_token (CommonLib::IReadStream* pStream);
			bool skip_space(CommonLib::IReadStream* pStream);
			bool find_open_tag(CommonLib::IReadStream* pStream );
			bool get_string( CommonLib::IReadStream* pStream );

			bool is_empty_char();
			bool is_escape_symbol();

			void parseName(CommonLib::IReadStream* pStream, enXmlLoadingState& state);

			private:

			char  m_char;
			std::string m_token;
			std::vector<char> m_vecText;
			uint32 m_nCurrCol;
			uint32 m_nCurrRow;
			IXMLNodePtr m_pRoot;
			CommonLib::str_t m_sError;
		};
	}
}
#endif