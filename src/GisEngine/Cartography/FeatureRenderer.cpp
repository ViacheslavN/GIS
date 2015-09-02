#include "StdAfx.h"
#include "FeatureRenderer.h"

namespace GisEngine
{
	namespace Cartography
	{
		CFeatureRenderer::CFeatureRenderer()
		{

		}
		CFeatureRenderer::~CFeatureRenderer()
		{

		}
		//IFeatureRenderer

		bool  CFeatureRenderer::CanRender(GeoDatabase::IFeatureClass* cls, Display::IDisplay* display) const
		{
			if(!m_pSymbolAssigner.get())
				return false;

			if(!m_pSymbolAssigner->CanAssign(cls))
				return false;

			return true;
		}
		void  CFeatureRenderer::PrepareFilter(GeoDatabase::IFeatureClass* cls, GeoDatabase::IQueryFilter* filter) const
		{
			if(!CanRender(cls, 0) || !filter)
				return;

 
			if(m_sShapeField.isEmpty())
			{
				filter->GetFieldSet()->Add(cls->GetShapeFieldName());
				m_nShapeFieldIndex = cls->GetFields()->FindField(cls->GetShapeFieldName());
			}
			else
			{
				if(iswdigit(m_sShapeField[0]))
				{
					int shpIndex = _wtoi(m_sShapeField.cwstr());
					int count = cls->GetFields()->GetFieldCount();
					int shp = -1;
					for(int i = 0; i < count; ++i)
					{
						GeoDatabase::IFieldPtr field = cls->GetFields()->GetField(i);
						if(field->GetType() == GeoDatabase::dtGeometry)
							++shp;
						if(shp == shpIndex)
						{
							m_sShapeField = field->GetName();
							break;
						}
					}
				}

				filter->GetFieldSet()->Add(m_sShapeField);
				m_nShapeFieldIndex = cls->GetFields()->FindField(m_sShapeField);
			}
			if(m_pSymbolAssigner.get())
				m_pSymbolAssigner->PrepareFilter(cls, filter);
		}
		Display::ISymbolPtr CFeatureRenderer::GetSymbolByFeature(GeoDatabase::IFeature* feature) const
		{
			Display::ISymbolPtr pSymbol;

				if(m_pSymbolAssigner.get())
				pSymbol = m_pSymbolAssigner->GetSymbolByFeature(feature);


			return pSymbol;
		}
		ISymbolAssignerPtr	CFeatureRenderer::GetSymbolAssigner() const
		{
			return m_pSymbolAssigner;
		}
		void	CFeatureRenderer::SetSymbolAssigner(ISymbolAssigner* pAssigner)
		{
			m_pSymbolAssigner = pAssigner;
		}
		void CFeatureRenderer::DrawFeature(Display::IDisplay* pDisplay, GeoDatabase::IFeature* feature, Display::ISymbol* pCustomSymbol)
		{
			if(!feature || !m_pSymbolAssigner.get() || !pCustomSymbol)
				return;

			CommonLib::CVariant *pVal = feature->GetValue(m_nShapeFieldIndex);
			if(!pVal)
				return;
			if(pVal->isType<CommonLib::IRefObjectPtr>())
				return;

			CommonLib::IRefObjectPtr pRefObj = pVal->Get<CommonLib::IRefObjectPtr>();

			CommonLib::CGeoShape* pShape =  (CommonLib::CGeoShape*)pRefObj.get();
			if(!pShape)
				return;

			if(pCustomSymbol)
			{
				pCustomSymbol->Prepare(pDisplay);
				pCustomSymbol->Draw(pDisplay, pShape);
				pCustomSymbol->Reset();
				return;

			}

			Display::ISymbolPtr pSymbol = m_pSymbolAssigner->GetSymbolByFeature(feature);
			if(pSymbol.get())
			{
				pSymbol->Prepare(pDisplay);
				pSymbol->Draw(pDisplay, pShape);
				pSymbol->Reset();
			}
		}

		bool CFeatureRenderer::save(CommonLib::IWriteStream *pWriteStream) const
		{
			if(!TBase::save(pWriteStream))
				return false;

			return true;
		}
		bool CFeatureRenderer::load(CommonLib::IReadStream* pReadStream)
		{
			if(!TBase::load(pReadStream))
				return false;

			return true;
		}
		bool CFeatureRenderer::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			if(!TBase::saveXML(pXmlNode))
				return false;

			if(m_pSymbolAssigner.get())
			{
				pXmlNode->AddPropertyBool("SymbolAssigner", true);
				m_pSymbolAssigner->saveXML(pXmlNode);
			}

			return true;
		}
		bool CFeatureRenderer::load(GisCommon::IXMLNode* pXmlNode)
		{
			if(!TBase::load(pXmlNode))
				return false;

			return true;
		}
	}

}