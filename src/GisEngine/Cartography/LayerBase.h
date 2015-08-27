#ifndef GIS_ENGINE_CARTOGRAPHY_LAYER_BASE_H_
#define GIS_ENGINE_CARTOGRAPHY_LAYER_BASE_H_

#include "Cartography.h"

namespace GisEngine
{
	namespace Cartography
	{

		template< class I>
		class CLayerBase : public I
		{
		public: 
			 virtual void   Draw(eDrawPhase phase, Display::IDisplay* pDisplay, GisCommon::ITrackCancel* trackCancel)
			 {
				 if(!display)
					 return;

				 if(!IsValid() || !(GetSupportedDrawPhases() & phase) || !GetVisible())
					 return;

				 double scale = display->GetTransformation()->GetScale();

				 double maximumScale = GetMaximumScale();
				 double minimumScale = GetMinimumScale();
				 if((maximumScale != 0.0 && scale < maximumScale) || (minimumScale != 0.0 && scale > minimumScale))
					 return;

				 DrawEx(phase, display, trackCancel);
			 }
			virtual double                    GetMaximumScale() const
			{
				return m_dMaximumScale;
			}
			virtual void                      SetMaximumScale(double dScale)
			{
				m_dMaximumScale = dScale;
			}
			virtual double                    GetMinimumScale() const
			{
				return m_dMinimumScale;
			}
			virtual void                      SetMinimumScale(double dScale)
			{
				m_dMinimumScale = dScale;
			}
			virtual CommonLib::str_t		 GetName() const
			{
				return m_sName;
			}
			virtual void   SetName(const CommonLib::str_t& sName)
			{
				m_sName = sName;
			}
			virtual bool                      GetVisible() const
			{
				return m_bVisible;
			}
			virtual void                      SetVisible(bool flag)
			{
				m_bVisible = flag;
			}
			virtual bool                      IsActiveOnScale(double scale) const
			{
				if((GetMinimumScale() != 0 && GetMinimumScale() < scale) || (GetMaximumScale() != 0 && GetMaximumScale() > scale))
					return false;
				return true;
			}

			virtual bool save(CommonLib::IWriteStream *pWriteStream) const
			{
				pWriteStream->write(m_sName);
				pWriteStream->write(m_bVisible);
				pWriteStream->write(m_dMinimumScale);
				pWriteStream->write(m_dMaximumScale);
				return true;
			}
			virtual bool load(CommonLib::IReadStream* pReadStream)
			{
				SAFE_READ_RES_EX(pReadStream, m_sName, 1)
				SAFE_READ_RES_EX(pReadStream, m_bVisible, 1)
				SAFE_READ(pReadStream, m_dMinimumScale)
				SAFE_READ(pReadStream, m_dMaximumScale)
				return true;
			}

			virtual bool saveXML(IXMLNode* pXmlNode) const
			{
				pXmlNode->AddPropertyString(L"name", m_sName);
				pXmlNode->AddPropertyBool(L"visible", m_bVisible);
				pXmlNode->AddPropertyDouble(L"MinScale", m_dMinimumScale);
				pXmlNode->AddPropertyDouble(L"MaxScale", m_dMaximumScale);
				return true;
			}
			virtual bool load(IXMLNode* pXmlNode)
			{

				m_sName = pXmlNode->GetPropertyString(L"name", m_sName);
				m_bVisible = pXmlNode->GetPropertyBool(L"visible", m_bVisible);
				m_dMinimumScale = pXmlNode->GetPropertyDouble(L"MinScale", m_dMinimumScale);
				m_dMaximumScale = pXmlNode->GetPropertyDouble(L"MaxScale", m_dMaximumScale);
				return true;
			}

		protected:
			virtual void                      DrawEx(eDrawPhase phase, Display::IDisplay* display, gisSystem::ITrackCancel* trackCancel) = 0;
		protected:
			CommonLib::str_t              m_sName;
			GisGeometry::ISpatialReferencePtr m_pSpatialRef;
			bool                              m_bVisible;
			double                            m_dMinimumScale;
			double                            m_dMaximumScale;

	}
}


#endif