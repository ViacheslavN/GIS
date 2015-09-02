#ifndef GIS_ENGINE_CARTOGRAPHY_FEATURE_RENDERER_BASE_H_
#define GIS_ENGINE_CARTOGRAPHY_FEATURE_RENDERER_BASE_H_
#include "Cartography.h"
 

namespace GisEngine
{
	namespace Cartography
	{

		template< class I>
		class CFeatureRendererBase : public I
		{
		public:
			CFeatureRendererBase() : m_dMaximumScale(0),	 m_dMinimumScale(0),	m_nShapeFieldIndex(-1)	
			{}

			double   GetMaximumScale() const
			{
				return m_dMaximumScale;
			}
			void     SetMaximumScale(double scale)
			{
				m_dMaximumScale = scale;
			}
			double   GetMinimumScale() const
			{
				  return m_dMinimumScale;
			}
			void     SetMinimumScale(double scale)
			{
				m_dMinimumScale = scale;
			}
			const CommonLib::str_t&  GetShapeField() const
			{
				return m_sShapeField;
			}
			void                   SetShapeField(const CommonLib::str_t&  field)
			{
				m_sShapeField = field;
			}

			virtual bool save(CommonLib::IWriteStream *pWriteStream) const
			{
				pWriteStream->write(m_dMaximumScale);
				pWriteStream->write(m_dMinimumScale);
				pWriteStream->write(m_sShapeField);
				return true;
			}
			virtual bool load(CommonLib::IReadStream* pReadStream)
			{
				SAFE_READ(pReadStream, m_dMaximumScale)
				SAFE_READ(pReadStream, m_dMinimumScale)
				SAFE_READ_RES_EX(pReadStream, m_sShapeField, 1)
			
		 
				return true;
			}

			virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const
			{
				pXmlNode->AddPropertyDouble(L"MinScale", m_dMinimumScale);
				pXmlNode->AddPropertyDouble(L"MaxScale", m_dMaximumScale);
				pXmlNode->AddPropertyString(L"ShapeField", m_sShapeField);
				return true;
			}
			virtual bool load(GisCommon::IXMLNode* pXmlNode)
			{
				m_dMinimumScale = pXmlNode->GetPropertyDouble(L"MinScale", m_dMinimumScale);
				m_dMaximumScale = pXmlNode->GetPropertyDouble(L"MaxScale", m_dMaximumScale);
				m_sShapeField = pXmlNode->GetPropertyString(L"name", m_sShapeField);
 				return true;
			}
		protected:
			double                            m_dMinimumScale;
			double                            m_dMaximumScale;
			mutable CommonLib::str_t		  m_sShapeField;
			mutable int                       m_nShapeFieldIndex;
				
		};

	}

}

#endif