#ifndef GIS_ENGINE_DISPLAY_BASE_FILL_SYMBOL_
#define GIS_ENGINE_DISPLAY_BASE_FILL_SYMBOL_

#include "SymbolBase.h"
#include "LoaderSymbols.h"

namespace GisEngine
{
	namespace Display
	{

		template< class I>
		class CSymbolFillBase : public CSymbolBase<I>
		{
		public:
			typedef CSymbolBase<I> TSymbolBase;

			CSymbolFillBase(){}
			~CSymbolFillBase(){}

			virtual ILineSymbolPtr GetOutlineSymbol() const
			{
				return m_pBorderSymbol;
			}
			virtual void SetOutlineSymbol(ILineSymbol *pLine)
			{
				m_pBorderSymbol = pLine;
				m_bDirty = true;
			}

			virtual Color GetColor() const
			{
				return m_Brush.GetColor();
			}
			virtual void  SetColor(const Color &color)
			{
				m_Brush.setColor(color);
				m_bDirty = true;
			}
			virtual void  Prepare(IDisplay* pDisplay){}

			void DrawOutline(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount)
			{
				if (m_pBorderSymbol.get())
					m_pBorderSymbol->DrawGeometryEx(pDisplay, points, polyCounts, polyCount);
			}



			bool save(CommonLib::IWriteStream *pWriteStream) const
			{
				TSymbolBase::save(pWriteStream);
				if(m_pBorderSymbol.get())
				{
					pWriteStream->write(true);
					m_pBorderSymbol->save(pWriteStream);
				}
				else
					pWriteStream->write(false);
				m_Brush.save(pWriteStream);
				return true;
			}
			bool load(CommonLib::IReadStream* pReadStream)
			{
				if(!TSymbolBase::load(pReadStream))
					return false;
				bool bSymbol = false;
				SAFE_READ(pReadStream, bSymbol);
				if(bSymbol)
					m_pBorderSymbol = (ILineSymbol*)LoaderSymbol::LoadSymbol(pReadStream).get();

				if(!m_Brush.load(pReadStream))
					return false;

				return true;
			}


			//IXMLSerialize
			bool saveXML(GisCommon::IXMLNode* pXmlNode) const
			{
				TSymbolBase::saveXML(pXmlNode);
				if(m_pBorderSymbol.get())
				{
					GisCommon::IXMLNodePtr pBorderNode = pXmlNode->CreateChildNode(L"BorderSymbol");
					m_pBorderSymbol->saveXML(pBorderNode.get());
				}
				m_Brush.saveXML(pXmlNode);
				return true;
			}
			bool load(GisCommon::IXMLNode* pXmlNode)
			{
				if(!TSymbolBase::load(pXmlNode))
					return false;

				GisCommon::IXMLNodePtr pBorderNode = pXmlNode->GetChild(L"BorderSymbol");
				if(pBorderNode.get())
					m_pBorderSymbol = (ILineSymbol*)LoaderSymbol::LoadSymbol(pBorderNode.get()).get();
				return m_Brush.load(pXmlNode);
			}

		protected:
			ILineSymbolPtr m_pBorderSymbol;
			CBrush m_Brush;

		};
	}
}
#endif