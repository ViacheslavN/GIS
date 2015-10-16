#ifndef GIS_ENGINE_DISPLAY_BASE_MARKER_SYMBOL_
#define GIS_ENGINE_DISPLAY_BASE_MARKER_SYMBOL_

#include "SymbolBase.h"
#include "LoaderSymbols.h"

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
				m_dXOffset = 0;
				m_dYOffset = 0;
				m_dDisplayAngle = 0;
				m_bIgnoreRotation = 0;
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
				return m_dXOffset;
			}
			virtual void SetXOffset(double offsetX)
			{
				m_dXOffset = offsetX;
			}

			virtual double GetYOffset() const 
			{
				return m_dYOffset;
			}
			virtual void   SetYOffset(double offsetY)
			{
				m_dYOffset = offsetY;
			}
			virtual bool GetIgnoreRotation() const
			{
				return m_bIgnoreRotation;
			}

			virtual void SetIgnoreRotation(bool gnore)
			{
				m_bIgnoreRotation = gnore;
			}

			bool save(CommonLib::IWriteStream *pWriteStream) const
			{
				CommonLib::MemoryStream stream;
				TSymbolBase::save(&stream);
				
				stream.write(m_dAngle);
				stream.write(m_dSize);
				stream.write(m_dXOffset);
				stream.write(m_dYOffset);
				stream.write(m_dDisplayAngle);
				stream.write(m_bIgnoreRotation);
				
				m_Color.save(stream);
				pWriteStream->write(&stream);
				return true;
			}
			bool load(CommonLib::IReadStream* pReadStream)
			{
				CommonLib::FxMemoryReadStream stream;
				pReadStream->AttachStream(&stream, pReadStream->readIntu32());

				if(!TSymbolBase::load(&stream))
					return false;

				m_dAngle = stream.readDouble();
				m_dSize = stream.readDouble();
				m_dXOffset = stream.readDouble();
				m_dYOffset = stream.readDouble();
				m_dDisplayAngle = stream.readDouble();
				m_bIgnoreRotation = stream.readBool();
				m_Color.load(stream);
				return true;
			}


			//IXMLSerialize
			bool saveXML(GisCommon::IXMLNode* pXmlNode) const
			{
				TSymbolBase::saveXML(pXmlNode);
				pXmlNode->AddPropertyDouble(L"Angle", m_dAngle);
				pXmlNode->AddPropertyDouble(L"Size", m_dSize);
				pXmlNode->AddPropertyDouble(L"XOffset", m_dXOffset);
				pXmlNode->AddPropertyDouble(L"YOffset", m_dYOffset);
				pXmlNode->AddPropertyDouble(L"DisplayAngle", m_dDisplayAngle);
				pXmlNode->AddPropertyBool(L"IgnoreRotation", m_bIgnoreRotation);
				m_Color.save(pXmlNode);
				return true;
			}
			bool load(const GisCommon::IXMLNode* pXmlNode)
			{
				if(!TSymbolBase::load(pXmlNode))
					return false;

				m_dAngle = pXmlNode->GetPropertyDouble(L"Angle", m_dAngle);
				m_dSize = pXmlNode->GetPropertyDouble(L"Size", m_dSize);
				m_dXOffset = pXmlNode->GetPropertyDouble(L"XOffset", m_dXOffset);
				m_dYOffset = pXmlNode->GetPropertyDouble(L"YOffset", m_dYOffset);
				m_dDisplayAngle = pXmlNode->GetPropertyDouble(L"DisplayAngle", m_dDisplayAngle);
				m_bIgnoreRotation = pXmlNode->GetPropertyBool(L"IgnoreRotation", m_bIgnoreRotation);
				m_Color.load(pXmlNode);
				return true;
			}

		protected:
			double m_dAngle;
			Color  m_Color;
			double m_dSize;
			double m_dXOffset;
			double m_dYOffset;
			double m_dDisplayAngle;
			bool   m_bIgnoreRotation;

		};
	}
}
#endif