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
			{

				m_nFeatureRendererID = UndefineFeatureRendererID;
			}

			virtual	uint32	GetFeatureRendererID()  const
			{
				return m_nFeatureRendererID;
			}
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
			const CommonLib::CString&  GetShapeField() const
			{
				return m_sShapeField;
			}
			void   SetShapeField(const CommonLib::CString&  field)
			{
				m_sShapeField = field;
			}

			virtual bool save(CommonLib::IWriteStream *pWriteStream) const
			{
				//pWriteStream->write(GetFeatureRendererID());
				
				CommonLib::MemoryStream stream;
				stream.write(m_dMaximumScale);
				stream.write(m_dMinimumScale);
				stream.write(m_sShapeField);
				return true;
			}
			virtual bool load(CommonLib::IReadStream* pReadStream)
			{
				CommonLib::FxMemoryReadStream stream;
				pReadStream->AttachStream(&stream, pReadStream->readIntu32());
				stream.read(m_dMaximumScale);
				stream.read(m_dMinimumScale);
				stream.read(m_sShapeField);
		 		return true;
			}

			virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const
			{
				pXmlNode->AddPropertyInt32U(L"FeatureRendererID", GetFeatureRendererID());
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
			mutable CommonLib::CString		  m_sShapeField;
			mutable int                       m_nShapeFieldIndex;
			uint32 m_nFeatureRendererID;
				
		};

	}

}

#endif