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

			CLayerBase() : m_bVisible(false), m_dMinimumScale(0.), m_dMaximumScale(0.), m_nLayerSymbolID(UndefineLayerID),
					m_nCheckCancelStep(100)
			{}

			~CLayerBase()
			{}

			virtual	uint32	GetLayerID() const {return m_nLayerSymbolID;}
			 virtual void   Draw(eDrawPhase phase, Display::IDisplay* pDisplay, GisCommon::ITrackCancel* trackCancel)
			 {
				 if(!pDisplay)
					 return;

				 if(!IsValid() || !(GetSupportedDrawPhases() & phase) || !GetVisible())
					 return;

				 double scale = pDisplay->GetTransformation()->GetScale();

				 double maximumScale = GetMaximumScale();
				 double minimumScale = GetMinimumScale();
				 if((maximumScale != 0.0 && scale < maximumScale) || (minimumScale != 0.0 && scale > minimumScale))
					 return;

				 DrawEx(phase, pDisplay, trackCancel);
			 }
			virtual double   GetMaximumScale() const
			{
				return m_dMaximumScale;
			}
			virtual void  SetMaximumScale(double dScale)
			{
				m_dMaximumScale = dScale;
			}
			virtual double   GetMinimumScale() const
			{
				return m_dMinimumScale;
			}
			virtual void  SetMinimumScale(double dScale)
			{
				m_dMinimumScale = dScale;
			}
			virtual const CommonLib::CString&		GetName() const
			{
				return m_sName;
			}
			virtual void   SetName(const CommonLib::CString& sName)
			{
				m_sName = sName;
			}
			virtual bool  GetVisible() const
			{
				return m_bVisible;
			}
			virtual void  SetVisible(bool flag)
			{
				m_bVisible = flag;
			}
			virtual bool  IsActiveOnScale(double scale) const
			{
				if((GetMinimumScale() != 0 && GetMinimumScale() < scale) || (GetMaximumScale() != 0 && GetMaximumScale() > scale))
					return false;
				return true;
			}
			virtual uint32	GetCheckCancelStep() const
			{
				return m_nCheckCancelStep;
			}
			virtual void	SetCheckCancelStep(uint32 nCount)
			{
				m_nCheckCancelStep = nCount;
			}
			virtual bool save(CommonLib::IWriteStream *pWriteStream) const
			{

				CommonLib::MemoryStream stream;
				stream.write(GetLayerID());
				stream.write(m_sName);
				stream.write(m_bVisible);
				stream.write(m_dMinimumScale);
				stream.write(m_dMaximumScale);
				stream.write(m_nCheckCancelStep);

				pWriteStream->write(&stream);
				return true;
			}
			virtual bool load(CommonLib::IReadStream* pReadStream)
			{
				CommonLib::FxMemoryReadStream stream;
				pReadStream->AttachStream(&stream, pReadStream->readIntu32());

				stream.read(m_sName);
				stream.read(m_bVisible);
				stream.read(m_dMinimumScale);
				stream.read(m_dMaximumScale);
				stream.read(m_nCheckCancelStep);
				return true;
			}

			virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const
			{
				pXmlNode->AddPropertyInt32U(L"LayerID", GetLayerID());
				pXmlNode->AddPropertyString(L"name", m_sName);
				pXmlNode->AddPropertyBool(L"visible", m_bVisible);
				pXmlNode->AddPropertyDouble(L"MinScale", m_dMinimumScale);
				pXmlNode->AddPropertyDouble(L"MaxScale", m_dMaximumScale);
				pXmlNode->AddPropertyInt32U(L"CheckCancelStep", m_nCheckCancelStep);
				return true;
			}
			virtual bool load(GisCommon::IXMLNode* pXmlNode)
			{
				m_sName = pXmlNode->GetPropertyString(L"name", m_sName);
				m_bVisible = pXmlNode->GetPropertyBool(L"visible", m_bVisible);
				m_dMinimumScale = pXmlNode->GetPropertyDouble(L"MinScale", m_dMinimumScale);
				m_dMaximumScale = pXmlNode->GetPropertyDouble(L"MaxScale", m_dMaximumScale);
				m_nCheckCancelStep = pXmlNode->GetPropertyInt32U(L"CheckCancelStep", m_nCheckCancelStep);
				return true;
			}

		protected:
			virtual void   DrawEx(eDrawPhase phase, Display::IDisplay* display, GisCommon::ITrackCancel* trackCancel) = 0;
		protected:
			CommonLib::CString              m_sName;
			GisGeometry::ISpatialReferencePtr m_pSpatialRef;
			bool                              m_bVisible;
			double                            m_dMinimumScale;
			double                            m_dMaximumScale;
			uint32							  m_nLayerSymbolID;
			uint32							  m_nCheckCancelStep;
		};

	}
}

#endif

 