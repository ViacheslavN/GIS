#ifndef GIS_ENGINE_CARTOGRAPHY_SIMPLE_SYMBOL_ASSIGNER_
#define GIS_ENGINE_CARTOGRAPHY_SIMPLE_SYMBOL_ASSIGNER_

#include "Cartography.h"


namespace GisEngine
{
	namespace Cartography
	{
		class    CSimpleSymbolAssigner : public ISimpleSymbolAssigner, public ILegendInfo
		{
		public:
			CSimpleSymbolAssigner();
			virtual ~CSimpleSymbolAssigner();

		private:
			CSimpleSymbolAssigner(const CSimpleSymbolAssigner&);
			CSimpleSymbolAssigner& operator=(const CSimpleSymbolAssigner&);

		public:
			 
			virtual uint32				   GetSymbolAssignerID() const {return SimpleSymbolAssignerID;} 
			virtual bool                   CanAssign(GeoDatabase::IFeatureClass* cls) const;
			virtual void                   PrepareFilter(GeoDatabase::IFeatureClass* cls, GeoDatabase::IQueryFilter* filter) const;
			virtual Display::ISymbolPtr	   GetSymbolByFeature(GeoDatabase::IFeature* feature) const;
			virtual void                   SetupSymbols(Display::IDisplay* display);
			virtual void                   ResetSymbols();

			// ISimpleSymbolAssigner
			virtual const CommonLib::CString&	  GetDescription() const;
			virtual void					  SetDescription(const CommonLib::CString& desc);
			virtual const CommonLib::CString&  GetLabel() const;
			virtual void                     SetLabel(const CommonLib::CString& label);
			virtual Display::ISymbolPtr		 GetSymbol() const;
			virtual void                     SetSymbol(Display::ISymbol* symbol);
			virtual void				     FlushBuffers(Display::IDisplay* pDisplay, GisCommon::ITrackCancel* trackCancel);
			// ILegendInfo

			virtual int                    GetSymbolCount() const;	
			virtual Display::ISymbolPtr	   GetSymbolByIndex(int index) const;
			virtual void                   SetSymbolByIndex(int index, Display::ISymbol *symbol );

	
			//IStreamSerialize
			virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
			virtual bool load(CommonLib::IReadStream* pReadStream);


			//IXMLSerialize
			virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
			virtual bool load(const GisCommon::IXMLNode* pXmlNode);

		
		private:
			CommonLib::CString	m_sLabel;
			CommonLib::CString	m_sDescription;
			Display::ISymbolPtr m_pSymbol;

		
		};

	}
}

#endif
