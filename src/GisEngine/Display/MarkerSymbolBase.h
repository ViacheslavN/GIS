#ifndef GIS_ENGINE_DISPLAY_BASE_MARKER_SYMBOL_
#define GIS_ENGINE_DISPLAY_BASE_MARKER_SYMBOL_

#include "SymbolBase.h"
#include "LoaderSymbols.h"
#include "DisplayUtils.h"
namespace GisEngine
{
	namespace Display
	{

		template< class I>
		class CMarkerSymbolBase : public CSymbolBase<I>
		{
		public:
			typedef CSymbolBase<I> TSymbolBase;

			CMarkerSymbolBase()
			{
				m_dAngle = 0;
				m_Color = 0;
				m_dSize = 0;
				m_dOffsetX = 0;
				m_dOffsetY = 0;
				m_dDisplayAngle = 0;
				m_bIgnoreRotation = 0;
				m_dDeviceSize = 0.;
				m_dDeviceOffsetX = 0.;
				m_dDeviceOffsetY = 0.;
				m_dDisplayAngle = 0.;
			}
			~CMarkerSymbolBase(){}

		

			virtual double GetAngle() const
			{
				return m_dAngle;
			}
			virtual void SetAngle(double dAngle)
			{
				m_dAngle = dAngle;
			}

			virtual Color GetColor() const
			{
				return m_Color;
			}
			virtual void SetColor(const Color &color)
			{
				m_Color = color;
			}

			virtual double GetSize() const
			{
				return m_dSize;
			}
			virtual void SetSize(double size)
			{
				m_dSize = size;
			}

			virtual double GetXOffset() const 
			{
				return m_dOffsetX;
			}
			virtual void SetXOffset(double offsetX)
			{
				m_dOffsetX = offsetX;
			}

			virtual double GetYOffset() const 
			{
				return m_dOffsetY;
			}
			virtual void   SetYOffset(double offsetY)
			{
				m_dOffsetY = offsetY;
			}
			virtual bool GetIgnoreRotation() const
			{
				return m_bIgnoreRotation;
			}

			virtual void SetIgnoreRotation(bool gnore)
			{
				m_bIgnoreRotation = gnore;
			}

			virtual void Prepare(IDisplay* pDisplay)
			{
				m_dDeviceSize = (GUnits)floor(SymbolSizeToDeviceSize(pDisplay->GetTransformation().get(), m_dSize, this->GetScaleDependent()) / 2.0);
				m_dDeviceOffsetX = SymbolSizeToDeviceSize(pDisplay->GetTransformation().get(), m_dOffsetX, this->GetScaleDependent());
				m_dDeviceOffsetY = -SymbolSizeToDeviceSize(pDisplay->GetTransformation().get(), m_dOffsetY, this->GetScaleDependent());
				m_dDisplayAngle = m_dAngle + (m_bIgnoreRotation) ? 0 : pDisplay->GetTransformation()->GetRotation();
			}


			bool save(CommonLib::IWriteStream *pWriteStream) const
			{
				CommonLib::CWriteMemoryStream stream;
				TSymbolBase::save(&stream);
				
				stream.write(m_dAngle);
				stream.write(m_dSize);
				stream.write(m_dOffsetX);
				stream.write(m_dOffsetY);
				stream.write(m_bIgnoreRotation);
				
				m_Color.save(&stream);
				pWriteStream->write(&stream);
				return true;
			}
			bool load(CommonLib::IReadStream* pReadStream)
			{
				CommonLib::FxMemoryReadStream stream;
				SAFE_READ(pReadStream->save_read(&stream, true))

				if(!TSymbolBase::load(&stream))
					return false;

				m_dAngle = stream.readDouble();
				m_dSize = stream.readDouble();
				m_dOffsetX = stream.readDouble();
				m_dOffsetY = stream.readDouble();
				m_bIgnoreRotation = stream.readBool();
				m_Color.load(&stream);
				return true;
			}


			//IXMLSerialize
			bool saveXML(GisCommon::IXMLNode* pXmlNode) const
			{
				TSymbolBase::saveXML(pXmlNode);
				pXmlNode->AddPropertyDouble(L"Angle", m_dAngle);
				pXmlNode->AddPropertyDouble(L"Size", m_dSize);
				pXmlNode->AddPropertyDouble(L"XOffset", m_dOffsetX);
				pXmlNode->AddPropertyDouble(L"YOffset", m_dOffsetY);
				pXmlNode->AddPropertyBool(L"IgnoreRotation", m_bIgnoreRotation);
				m_Color.saveXML(pXmlNode);
				return true;
			}
			bool load(const GisCommon::IXMLNode* pXmlNode)
			{
				if(!TSymbolBase::load(pXmlNode))
					return false;

				m_dAngle = pXmlNode->GetPropertyDouble(L"Angle", m_dAngle);
				m_dSize = pXmlNode->GetPropertyDouble(L"Size", m_dSize);
				m_dOffsetX = pXmlNode->GetPropertyDouble(L"XOffset", m_dOffsetX);
				m_dOffsetY = pXmlNode->GetPropertyDouble(L"YOffset", m_dOffsetY);
				m_bIgnoreRotation = pXmlNode->GetPropertyBool(L"IgnoreRotation", m_bIgnoreRotation);
				m_Color.load(pXmlNode);
				return true;
			}

		protected:
			double m_dAngle;
			Color  m_Color;
			double m_dSize;
			double m_dOffsetX;
			double m_dOffsetY;
			double m_dDeviceSize;
			double m_dDeviceOffsetX;
			double m_dDeviceOffsetY;
			double m_dDisplayAngle;
			bool   m_bIgnoreRotation;

		};
	}
}
#endif