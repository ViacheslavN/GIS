#include "stdafx.h"
#include "SimpleFillSymbol.h"

namespace GisEngine
{
	namespace Display
	{
		CSimpleFillSymbol::CSimpleFillSymbol() : m_FillStyle(SimpleFillStyleSolid)
		{
			m_nSymbolID = SimpleFillSymbolID;
			m_Pen.setPenType(PenTypeNull);
		}
		CSimpleFillSymbol::~CSimpleFillSymbol()
		{

		}




		//ISimpleFillSymbol
		eSimpleFillStyle CSimpleFillSymbol::GetStyle() const
		{
			return m_FillStyle;
		}
		void CSimpleFillSymbol::SetStyle(eSimpleFillStyle style)
		{
			m_FillStyle = style;
			m_Brush.setType(FillStyle2BrushType(style));
			m_bDirty = true;
		}
		Color CSimpleFillSymbol::GetBackgroundColor() const
		{
			return m_Brush.GetBgColor();
		}
		void  CSimpleFillSymbol::SetBackgroundColor( Color color )
		{
			m_Brush.setBgColor(color);
			m_bDirty = true;
		}

		void CSimpleFillSymbol::Prepare(IDisplay* pDisplay)
		{
			TSymbolBase::Prepare(pDisplay);

 			if(m_Brush.GetType() == BrushTypeTextured)
			{
				m_pBitmap.reset(new CBitmap(8, 8, BitmapFormatType32bppARGB));
				unsigned char* bits = m_pBitmap->bits();
				for(int i = 0; i < 8; i++)
				{
					for(int j = 0; j < 8; j++)
					{
						Color c = m_Brush.GetBgColor();
						switch(m_FillStyle)
						{
						case SimpleFillStyleHorizontal:
							if(i == 0)
								c = m_Brush.GetColor();
							break;
						case SimpleFillStyleVertical:
							if(j == 0)
								c =  m_Brush.GetColor();
							break;
						case SimpleFillStyleForwardDiagonal:
							if(i == 7 - j)
								c =  m_Brush.GetColor();
							break;
						case SimpleFillStyleBackwardDiagonal:
							if(i == j)
								c =  m_Brush.GetColor();
							break;
						case SimpleFillStyleCross:
							if(i == 0 || j == 0)
								c =  m_Brush.GetColor();
							break;
						case SimpleFillStyleDiagonalCross:
							if(i == 0 || i == 7 - j)
								c =  m_Brush.GetColor();
							break;
						}
						bits[i * 8 * 4 + j * 4] = c.GetB();
						bits[i * 8 * 4 + j * 4 + 1] = c.GetG();;
						bits[i * 8 * 4 + j * 4 + 2] = c.GetR();;
						bits[i * 8 * 4 + j * 4 + 3] = c.GetA();;
					}
				}

				m_Brush.setTexture(m_pBitmap.get());
			}
		}

		void  CSimpleFillSymbol::DrawGeometryEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount)
		{
			IGraphicsPtr pGraphics = pDisplay->GetGraphics();
			if(polyCount == 1)
				pGraphics->DrawPolygon(&m_Pen, &m_Brush, points, polyCounts[0]);
			else
				pGraphics->DrawPolyPolygon(&m_Pen, &m_Brush, points, polyCounts, (int)polyCount);

			DrawOutline(pDisplay, points, polyCounts, polyCount);
		}
		void CSimpleFillSymbol::FillRect(IDisplay* pDisplay, const Display::GRect& rect)
		{
			IGraphicsPtr pGraphics = pDisplay->GetGraphics();

			pGraphics->DrawRect(&m_Pen, &m_Brush, rect);
		}
		 void CSimpleFillSymbol::DrawDirectly(IDisplay* display, const GPoint* lpPoints, const int *lpPolyCounts, int nCount )
		{
			DrawGeometryEx(display, lpPoints, lpPolyCounts, nCount);
		}
		void  CSimpleFillSymbol::QueryBoundaryRectEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount,   GRect &rect) const
		{
			for(size_t part = 0, offset = 0; part < polyCount; part++)
				for(size_t p = offset; p < offset + (size_t)polyCounts[part]; p ++)
					rect.expandRect(points[p]);

			ILineSymbolPtr pOutline = GetOutlineSymbol();
			if(pOutline.get())
			{
				GPoint testPoints[] = { GPoint(100, 100), GPoint(150, 100) };
				int size = 2;
				GRect r;
				pOutline->QueryBoundaryRectEx(pDisplay, testPoints, &size, 1, r);
				rect.xMin -= r.height() / 2;
				rect.yMin -= r.height() / 2;
				rect.xMax += r.height() / 2;
				rect.yMax += r.height() / 2;
			}
		}

		bool CSimpleFillSymbol::save(CommonLib::IWriteStream *pWriteStream) const
		{
			TSymbolBase::save(pWriteStream);
			pWriteStream->write((uint16)m_FillStyle);
			return true;
		}
		bool CSimpleFillSymbol::load(CommonLib::IReadStream* pReadStream)
		{
			if(!TSymbolBase::load(pReadStream))
				return false;
			 
			uint16 nStyle = SimpleFillStyleNull;
			SAFE_READ(pReadStream, nStyle)
			m_FillStyle = (eSimpleFillStyle)nStyle;
			return true;
		}


		//IXMLSerialize
		bool CSimpleFillSymbol::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			TSymbolBase::saveXML(pXmlNode);
			 pXmlNode->AddPropertyInt16(L"Style", m_FillStyle);
			return true;
		}
		bool CSimpleFillSymbol::load(GisCommon::IXMLNode* pXmlNode)
		{
			if(!TSymbolBase::load(pXmlNode))
				return false;
			m_FillStyle = (eSimpleFillStyle)pXmlNode->GetPropertyInt16(L"Style", SimpleFillStyleNull);
			return true;
		}

	}
}