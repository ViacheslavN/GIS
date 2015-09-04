#include "stdafx.h"
#include "SimpleSymbolAssigner.h"
#include "Display/LoaderSymbols.h"
namespace GisEngine
{
	namespace Cartography
	{
		CSimpleSymbolAssigner::CSimpleSymbolAssigner()
		{

		}
		CSimpleSymbolAssigner::~CSimpleSymbolAssigner()
		{

		}

		bool  CSimpleSymbolAssigner::CanAssign(GeoDatabase::IFeatureClass* cls) const
		{
			if(!cls)
				return false;

			return true;
		}
		void   CSimpleSymbolAssigner::PrepareFilter(GeoDatabase::IFeatureClass* cls, GeoDatabase::IQueryFilter* filter) const
		{
			if(!CanAssign(cls) || !filter)
				return;

			filter->GetFieldSet()->Add(cls->GetOIDFieldName());
		}
		Display::ISymbolPtr	   CSimpleSymbolAssigner::GetSymbolByFeature(GeoDatabase::IFeature* feature) const
		{
			  return m_pSymbol;
		}
		void  CSimpleSymbolAssigner::SetupSymbols(Display::IDisplay* pDisplay)
		{
			if(pDisplay && m_pSymbol.get())
				m_pSymbol->Prepare(pDisplay);
		}
		void   CSimpleSymbolAssigner::ResetSymbols()
		{
			if(m_pSymbol.get())
				m_pSymbol->Reset();
		}

		const CommonLib::str_t&	  CSimpleSymbolAssigner::GetDescription() const
		{
			 return m_sDescription;
		}
		void  CSimpleSymbolAssigner::SetDescription(const CommonLib::str_t& desc)
		{
			m_sDescription = desc;
		}
		const CommonLib::str_t&  CSimpleSymbolAssigner::GetLabel() const
		{
			return m_sLabel;
		}
		void  CSimpleSymbolAssigner::SetLabel(const CommonLib::str_t& label)
		{
			m_sLabel = label;
		}
		Display::ISymbolPtr		 CSimpleSymbolAssigner::GetSymbol() const
		{
			return m_pSymbol;
		}
		void   CSimpleSymbolAssigner::SetSymbol(Display::ISymbol* symbol)
		{
			 m_pSymbol = symbol;
		}
		void  CSimpleSymbolAssigner::FlushBuffers(Display::IDisplay* pDisplay, GisCommon::ITrackCancel* trackCancel)
		{
			if(m_pSymbol.get())
				m_pSymbol->FlushBuffers(pDisplay, trackCancel);
		}
		/*ILegendGroupPtr SimpleSymbolAssigner::GetLegendGroup( int index ) const
		{
			if ( index == 0 )
			{    
				ILegendGroupPtr ptrGroup = LegendGroup::CreateInstance();
				ptrGroup->AddClass( LegendClass::CreateInstance( GetLabel(), 
					symbol_.get(), static_cast<gisDisplay::ISymbolContainer *>(const_cast<ThisType*>(this)) , 0 ).get() );
				return ptrGroup;
			}
			return ILegendGroupPtr();
		}
		int  SimpleSymbolAssigner::GetLegendGroupCount() const
		{
			 return 1;
		}
		*/

		int  CSimpleSymbolAssigner::GetSymbolCount() const
		{
			  return 1;
		}
		Display::ISymbolPtr	   CSimpleSymbolAssigner::GetSymbolByIndex(int index) const
		{
			if ( index == 0 )
				return m_pSymbol;
			return Display::ISymbolPtr();
		}
		void   CSimpleSymbolAssigner::SetSymbolByIndex(int index, Display::ISymbol *symbol )
		{
			if ( index == 0 )
				m_pSymbol = symbol;
		}


		//IStreamSerialize
		bool CSimpleSymbolAssigner::save(CommonLib::IWriteStream *pWriteStream) const
		{
			pWriteStream->write(GetSymbolAssignerID());
			pWriteStream->write(m_sLabel);
			pWriteStream->write(m_sDescription);
			pWriteStream->write(m_pSymbol.get() ? true : false);
			if(m_pSymbol.get())
				m_pSymbol->save(pWriteStream);
			return true;
		}
		bool CSimpleSymbolAssigner::load(CommonLib::IReadStream* pReadStream)
		{
			bool bSymbol = false;
			SAFE_READ_BOOL_RES(pReadStream, bSymbol);
			SAFE_READ_EX(pReadStream, m_sLabel, 1);
			SAFE_READ_EX(pReadStream, m_sDescription, 1);
			if(bSymbol)
				m_pSymbol = Display::LoadSymbol(pReadStream);
			return true;
		}


		//IXMLSerialize
		bool CSimpleSymbolAssigner::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			pXmlNode->AddPropertyInt32U(L"SymbolID",  GetSymbolAssignerID());
			pXmlNode->AddPropertyString(L"Label",  m_sLabel);
			pXmlNode->AddPropertyString(L"Description",  m_sDescription);
			pXmlNode->AddPropertyBool(L"symbol", m_pSymbol.get() ? true : false);
			if(m_pSymbol.get())
			{
				GisCommon::IXMLNodePtr pSymbolNode = pXmlNode->CreateChildNode(L"Symbol");
				m_pSymbol->saveXML(pSymbolNode.get());
			}
			return true;
		}
		bool CSimpleSymbolAssigner::load(GisCommon::IXMLNode* pXmlNode)
		{
	 
			m_sLabel = pXmlNode->GetPropertyString(L"Label",  m_sLabel);
			m_sDescription = pXmlNode->GetPropertyString(L"Description",  m_sDescription);

			bool bSymbol = pXmlNode->GetPropertyBool(L"symbol", false);
			if(bSymbol)
			{
				GisCommon::IXMLNodePtr pSymbolNode = pXmlNode->GetChild(L"Symbol");
				if(pSymbolNode.get())
					m_pSymbol = Display::LoadSymbol(pSymbolNode.get());

			}
			return true;
		}
	}
}